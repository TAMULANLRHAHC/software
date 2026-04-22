#include "main.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting hand controller Modbus server...");

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    for (int i = 0; i < RELAY_COUNT; ++i) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
    }

    Wire.begin();
    Wire.setClock(400000);

    if (!ads1.begin(0x49)) {
        Serial.println("Failed to initialize ADS1115 #1");
    }

    if (!ads2.begin(0x48)) {
        Serial.println("Failed to initialize ADS1115 #2");
    }

    ads1.setGain(GAIN_TWOTHIRDS);
    ads2.setGain(GAIN_TWOTHIRDS);
    ads1.setDataRate(RATE_ADS1115_860SPS);
    ads2.setDataRate(RATE_ADS1115_860SPS);

    Ethernet.init(ETHERNET_SPI_CS_PIN);
    Ethernet.begin(mac, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.");
        while (true) {
            delay(1);
        }
    }

    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    ethServer.begin();

    if (!modbusTCPServer.begin()) {
        Serial.println("Failed to start Modbus TCP server.");
        while (true) {
            delay(1);
        }
    }

    configure_modbus();
    update_input_registers();

    lastCoilUpdateTime = millis();
    lastInputRegisterUpdateTime = millis();

    Serial.print("Modbus TCP server IP: ");
    Serial.println(Ethernet.localIP());
    Serial.println("Listening on port 502");
}

void loop() {
    EthernetClient client = ethServer.available();

    if (client) {
        modbusTCPServer.accept(client);
        Serial.println("Modbus client connected");

        while (client.connected()) {
            int requestsHandled = 0;

            if (client.available() > 0) {
                requestsHandled = modbusTCPServer.poll();
            }

            if (requestsHandled > 0) {
                lastCoilUpdateTime = millis();
                watchdogTriggered = false;
                apply_live_relay_states();
            }

            if ((millis() - lastCoilUpdateTime) > WATCHDOG_TIMEOUT_MS) {
                apply_watchdog_relay_states();
            }

            if ((millis() - lastInputRegisterUpdateTime) >= INPUT_REGISTER_UPDATE_INTERVAL_MS) {
                update_input_registers();
                lastInputRegisterUpdateTime = millis();
            }
        }

        Serial.println("Modbus client disconnected");
    } else {
        if ((millis() - lastCoilUpdateTime) > WATCHDOG_TIMEOUT_MS) {
            apply_watchdog_relay_states();
        }

        if ((millis() - lastInputRegisterUpdateTime) >= INPUT_REGISTER_UPDATE_INTERVAL_MS) {
            update_input_registers();
            lastInputRegisterUpdateTime = millis();
        }
    }
}
