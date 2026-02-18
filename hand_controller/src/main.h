#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>

// =====================================================
// ================= NETWORK SETTINGS ==================
// =====================================================

extern byte mac[];
extern IPAddress ip;
extern const uint16_t SERVER_PORT;
extern const int ETHERNET_SPI_CS_PIN;


// =====================================================
// ================= CONTROL/ DAQ SETTINGS =============
// =====================================================

// ---------- RELAY ---------- //
extern const int RELAY_NUM_START;
extern const int RELAY_NUM_END;
extern const int RELAY_COUNT;

extern const int RELAY_PINS[];


// ---------- ANALOG ---------- //
extern const int ANALOG_NUM_START;
extern const int ANALOG_NUM_END;
extern const int ANALOG_COUNT;

extern const int ANALOG_PINS[];


// =====================================================
// ================= DATA SETTINGS =====================
// =====================================================

// ---------- GLOBAL ONE-SHOT DATA ---------- //
extern JsonDocument incoming_data;
extern JsonDocument outgoing_data;

// ---------- GLOBAL PERSISTENT DATA ---------- //
// WATCHDOG
extern unsigned long lastPacketTime;
extern unsigned long watchdogTimeoutMs;
extern bool watchdogTriggered;

extern int relay_watchdog_states[];


// =====================================================
// ================= OTHER =============================
// =====================================================

// other here
void run_connected_control_iteration();
// when connection is NOT valid
void run_watchdog_control_iteration();

#endif