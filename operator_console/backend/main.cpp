//
// Created by kemptonburton on 8/29/2025.
//

#include "haos.h"
#include "tcp.h"

int main() {

    /// SERVER SETUP ///
    std::string local_address = "127.0.0.1";
    HAOSInstance instance(local_address, 5000);

    /// TCP Client for remote Arduino ///
    static TCPClient arduino("192.168.1.116", 6000);

    int relay_start = 1;
    int relay_end = 8;


    /// CONTROL LOOP ///
    HAOS_ON_START(instance, "command_loop", [&](ThreadedLoop& loop) {
        /// INIT ///
        upsertTag("HAOS:hand/connected.value", 0.0);

        // relays
        for (int i = relay_start; i <= relay_end; i++) {
            //desired state
            std::string desired_state_tag = "HAOS:hand/relay_" + std::to_string(i) + "_desired_state.value";
            insertTag(desired_state_tag, 0.0);

            //watchdog state config option
            std::string watchdog_state_tag = "HAOS:hand/relay_" + std::to_string(i) + "_watchdog_state.value";
            insertTag(watchdog_state_tag, 0.0);
        }

        //configs
        insertTag("HAOS:hand/watchdog_timeout_seconds.value", 10.0);

    });

    HAOS_ON_LOOP(instance, "command_loop", [&](ThreadedLoop& loop) {

        /// SEND DESIRED STATE TO HAND ///
        nlohmann::json out;

        // RELAYS
        for (int i = relay_start; i <= relay_end; i++) {
            //desired state
            std::string desired_state_tag = "HAOS:hand/relay_" + std::to_string(i) + "_desired_state.value";
            double desired_state = queryTag<double>(desired_state_tag, 0.0);

            std::string desired_state_command = "relay_" + std::to_string(i) + "_desired_state";
            out[desired_state_command] = desired_state;

            //watchdog state
            std::string watchdog_state_tag = "HAOS:hand/relay_" + std::to_string(i) + "_watchdog_state.value";
            double watchdog_state = queryTag<double>(watchdog_state_tag, 0.0);

            std::string watchdog_state_command = "relay_" + std::to_string(i) + "_watchdog_state";
            out[watchdog_state_command] = watchdog_state;
        }

        // CONFIGS
        double watchdog_seconds = queryTag<double>("HAOS:hand/watchdog_timeout_seconds.value", 10.0);
        out["watchdog_timeout_seconds"] = watchdog_seconds;

        // SEND
        if (!arduino.sendJSON(out)) {
            upsertTag("HAOS:hand/connected.value", 0.0);
        } else {
            upsertTag("HAOS:hand/connected.value", 1.0);
        }

    });

    HAOS_ON_LOOP(instance, "telemetry_loop", [&](ThreadedLoop& loop) {

        /// RECIEVE ACTUAL STATE FROM HAND ///
        nlohmann::json j;
        if (arduino.readJSON(j)) {
            upsertTag("HAOS:hand/connected.value", 1.0);

            // ALL DATA
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();

                // Convert value to double (default to 0.0 if not a number)
                double value = 0.0;

                if (it.value().is_number()) {
                    value = it.value().get<double>();
                }

                // Construct tag name
                std::string tagName = "HAOS:hand/" + key + ".value";

                // Write to tag
                upsertTag(tagName, value);
            }
        } else {
            upsertTag("HAOS:hand/connected.value", 0.0);
        }

    });

}

