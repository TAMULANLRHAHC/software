#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

#include "main.h"

// MAC address — must be unique on your network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

// Static IP — change to match your LAN
IPAddress ip(192, 168, 1, 116);

// TCP server port — use high port to avoid conflicts
EthernetServer server(6000);

// Persistent client object
EthernetClient currentClient;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting W5500...");

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Initialize SPI CS pin
    Ethernet.init(10);

    // Static IP
    Ethernet.begin(mac, ip);

    Serial.print("Server IP: ");
    Serial.println(Ethernet.localIP());

    server.begin();
    Serial.print("TCP server started on port ");
    Serial.println(6000);
}

void loop() {
    // Accept new client if we don't have one or previous disconnected
    if (!currentClient || !currentClient.connected()) {
        EthernetClient newClient = server.available();
        if (newClient) {
            currentClient = newClient;
            Serial.println("Client connected!");
        }
    }

    //// CLIENT CONNECTED ////
    if (currentClient && currentClient.connected() && currentClient.available()) {

        /// RECIEVE INCOMING DATA & RUN CONTROL LOOP ///
        String jsonLine = currentClient.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(jsonLine);

        incoming_data.clear();  // reset previous data
        auto err = deserializeJson(incoming_data, jsonLine);
        if (!err) {
            run_control_iteration();
        } else {
            Serial.print("Error Processing Incoming Data: ");
            Serial.println(err.c_str());
        }

        /// SEND OUTGOING DATA ///
        String buffer;
        serializeJson(outgoing_data, buffer);
        buffer += "\n";  // newline framing
        currentClient.print(buffer);
    }

    // Check if client disconnected
    if (currentClient && !currentClient.connected()) {
        Serial.println("Client disconnected");
        currentClient.stop();
    }
}


void run_control_iteration() {
    int led_state = incoming_data["led_state"];

    if (led_state == 1) {
        digitalWrite(2, HIGH);
        outgoing_data["led_state"] = 1;
    } else {
        digitalWrite(2, LOW);
        outgoing_data["led_state"] = 0;
    }

    int raw = analogRead(A0);
    float voltage = raw * (5.0 / 1023.0);
    outgoing_data["A0_voltage"] = voltage;
}


