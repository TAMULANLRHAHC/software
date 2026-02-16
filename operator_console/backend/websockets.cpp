//
// Created by kemptonburton on 8/29/2025.
//

//Local Files
#include "websockets.h"

//Standard Libraries
#include <clientupdates.h>
#include <haos.h>
#include <iostream>
#include <utility>
#include "servervariables.h"

//CALLED ON INSTANCE CREATION
void setup_socket_connections(CrowSocket &socket) {
    //Setup socket connections for client updates
    setup_socket_connections_for_client_updates(socket);
}

// Constructor definition
CrowSocket::CrowSocket(crow::SimpleApp& app) {
    // Init websocket endpoint
    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([this](crow::websocket::connection& conn) {
        std::lock_guard<std::mutex> guard(crow_websocket_clients_mutex);
        crow_websocket_clients[&conn] = {};
    })
    .onclose([this](crow::websocket::connection& conn, const std::string& reason) {
        std::lock_guard<std::mutex> guard(crow_websocket_clients_mutex);
        for (auto it = crow_websocket_clients.begin(); it != crow_websocket_clients.end(); ) {
            if (it->first == &conn) {
                it = crow_websocket_clients.erase(it);
            } else {
                ++it;
            }
        }
    })
    .onmessage([this](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
        process_event(conn, data);
    });
}

// emit function definition
void CrowSocket::emit(const std::string& event_name, const nlohmann::json& data, std::optional<std::reference_wrapper<crow::websocket::connection>> conn) {
    // Create the message as JSON
    const nlohmann::json message = {
        {"event_name", event_name},
        {"data", data}
    };

    // Send serialized data to the specified connection
    if (conn.has_value()) {
        conn->get().send_text(message.dump());
    } else {
        //lock mutex
        std::lock_guard<std::mutex> guard(crow_websocket_clients_mutex);
        // Broadcast to all connected clients
        for (const auto& [client_conn, client_data_map] : crow_websocket_clients) {
            client_conn->send_text(message.dump());
        }
    }
}

// on function definition
void CrowSocket::on(const std::string& event_name, std::function<void(crow::websocket::connection& conn, nlohmann::basic_json<>)> callback_function) {
    // Add callback_function to event dictionary
    event_dict[event_name] = std::move(callback_function);
}

// off function definition
void CrowSocket::off(const std::string& event_name) {
    // Remove dictionary entry that matches event name
    event_dict.erase(event_name);
}

// process_event function definition
void CrowSocket::process_event(crow::websocket::connection& conn, const std::string& message) {
    // Deserialize the incoming JSON message
    try {
        auto received_message = nlohmann::json::parse(message);
        std::string event_name = received_message["event_name"];
        nlohmann::basic_json<> data = received_message["data"];

        // Call the proper callback function if it exists in event dictionary
        for (const auto&[callback_event_name, callback_function] : event_dict) {
            // If event name matches
            if (callback_event_name == event_name) {
                // Invoke callback function with data
                callback_function(conn, data);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error parsing message: " << e.what() << std::endl;
    }

}