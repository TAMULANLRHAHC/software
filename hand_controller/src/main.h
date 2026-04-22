#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

extern byte mac[];
extern IPAddress ip;
extern const int ETHERNET_SPI_CS_PIN;

extern const int RELAY_NUM_START;
extern const int RELAY_NUM_END;
extern const int RELAY_COUNT;
extern const int RELAY_PINS[];

extern const int ADS_ANALOG_NUM_START;
extern const int ADS_ANALOG_NUM_END;
extern const int ADS_ANALOG_COUNT;

extern const int INTERNAL_ANALOG_COUNT;
extern const int INTERNAL_ANALOG_NUM_START;
extern const int INTERNAL_ANALOG_NUM_END;
extern const int INTERNAL_ANALOG_PINS[];

extern const uint16_t RELAY_COIL_START;
extern const uint16_t WATCHDOG_COIL_START;
extern const uint16_t COIL_COUNT;

extern const uint16_t VOLTAGE_INPUT_REGISTER_START;
extern const uint16_t RELAY_STATE_INPUT_REGISTER_START;
extern const uint16_t INPUT_REGISTER_COUNT;

extern const unsigned long WATCHDOG_TIMEOUT_MS;
extern const unsigned long INPUT_REGISTER_UPDATE_INTERVAL_MS;

extern unsigned long lastCoilUpdateTime;
extern unsigned long lastInputRegisterUpdateTime;
extern bool watchdogTriggered;
extern int relay_watchdog_states[];

extern Adafruit_ADS1115 ads1;
extern Adafruit_ADS1115 ads2;
extern Adafruit_ADS1115* ads_list[2];

extern EthernetServer ethServer;
extern ModbusTCPServer modbusTCPServer;

void configure_modbus();
void update_input_registers();
void apply_live_relay_states();
void apply_watchdog_relay_states();

#endif
