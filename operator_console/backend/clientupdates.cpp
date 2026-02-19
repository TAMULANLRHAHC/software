//
// Created by kemptonburton on 8/29/2025.
//

#include "clientupdates.h"

#include <helperfunctions.h>

#include "haos.h"
#include "inmemorydatabase.h"
#include "servervariables.h"

//Sets up socket connections for client updates
void setup_socket_connections_for_client_updates(CrowSocket &socket) {

    //// DATABASE QUERYS ////
    //subscribe
    socket.on("database_viewer_subscribe_to_query", [&](crow::websocket::connection &conn, nlohmann::json data) {
        //record this connections most recent database query
        auto query  = removeQuotes(data.dump());

        //save query
        //lock mutex
        std::lock_guard<std::mutex> gaurd(socket.crow_websocket_clients_mutex);
        auto& query_list = socket.crow_websocket_clients[&conn]["most_recent_database_viewer_query"];
        if (query_list.empty()) {
            //if not found, add the query to first element
            query_list.push_back(query);
        } else {
            //if not empty, overwrite first element
            query_list[0] = query;
        }

    });
    //unsubscribe
    socket.on("database_viewer_unsubscribe_from_query", [&](crow::websocket::connection &conn, nlohmann::json data) {
        //delete most recent database query key from this clients data
        //lock mutex
        std::lock_guard<std::mutex> gaurd(socket.crow_websocket_clients_mutex);
        socket.crow_websocket_clients[&conn].erase("most_recent_database_viewer_query");
    });

    //// UPDATE TAG REQUESTS ////
    //Update Tag Requests from Client
   socket.on("update_tag", [&](crow::websocket::connection &conn, nlohmann::json data) {
       //unpack data. ensure to remove quotes.
       auto tag_path  = removeQuotes(data["tag_path"].dump());
       auto desired_value = removeQuotes(data["desired_value"].dump());

       //set the tag value
       auto result = upsertTag(tag_path, desired_value);
    });

    ///// TAG STREAMER /////
    //set new subscribe tag dictionary from client
    socket.on("set_tag_streamer_subscription_list", [&](crow::websocket::connection &conn, nlohmann::json data) {
        //get the list
        auto tag_streamer_subscription_list = data["tag_streamer_subscription_list"];

        //set the dictionary for this client
        //lock mutex
        std::lock_guard<std::mutex> gaurd(socket.crow_websocket_clients_mutex);
        socket.crow_websocket_clients[&conn]["tag_streamer_subscription_list"] = tag_streamer_subscription_list;
    });
}

void send_client_updates(CrowSocket &socket) {
    //go through each client connection

    //lock mutex
    std::lock_guard<std::mutex> guard(socket.crow_websocket_clients_mutex);

    for (const auto& [client_conn, client_data_map] : socket.crow_websocket_clients) {

        ///// DATABASE SEARCH QUERY /////
        //if this client is requesting a database query
        auto it = client_data_map.find("most_recent_database_viewer_query");
        if (it != client_data_map.end()) {
            //get query
            std::string query = it->second[0];

            //substring search
            auto result = substring_search_tag_data_map(query, 1000, tagData, tagDataMutex);

            //convert to json
            auto json = convertTagDataMapTypeToJson(result);
            nlohmann::json message;
            message["query_results"] = json;
            message["query"] = query;
            //send to client
            socket.emit("database_viewer_query_results", message, *client_conn);
        }

        ///// TAG STREAMER PUSHES /////
        //if this client has a tag subscription list
        it = client_data_map.find("tag_streamer_subscription_list");
        if (it != client_data_map.end()) {
            //tag data dictionary
            nlohmann::json data;
            data["microsecond_resolution_time_stamp"] = getCurrentTimeStampWithMicroseconds();

            //for eacth entry
            for (auto tag_streamer_subscription_list = it->second; const auto& tag_path: tag_streamer_subscription_list) {

                //get tag value as a string
                auto value = roundToDecimalPlaces(queryTag<std::string>(tag_path), 6);
                //add to data dictionary
                data[tag_path] = value;
            }

            //send to client
            socket.emit("tag_streamer_tag_data", data, *client_conn);
        }
    }
}

