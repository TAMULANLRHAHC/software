#pragma once
#include <string>
#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>

class TCPClient {
public:
    TCPClient(const std::string& ip, int port);
    ~TCPClient();

    TCPClient(const TCPClient&) = delete;
    TCPClient& operator=(const TCPClient&) = delete;

    bool isConnected() const;

    bool sendJSON(const nlohmann::json& j);
    bool readJSON(nlohmann::json& out);

    void startBackgroundConnect(int retry_ms = 1000);
    void stopBackgroundConnect();

private:
    void disconnect();

    std::string ip_;
    std::string port_;

    asio::io_context io_;
    asio::ip::tcp::socket socket_;
    mutable std::mutex socket_mutex_;
    bool connected_;

    std::thread connect_thread_;
    std::atomic<bool> run_thread_;
};
