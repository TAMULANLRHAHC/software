#include "tcp.h"
#include "haos.h" // your HAOS framework
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include <iostream>

int main() {
    /// SERVER SETUP ///
    std::string local_address = "127.0.0.1";
    HAOSInstance instance(local_address, 5000);

    /// TCP Client for Arduino ///
    static TCPClient arduino("192.168.1.116", 6000);

    int relay_start = 1;
    int relay_end = 8;

    /// CONTROL LOOP ///
    HAOS_ON_START(instance, "hand_init", [&](ThreadedLoop& loop) {
        upsertTag("HAOS:hand/connected.value", 0.0);

        for (int i = relay_start; i <= relay_end; i++) {
            insertTag("HAOS:hand/relay_" + std::to_string(i) + "_desired_state.value", 0.0);
            insertTag("HAOS:hand/relay_" + std::to_string(i) + "_watchdog_state.value", 0.0);
        }

        insertTag("HAOS:hand/watchdog_timeout_seconds.value", 10.0);
    });

    HAOS_ON_LOOP(instance, "hand_loop", [&](ThreadedLoop& loop) {
        static auto lastReconnect = std::chrono::steady_clock::now();

        // Attempt reconnect at most once per second
        if (!arduino.isConnected()) {
            arduino.tryReconnect();
        }

        // Send desired relay states to Arduino
        nlohmann::json out;
        for (int i = relay_start; i <= relay_end; i++) {
            double desired = queryTag<double>("HAOS:hand/relay_" + std::to_string(i) + "_desired_state.value", 0.0);
            double watchdog = queryTag<double>("HAOS:hand/relay_" + std::to_string(i) + "_watchdog_state.value", 0.0);
            out["relay_" + std::to_string(i) + "_desired_state"] = desired;
            out["relay_" + std::to_string(i) + "_watchdog_state"] = watchdog;
        }
        out["watchdog_timeout_seconds"] = queryTag<double>("HAOS:hand/watchdog_timeout_seconds.value", 10.0);

        arduino.sendJSON(out);

        // Receive telemetry
        nlohmann::json j;

        while (arduino.readJSON(j)) {
            upsertTag("HAOS:hand/connected.value", 1.0);

            for (auto it = j.begin(); it != j.end(); ++it) {
                double value = it.value().is_number() ? it.value().get<double>() : 0.0;
                upsertTag("HAOS:hand/" + it.key() + ".value", value);
            }
        }

        //connection tag
        if (arduino.hasHeartbeat()) {
            upsertTag("HAOS:hand/connected.value", 1.0);
        } else {
            upsertTag("HAOS:hand/connected.value", 0.0);
        }

    });
}
