//
// Created by kemptonburton on 8/29/2025.
//

#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

//Vcpkg Libraries
#include <crow.h>
#include <nlohmann/json.hpp>

//Standard Libraries
#include <map>
#include <functional>
#include <string>

class CrowSocket {
public:
    // Constructor declaration
    explicit CrowSocket(crow::SimpleApp& app);

    // Sends message in CrowSocket event format
    void emit(const std::string& event_name, const nlohmann::json& data, std::optional<std::reference_wrapper<crow::websocket::connection>> conn = std::nullopt);

    // Adds function with event to dictionary
    void on(const std::string& event_name, std::function<void(crow::websocket::connection& conn, nlohmann::basic_json<>)> callback_function);

    // Removes function with matching event from dictionary
    void off(const std::string& event_name);

    // map to hold currently connected clients
    std::unordered_map<crow::websocket::connection*, std::unordered_map<std::string, std::vector<std::string>>> crow_websocket_clients;
    // mutex for map
    std::mutex crow_websocket_clients_mutex;

private:
    // Private member variables
    // map to hold current call back functions with event strings
    std::map<std::string, std::function<void(crow::websocket::connection& conn, nlohmann::basic_json<> data)>> event_dict;

    // When message is received
    void process_event(crow::websocket::connection& conn, const std::string& message);
};

void setup_socket_connections(CrowSocket &socket);

#endif //WEBSOCKETS_H
