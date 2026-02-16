//
// Created by kemptonburton on 8/29/2025.
//

#ifndef HAOS_H
#define HAOS_H

#include <threading.h>
#include <websockets.h>
#include <crow/app.h>
#include "inmemorydatabase.h"

#define HAOS_ON_START(HAOS_instance, loop_name, function) HAOS_instance.setThreadedLoop(loop_name, "start", function)
#define HAOS_ON_LOOP(HAOS_instance, loop_name, function) HAOS_instance.setThreadedLoop(loop_name, "loop", function)
#define HAOS_ON_END(HAOS_instance, loop_name, function) HAOS_instance.setThreadedLoop(loop_name, "end", function)


////IMPORTANT GLOBAL VARIABLES////
extern std::chrono::steady_clock::time_point steadyClockReference;
extern std::chrono::system_clock::time_point systemClockReference;

// only one should be created per cmake project
class HAOSInstance {
public:

    // constructor [ip, port, mongodb_uri]
    HAOSInstance(const std::string &crow_web_server_ip, const int &crow_web_server_port);

    void setThreadedLoop(const std::string &loop_name, const std::string &loop_function_condition, const std::function<void(ThreadedLoop& threadedLoopObject)> &function);

    std::unordered_map<std::string, ThreadedLoop>& getLoops();

    void run_general_program_updates(HAOSInstance &instance);

    CrowSocket &getSocket();

    ~HAOSInstance();

private:
    //// WEB SERVER ////
    std::string crow_web_server_ip;
    int crow_web_server_port;

    //crow web server (since its constructor takes in no arguments, a default constructor, it will
    //automatically create an instace without needing to explicitly create one in the constructor.
    crow::SimpleApp crow_web_server;
    //crow web server thread
    std::thread crow_web_server_thread;
    //socket connections for web server
    CrowSocket crow_web_server_socket;

    /// CLIENT UPDATES ///
    std::string client_update_loop_name = "client_update_thread";

    /// GENERAL UPDATES ///
    std::string general_program_checks_loop_name = "general_program_checks_thread";

    //// THREADED LOOPS ////
    std::unordered_map<std::string, ThreadedLoop> loop_threads;
};

extern HAOSInstance* globalInstance;

#endif //HAOS_H
