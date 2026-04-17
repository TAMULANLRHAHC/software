//
// Created by kemptonburton on 8/29/2025.
//

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <thread>
#include <crow/app.h>

std::thread create_web_server(crow::SimpleApp &app, const std::string &ip, int port);
void setup_routes(crow::SimpleApp& app);

#endif //WEBSERVER_H
