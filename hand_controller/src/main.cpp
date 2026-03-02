#include "main.h"

// TCP server
EthernetServer server(SERVER_PORT);

// Persistent client object
EthernetClient currentClient;

/* ---------- BAKED-IN TELEMETRY ---------- /
 * "heatbeat"
 */


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting W5500...");

    // Optional status pin
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Initialize SPI CS pin
    Ethernet.init(ETHERNET_SPI_CS_PIN);

    // Static IP
    Ethernet.begin(mac, ip);

    Serial.print("Server IP: ");
    Serial.println(Ethernet.localIP());

    server.begin();
    Serial.print("TCP server started on port ");
    Serial.println(SERVER_PORT);

    // Initialize relay pins as outputs
    for (int i = 0; i < RELAY_COUNT; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
    }

    lastPacketTime = millis();
}

void loop() {

    // Accept new client if none connected
    if (!currentClient || !currentClient.connected()) {
        EthernetClient newClient = server.available();
        if (newClient) {
            currentClient = newClient;
            Serial.println("Client connected!");
        }
    }

    //// CLIENT CONNECTED ////
    if (currentClient && currentClient.connected() && currentClient.available()) {

        /// RECEIVE INCOMING DATA ///
        String jsonLine = currentClient.readStringUntil('\n');

        incoming_data.clear();
        outgoing_data.clear();

        auto err = deserializeJson(incoming_data, jsonLine);
        // Serial.print("Received: ");
        // serializeJsonPretty(incoming_data, Serial);

        if (!err) {
            lastPacketTime = millis();
            watchdogTriggered = false;
            run_connected_control_iteration();
        } else {
            Serial.print("Error Processing Incoming Data: ");
            Serial.println(err.c_str());
        }

        /// SEND OUTGOING DATA ///
        outgoing_data["heartbeat"] = millis(); //bake in heartbeat
        String buffer;
        serializeJson(outgoing_data, buffer);
        buffer += "\n";
        currentClient.print(buffer);

        // Serial.print("Outoging: ");
        // serializeJsonPretty(outgoing_data, Serial);
    }

    // -------- WATCHDOG CHECK -------- //
    if (watchdogTimeoutMs > 0) {
        if (millis() - lastPacketTime > watchdogTimeoutMs) {
            if (!watchdogTriggered) {
                Serial.println("WATCHDOG TIMEOUT");
                run_watchdog_control_iteration();
                watchdogTriggered = true;
            }
        }
    }

    // Client disconnect handling
    if (currentClient && !currentClient.connected()) {
        Serial.println("Client disconnected");
        currentClient.stop();
        currentClient = EthernetClient();  // reset to a fresh object
    }
}
