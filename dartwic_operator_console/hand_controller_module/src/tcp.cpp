#include "tcp.h"
#include <iostream>
#include <Poco/Exception.h>
#include <Poco/Timespan.h>

TCPClient::TCPClient(const std::string& host, int port)
    : host_(host), port_(port), connected_(false) {}

bool TCPClient::isConnected() const {
    return connected_;
}

bool TCPClient::hasHeartbeat() const {
    return heartbeat_recieved_;
}

bool TCPClient::sendJSON(const nlohmann::json& j) {
    if (!connected_) return false;

    try {
        std::string data = j.dump() + "\n";
        socket_.sendBytes(data.data(), static_cast<int>(data.size()));
        return true;
    } catch (const Poco::Exception& e) {
        std::cerr << "[TCP] Send failed: " << e.displayText() << "\n";
        connected_ = false;
        return false;
    }
}

bool TCPClient::readJSON(nlohmann::json& j) {
    if (!connected_) return false;

    try {
        static std::string buffer;

        // Non-blocking check (10 ms)
        if (socket_.poll(Poco::Timespan(0, 10000), Poco::Net::Socket::SELECT_READ)) {
            char temp[512];
            int n = socket_.receiveBytes(temp, sizeof(temp));
            if (n <= 0) {
                connected_ = false;
                std::cerr << "[TCP] Server closed connection!\n";
                return false;
            }

            buffer.append(temp, n);

            size_t pos;
            while ((pos = buffer.find('\n')) != std::string::npos) {
                std::string line = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);

                if (!line.empty()) {
                    j = nlohmann::json::parse(line);

                    // Check heartbeat
                    if (j.contains("heartbeat")) {
                        lastHeartbeat_ = std::chrono::steady_clock::now();
                        heartbeat_recieved_ = true;
                    }

                    return true;
                }
            }
        }

        // Check heartbeat timeout ONLY after we have recieved a valid heartbeat
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - lastHeartbeat_).count();
        if ((elapsed > heartbeatTimeoutSeconds_) && heartbeat_recieved_) {
            connected_ = false;
            heartbeat_recieved_ = false;
            std::cerr << "[TCP] Heartbeat timeout! No heartbeat in "
                      << elapsed << " seconds.\n";
        }

        return false; // no complete JSON yet
    } catch (const Poco::Exception& e) {
        std::cerr << "[TCP] Read failed: " << e.displayText() << "\n";
        connected_ = false;
        return false;
    }
}

void TCPClient::tryReconnect() {
    if (connected_) return;

    try {
        Poco::Net::SocketAddress addr(host_, port_);
        socket_ = Poco::Net::StreamSocket();

        // Connect timeout (5 second)
        Poco::Timespan timeout(5, 0);

        // connect
        socket_.connect(addr, timeout);

        // Recieve timeout (0.1 seconds)
        socket_.setReceiveTimeout(Poco::Timespan(0, 100000));

        connected_ = true;
        lastHeartbeat_ = std::chrono::steady_clock::now(); // RESET on reconnect

        std::cout << "[TCP] Connected to " << host_ << ":" << port_ << "\n";
    } catch (const Poco::Exception& e) {
        connected_ = false;
        std::cerr << "[TCP] Connect failed: " << e.displayText() << "\n";
    }
}
