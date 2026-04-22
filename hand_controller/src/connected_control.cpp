#include "main.h"

namespace {
uint16_t read_voltage_millivolts(Adafruit_ADS1115& ads, int channel) {
    int16_t raw = ads.readADC_SingleEnded(channel);
    float volts = raw * 0.0001875f;

    if (volts < 0.0f) {
        volts = 0.0f;
    }

    return static_cast<uint16_t>(volts * 1000.0f);
}

uint16_t read_internal_voltage_millivolts(int pin) {
    const int raw = analogRead(pin);
    const float volts = (static_cast<float>(raw) * 5.0f) / 1023.0f;
    return static_cast<uint16_t>(volts * 1000.0f);
}
}

void configure_modbus() {
    modbusTCPServer.configureCoils(RELAY_COIL_START, COIL_COUNT);
    modbusTCPServer.configureInputRegisters(
        VOLTAGE_INPUT_REGISTER_START,
        INPUT_REGISTER_COUNT
    );

    for (int i = 0; i < RELAY_COUNT; ++i) {
        modbusTCPServer.coilWrite(RELAY_COIL_START + i, 0);
        modbusTCPServer.coilWrite(WATCHDOG_COIL_START + i, relay_watchdog_states[i]);
    }
}

void update_input_registers() {
    for (int device = 0; device < 2; ++device) {
        for (int channel = 0; channel < 4; ++channel) {
            const int inputRegister = VOLTAGE_INPUT_REGISTER_START + (device * 4) + channel;
            modbusTCPServer.inputRegisterWrite(
                inputRegister,
                read_voltage_millivolts(*ads_list[device], channel)
            );
        }
    }

    for (int i = 0; i < INTERNAL_ANALOG_COUNT; ++i) {
        modbusTCPServer.inputRegisterWrite(
            VOLTAGE_INPUT_REGISTER_START + ADS_ANALOG_COUNT + i,
            read_internal_voltage_millivolts(INTERNAL_ANALOG_PINS[i])
        );
    }

    for (int i = 0; i < RELAY_COUNT; ++i) {
        const int state = digitalRead(RELAY_PINS[i]) == HIGH ? 1 : 0;
        modbusTCPServer.inputRegisterWrite(RELAY_STATE_INPUT_REGISTER_START + i, state);
    }
}

void apply_live_relay_states() {
    for (int i = 0; i < RELAY_COUNT; ++i) {
        const int relayState = modbusTCPServer.coilRead(RELAY_COIL_START + i) ? HIGH : LOW;
        const int watchdogState = modbusTCPServer.coilRead(WATCHDOG_COIL_START + i) ? 1 : 0;

        relay_watchdog_states[i] = watchdogState;
        digitalWrite(RELAY_PINS[i], relayState);
    }
}
