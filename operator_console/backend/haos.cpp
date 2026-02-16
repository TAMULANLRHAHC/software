
// Created by kemptonburton on 8/29/2025.
//

#include "haos.h"

#include <clientupdates.h>
#include <servervariables.h>
#include "webserver.h"

HAOSInstance* globalInstance = nullptr;

HAOSInstance::HAOSInstance(
    const std::string &crow_web_server_ip,
    const int &crow_web_server_port)
    //initializer list
    : crow_web_server_ip(crow_web_server_ip),
      crow_web_server_port(crow_web_server_port), crow_web_server_socket(crow_web_server) {


    //set global reference
    globalInstance = this;

    ///// MISC /////
    upsertTag("INSTANCE:instancevariables/instance_data_source.value", dataSourceName);

    ///// CROW SERVER AND SOCKET /////
    //thread to keep crow server running
    crow_web_server_thread = create_web_server(crow_web_server, crow_web_server_ip, crow_web_server_port);
    //setup all socket connections
    setup_socket_connections(crow_web_server_socket);

    ///// CLIENT UPDATES /////
    //create a thread to run client updates
    insertTag(dataSourceName+":servervariables/loops." + this->client_update_loop_name + ".target_loop_frequency.value", DEFAULT_CLIENT_UPDATE_TARGET_FREQUENCY);
    this->setThreadedLoop(client_update_loop_name, "start", [this](ThreadedLoop &threadedLoopObject) {
    });
    this->setThreadedLoop(client_update_loop_name, "loop", [this](ThreadedLoop &threadedLoopObject) {
        //in loop
        //send updates to clients
        send_client_updates(crow_web_server_socket);
    });
    this->setThreadedLoop(client_update_loop_name, "end", [](ThreadedLoop &threadedLoopObject) {
    });

    ///// GENERAL PROGRAM UPDATES
    //DEFAULT LOOP TO 10 TIMES A SECOND
    this->setThreadedLoop(general_program_checks_loop_name, "loop", [this](ThreadedLoop &threadedLoopObject) {

        //also run general control loop - for general things that need to be watched and modified
        run_general_program_updates(*this);

        });
}

void HAOSInstance::setThreadedLoop(const std::string &loop_name, const std::string &loop_function_condition,
                                               const std::function<void(ThreadedLoop &threadedLoopObject)> &function) {

    auto it = loop_threads.find(loop_name);
    // If loop name NOT in loop_threads unordered map
    if (it == loop_threads.end()) {
        //create object and place into map
        loop_threads.emplace(std::piecewise_construct, std::forward_as_tuple(loop_name), std::forward_as_tuple(loop_name));
        //get object from map
        it = loop_threads.find(loop_name);
    }

    //depending on function configuration, call it on start, loop, or end
    if (loop_function_condition == "start") {
        it->second.setOnStart(function);
    }
    else if (loop_function_condition == "loop") {
        it->second.setOnLoop(function);
    }
    else if (loop_function_condition == "end") {
        it->second.setOnEnd(function);
    }
}

std::unordered_map<std::string, ThreadedLoop>& HAOSInstance::getLoops() {
    return this->loop_threads;
}

//before instance goes out of scope (before program ends) we need to join the server thread
//so the main thread stays open (waits for crow server to stop)
HAOSInstance::~HAOSInstance() {

    //join crow server thread before object is deconstructed.
    if (this->crow_web_server_thread.joinable()) {
        this->crow_web_server_thread.join();
    }

    //before threads go out of scope, they should be joined to the main thread.
    //loop threads are not joined however, as for some reason, it causes errors.
    //dont think they need to be joined however, as main thread will wait untill crow server has closed.
}

void HAOSInstance::run_general_program_updates(HAOSInstance &instance) {
    
    ///// THREADED LOOP RUNNING WATCHING /////
    //loop through all threaded loops as a reference
    for (auto &loop : instance.getLoops()) {

        //get loop is running tag value
        int running = queryTag<double>(dataSourceName+":servervariables/loops." + loop.first + ".running.value");

        //if loop is NOT running internally
        if (!loop.second.running) {
            //if tag value is set to 1, call start function
            if (running == 1) {
                loop.second.start();
            }
            //if loop is running internally
        } else {
            //if tag value is set to 0, call stop function
            if (running == 0) {
                loop.second.stop();
            }
        }
    }
    
}

CrowSocket & HAOSInstance::getSocket() {
    return crow_web_server_socket;
}