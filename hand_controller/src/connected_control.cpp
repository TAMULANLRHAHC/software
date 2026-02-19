//
// Created by kemptonburton on 2/18/2026.
//

#include "main.h"

/* ---------- CONFIGURATIONS ---------- /
"watchdog_timeout_seconds"
"relay_#_watchdog_state"

/* ---------- COMMANDS ---------- /
"relay_#_desired_state"

/* ---------- TELEMETRY ---------- /
"relay_#_actual_state"
"AIN#_raw"

/* ---------- VALID CONNECTION ---------- /
 * When the telemetry connection is VALID, this runs.
 * Data comes from incomingData. Cleared on each iteration (NOT Persistent).
 * Outgoing data is written to outgoingData.
 *
 * COMMANDS
 * Gets commands from incomingData JSON.
 * Runs the commands. (commanding relays)
 *
 * SETTINGS / CONFIGURATION
 * Some commands are also persestent configuration variables
 * These are written to any persistent data store (watchdog states)
 *
 * TELEMETRY
 * Telemetry is gathered (analog channels)
 * Writes telemetry & state to outgoingData JSON.
 *
 */
void run_connected_control_iteration() {
    // =====================================================
    // ================= SETTINGS/ CONFIGURATION ===========
    // =====================================================

    // -------- WATCHDOG TIMEOUT COMMAND -------- //
    if (incoming_data.containsKey("watchdog_timeout_seconds")) {

        int seconds = incoming_data["watchdog_timeout_seconds"];
        watchdogTimeoutMs = (unsigned long)seconds * 1000UL;
    }


    // =====================================================
    // ================= COMMANDING ========================
    // =====================================================

    // -------- COMMAND RELAYS -------- //
    for (int i = RELAY_NUM_START; i <= RELAY_NUM_END; i++) {

        int index = i - RELAY_NUM_START;

        // Desired state
        String desired_key = "relay_" + String(i) + "_desired_state";
        if (incoming_data.containsKey(desired_key)) {

            int desired = incoming_data[desired_key];
            digitalWrite(RELAY_PINS[index], desired ? HIGH : LOW);

            String actual_key = "relay_" + String(i) + "_actual_state";
            outgoing_data[actual_key] = desired;
        }

        // Save Watchdog state to persistent data
        String watchdog_key = "relay_" + String(i) + "_watchdog_state";
        if (incoming_data.containsKey(watchdog_key)) {
            relay_watchdog_states[index] = incoming_data[watchdog_key];
        }
    }


    // -------- ANALOG CHANNELS -------- //
    for (int i = ANALOG_NUM_START; i <= ANALOG_NUM_END; i++) {

        int index = i - ANALOG_NUM_START;
        int raw = analogRead(ANALOG_PINS[index]);

        String outgoing_key = "AIN" + String(i) + "_raw";
        outgoing_data[outgoing_key] = raw;
    }
}