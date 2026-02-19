//
// Created by kemptonburton on 2/18/2026.
//

#include "main.h"

// =====================================================
// ================= NETWORK SETTINGS ==================
// =====================================================

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 1, 116);
const uint16_t SERVER_PORT = 6000;
const int ETHERNET_SPI_CS_PIN = 10;


// =====================================================
// ================= CONTROL/ DAQ SETTINGS =============
// =====================================================

// ---------- RELAY ---------- //
const int RELAY_NUM_START = 1;
const int RELAY_NUM_END   = 8;
const int RELAY_COUNT     = RELAY_NUM_END - RELAY_NUM_START + 1;

const int RELAY_PINS[RELAY_COUNT] = {
    49, 47, 45, 43, 41, 39, 37, 35
};
int relay_watchdog_states[RELAY_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0 //all closed
};


// ---------- ANALOG ---------- //
const int ANALOG_NUM_START = 0;
const int ANALOG_NUM_END   = 4;
const int ANALOG_COUNT     = ANALOG_NUM_END - ANALOG_NUM_START + 1;

const int ANALOG_PINS[ANALOG_COUNT] = {
    A0, A1, A2, A3, A4
};


// =====================================================
// ================= DATA SETTINGS =====================
// =====================================================

// ---------- GLOBAL ONE-SHOT DATA ---------- //
JsonDocument incoming_data;
JsonDocument outgoing_data;

// ---------- GLOBAL PERSISTENT DATA ---------- //
// WATCHDOG
unsigned long lastPacketTime = 0;
unsigned long watchdogTimeoutMs = 1000;
bool watchdogTriggered = false;




// =====================================================
// ================= OTHER =============================
// =====================================================