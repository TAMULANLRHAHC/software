#include "main.h"

// =====================================================
// ================= NETWORK SETTINGS ==================
// =====================================================

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
IPAddress ip(192, 168, 1, 116);
const int ETHERNET_SPI_CS_PIN = 10;

// =====================================================
// ================= RELAY SETTINGS ====================
// =====================================================

const int RELAY_NUM_START = 1;
const int RELAY_NUM_END = 8;
const int RELAY_COUNT = RELAY_NUM_END - RELAY_NUM_START + 1;

const int RELAY_PINS[RELAY_COUNT] = {
    49, 47, 45, 43, 41, 39, 37, 35
};

// =====================================================
// ================= ANALOG SETTINGS ===================
// =====================================================

const int ADS_ANALOG_NUM_START = 0;
const int ADS_ANALOG_NUM_END = 7;
const int ADS_ANALOG_COUNT = ADS_ANALOG_NUM_END - ADS_ANALOG_NUM_START + 1;

const int INTERNAL_ANALOG_NUM_START = 8;
const int INTERNAL_ANALOG_NUM_END = 10;
const int INTERNAL_ANALOG_COUNT =
    INTERNAL_ANALOG_NUM_END - INTERNAL_ANALOG_NUM_START + 1;
const int INTERNAL_ANALOG_PINS[INTERNAL_ANALOG_COUNT] = {
    A0, A1, A2
};

// =====================================================
// ================= OTHER SETTINGS ====================
// =====================================================

const uint16_t RELAY_COIL_START = 0;
const uint16_t WATCHDOG_COIL_START = RELAY_COIL_START + RELAY_COUNT;
const uint16_t COIL_COUNT = RELAY_COUNT * 2;

const uint16_t VOLTAGE_INPUT_REGISTER_START = 0;
const uint16_t RELAY_STATE_INPUT_REGISTER_START =
    VOLTAGE_INPUT_REGISTER_START + ADS_ANALOG_COUNT + INTERNAL_ANALOG_COUNT;
const uint16_t INPUT_REGISTER_COUNT = RELAY_STATE_INPUT_REGISTER_START + RELAY_COUNT;

const unsigned long WATCHDOG_TIMEOUT_MS = 10000UL;
const unsigned long INPUT_REGISTER_UPDATE_INTERVAL_MS = 50UL;

unsigned long lastCoilUpdateTime = 0;
unsigned long lastInputRegisterUpdateTime = 0;
bool watchdogTriggered = false;
int relay_watchdog_states[RELAY_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};

Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
Adafruit_ADS1115* ads_list[2] = {&ads1, &ads2};

EthernetServer ethServer(502);
ModbusTCPServer modbusTCPServer;
