#include "tcp.h"
#include <chrono>

TCPClient::TCPClient(const std::string& ip, int port)
    : ip_(ip), port_(std::to_string(port)),
      socket_(io_), connected_(false), run_thread_(false) {
    startBackgroundConnect();
}

TCPClient::~TCPClient() {
    stopBackgroundConnect();
    disconnect();
}

bool TCPClient::isConnected() const {
    return connected_;
}

void TCPClient::disconnect() {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_.is_open()) {
        asio::error_code ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }
    connected_ = false;
}

bool TCPClient::sendJSON(const nlohmann::json& j) {
    asio::ip::tcp::socket* socket_ptr = nullptr;

    {
        std::lock_guard<std::mutex> lock(socket_mutex_);
        if (!connected_ || !socket_.is_open()) return false;
        socket_ptr = &socket_;
    }

    try {
        std::string data = j.dump() + "\n";
        asio::error_code ec;
        asio::write(*socket_ptr, asio::buffer(data), ec);
        if (ec) {
            std::cerr << "[TCP] Send failed: " << ec.message() << "\n";
            disconnect(); // reconnect thread will handle reconnect
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[TCP] Send exception: " << e.what() << "\n";
        disconnect();
        return false;
    }
}

bool TCPClient::readJSON(nlohmann::json& out) {
    asio::ip::tcp::socket* socket_ptr = nullptr;

    // Lock only to check connection and get a pointer
    {
        std::lock_guard<std::mutex> lock(socket_mutex_);
        if (!connected_ || !socket_.is_open()) return false;
        socket_ptr = &socket_;
    }

    try {
        // Flush leftover bytes (non-blocking)
        asio::error_code ec;
        while (socket_ptr->available(ec) > 0) {
            char discard[1024];
            socket_ptr->read_some(asio::buffer(discard, 1024), ec);
            if (ec) break;
        }

        // Blocking read (no mutex held)
        asio::streambuf buffer;
        asio::read_until(*socket_ptr, buffer, "\n", ec);
        if (ec) {
            std::cerr << "[TCP] Read failed: " << ec.message() << "\n";
            disconnect();  // reconnect will take care of it
            return false;
        }

        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);
        if (line.empty()) return false;

        out = nlohmann::json::parse(line);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[TCP] Read failed: " << e.what() << "\n";
        disconnect();
        return false;
    }
}



void TCPClient::startBackgroundConnect(int retry_ms) {
    if (run_thread_) return;
    run_thread_ = true;

    connect_thread_ = std::thread([this, retry_ms]() {
        while (run_thread_) {
            if (!connected_) {
                try {
                    asio::ip::tcp::resolver resolver(io_);
                    auto endpoints = resolver.resolve(ip_, port_);
                    asio::ip::tcp::socket temp_socket(io_);
                    asio::connect(temp_socket, endpoints);

                    std::lock_guard<std::mutex> lock(socket_mutex_);
                    socket_ = std::move(temp_socket);

                    // flush leftover bytes
                    asio::error_code ec;
                    while (socket_.available(ec) > 0) {
                        char discard[1024];
                        socket_.read_some(asio::buffer(discard, 1024), ec);
                        if (ec) break;
                    }

                    connected_ = true;
                    std::cout << "[TCP] Connected to " << ip_ << ":" << port_ << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[TCP] Connect failed: " << e.what() << "\n";
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_ms));
        }
    });
}

void TCPClient::stopBackgroundConnect() {
    run_thread_ = false;
    if (connect_thread_.joinable()) connect_thread_.join();
}
