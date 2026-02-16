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

    /// CONTROL LOOP ///
    HAOS_ON_LOOP(instance, "DACS_LOOP", [&](ThreadedLoop& loop) {

        /// CONNECT TO HAND TCP SERVER ///
        if (!arduino.isConnected()) {
            std::cout << "[TCP] Not connected, trying to connect...\n";
            arduino.connect();
        }

        /// SEND DESIRED STATE TO HAND ///
        nlohmann::json out;

        double led_state_desired = queryTag<double>("HAOS:hand/led.desired_state.value", 0.0);
        out["led_state"] = led_state_desired;

        arduino.sendJSON(out);

        /// RECIEVE ACTUAL STATE FROM HAND ///
        nlohmann::json j;
        if (arduino.readJSON(j)) {

            double led_actual_state = j.value("led_state", 0.0);
            upsertTag("HAOS:hand/led.actual_state.value", led_actual_state);

            double A0_voltage = j.value("A0_voltage", 0.0);
            upsertTag("HAOS:hand/A0_voltage.value", A0_voltage);

        }

    });

}