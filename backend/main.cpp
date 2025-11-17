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
    TCPClient arduino("192.168.1.116", 6000);

    /// CONTROL LOOP ///
    HAOS_ON_LOOP(instance, "DACS_LOOP", [&](ThreadedLoop& loop) {

        // 1. Try to connect if not connected
        if (!arduino.isConnected()) {
            std::cout << "[TCP] Not connected, trying to connect...\n";
            if (arduino.connectBlocking()) {
                std::cout << "[TCP] Connected!\n";
            } else {
                std::cout << "[TCP] Connect failed\n";
            }
            return;
        }

        // 2. Receive packet (blocking)
        std::string packet;
        if (arduino.receiveLine(packet)) {
            std::cout << "[RX] " << packet << "\n";
        }

        // 3. Send packet
        arduino.sendLine("LED_ON");

    });

}