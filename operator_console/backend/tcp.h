#pragma once
#include <string>
#include <queue>
#include <nlohmann/json.hpp>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>

class TCPClient {
public:
    TCPClient(const std::string& host, int port);

    bool isConnected() const;

    bool hasHeartbeat() const;

    // Non-blocking send
    bool sendJSON(const nlohmann::json& j);

    // Non-blocking receive; returns true if a JSON object was received
    bool readJSON(nlohmann::json& out);

    // Attempt reconnect if disconnected
    void tryReconnect();

private:
    std::string host_;
    int port_;
    bool connected_ = false;

    std::chrono::steady_clock::time_point lastHeartbeat_;
    double heartbeatTimeoutSeconds_ = 2.0; // max seconds allowed without heartbeat
    bool heartbeat_recieved_ = false;


    Poco::Net::StreamSocket socket_;
};
