//
// Created by kemptonburton on 11/14/2025.
//

#ifndef MYMODULE_H
#define MYMODULE_H

#include <iostream>
#include <BaseModule.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "tcp.h"

class MyModule : public DARTWIC::Modules::BaseModule {
public:
    MyModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) : BaseModule(cfg, drtw) {
        // ---------- CREATE TCP OBJECT ---------- //
        std::string server_ip = getParameter<std::string>("server_ip");
        int server_port = getParameter<int>("server_port");

        tcp = std::make_unique<TCPClient>(server_ip, server_port);

        // ---------- CREATE CAESAR LOOPS ---------- //
        device_name = getParameter<std::string>("device_name");



        dartwic->onStart("dacs_loop", [&]() {
            dartwic->upsertChannelValue(device_name, "connected", 0.0);

            dartwic->insertChannelValue(device_name, "watchdog_timeout_seconds", 10.0);
        });

        // DACS LOOP
        dartwic->onLoop("dacs_loop", [&]() {
            static auto lastReconnect = std::chrono::steady_clock::now();

            // WHEN NOT CONNECTED
            if (!tcp->isConnected()) {
                // attempt reconnect
                tcp->tryReconnect();
            }

            // Send desired relay states to tcp
            nlohmann::json out;
            for (int i = relay_start; i <= relay_end; i++) {
                double desired = dartwic->queryChannelValue(device_name, "relay_" + std::to_string(i) + "_desired_state", 0.0);
                double watchdog = dartwic->queryChannelValue(device_name, "relay_" + std::to_string(i) + "_watchdog_state", 0.0);

                out["relay_" + std::to_string(i) + "_desired_state"] = desired;
                out["relay_" + std::to_string(i) + "_watchdog_state"] = watchdog;
            }
            out["watchdog_timeout_seconds"] = dartwic->queryChannelValue(device_name, "watchdog_timeout_seconds", 10.0);

            tcp->sendJSON(out);

            // Receive telemetry
            nlohmann::json j;

            while (tcp->readJSON(j)) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    double value = it.value().is_number() ? it.value().get<double>() : 0.0;
                    dartwic->upsertChannelValue(device_name, it.key(), value);
                }
            }

            //connection tag
            if (tcp->hasHeartbeat()) {
                dartwic->upsertChannelValue(device_name, "connected", 1.0);
            } else {
                dartwic->upsertChannelValue(device_name, "connected", 0.0);

                std::string title = "TCP Client Heartbeat Error [" + device_name + "]";
                std::string tag = device_name + "/connected.value";
                std::string description = "The TCP client lost heartbeat connection to the TCP server on the given IP and Port.";

                dartwic->consoleError(title,
                    description,
                    {tag},
                    "Ensure the configured IP and Port is correct, both on the server and client device. "
                    "Ensure any wire connections are not broken, and that the network router and device the TCP "
                    "client is trying to connect to has power. Cylce the power to the TCP server if needed.",
                    5
                );
            }
        });

    };

    ~MyModule() override {
        dartwic->removeLoop("dacs_loop");
    };

private:
    std::unique_ptr<TCPClient> tcp;

    std::string device_name;
    int relay_start = 1;
    int relay_end = 8;

};

#endif //MYMODULE_H

