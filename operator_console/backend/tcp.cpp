#include "tcp.h"
#include <iostream>

using asio::ip::tcp;

// define static member
asio::io_context TCPClient::io_;

// ------------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------------

TCPClient::TCPClient(const std::string ip, const int port)
    : ip_(ip),
      port_(std::to_string(port)),
      socket_(io_),
      timer_(io_) {}


TCPClient::~TCPClient() {
    disconnect();
}

// ------------------------------------------------------------------
// Connect
// ------------------------------------------------------------------

bool TCPClient::connect() {
    asio::error_code ec;

    tcp::resolver resolver(io_);
    auto endpoints = resolver.resolve(ip_, port_, ec);
    if (ec) {
        std::cerr << "[TCP] Resolve error: " << ec.message() << "\n";
        return false;
    }

    asio::connect(socket_, endpoints, ec);
    if (ec) {
        std::cerr << "[TCP] Connect error: " << ec.message() << "\n";
        return false;
    }

    std::cout << "[TCP] Connected to " << ip_ << ":" << port_ << "\n";
    return true;
}

// ------------------------------------------------------------------
// Disconnect
// ------------------------------------------------------------------

void TCPClient::disconnect() {
    if (socket_.is_open()) {
        asio::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }
}

bool TCPClient::isConnected() const {
    return socket_.is_open();
}

// ------------------------------------------------------------------
// Send JSON (newline terminated)
// ------------------------------------------------------------------

bool TCPClient::sendJSON(const nlohmann::json& j) {
    if (!isConnected()) return false;

    std::string data = j.dump() + "\n";

    asio::error_code ec;
    asio::write(socket_, asio::buffer(data), ec);

    if (ec) {
        std::cerr << "[TCP] Send error: " << ec.message() << "\n";
        return false;
    }

    return true;
}

// ------------------------------------------------------------------
// Read JSON (blocking until '\n')
// ------------------------------------------------------------------

bool TCPClient::readJSON(nlohmann::json& out, int timeout_ms)
{
    if (!isConnected())
        return false;

    asio::streambuf buffer;
    asio::error_code ec;

    bool read_finished = false;
    bool timed_out = false;

    // --------------- SETUP TIMEOUT ---------------
    timer_.expires_after(std::chrono::milliseconds(timeout_ms));
    timer_.async_wait([&](const asio::error_code& err){
        if (!err) {        // timer expired
            timed_out = true;
            socket_.cancel();   // force read to stop
        }
    });

    // --------------- START ASYNC READ ---------------
    asio::async_read_until(
        socket_,
        buffer,
        "\n",
        [&](const asio::error_code& read_err, std::size_t){
            ec = read_err;
            read_finished = true;
            timer_.cancel();   // stop timer if read succeeded
        }
    );

    // --------------- RUN TEMPORARY EVENT LOOP ---------------
    while (!read_finished && !timed_out)
        io_.run_one();

    io_.restart(); // ready for next op

    // --------------- HANDLE TIMEOUT ---------------
    if (timed_out) {
        std::cerr << "[TCP] Read timeout\n";
        return false;
    }

    // --------------- HANDLE READ ERRORS ---------------
    if (ec) {
        if (ec == asio::error::operation_aborted)
            return false;  // was canceled by timeout

        std::cerr << "[TCP] Read error: " << ec.message() << "\n";
        return false;
    }

    // --------------- PARSE JSON ---------------
    std::istream is(&buffer);
    std::string line;
    std::getline(is, line);

    if (line.empty())
        return false;

    try {
        out = nlohmann::json::parse(line);
    }
    catch (const std::exception& e) {
        std::cerr << "[TCP] JSON parse error: " << e.what() << "\n";
        return false;
    }

    return true;
}

