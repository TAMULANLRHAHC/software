#pragma once

#include <string>
#include <asio.hpp>
#include <nlohmann/json.hpp>

class TCPClient {
public:
    TCPClient(const std::string ip, const int port);
    ~TCPClient();

    // Disable copy
    TCPClient(const TCPClient&) = delete;
    TCPClient& operator=(const TCPClient&) = delete;

    // Allow move (optional)
    TCPClient(TCPClient&&) = default;
    TCPClient& operator=(TCPClient&&) = default;

    bool connect();
    void disconnect();
    bool isConnected() const;

    bool sendJSON(const nlohmann::json& j);
    bool readJSON(nlohmann::json& out, int timeout_ms = 500);

private:
    std::string ip_;
    std::string port_;

    static asio::io_context io_;
    asio::ip::tcp::socket socket_;
    asio::steady_timer timer_;

};
