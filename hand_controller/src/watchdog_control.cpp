//
// Created by kemptonburton on 2/18/2026.
//

#include "main.h"

/* ---------- INVALID CONNECTION ---------- /
 * When the telemetry connection INVALID, this runs.
 * Data used here are global persistent data stores.
 * Default values exist in header files.
 * Configured values exist through the run_connected_control_iteration
 *
 * ERROR COMMANDS
 * Gets commands or settings from any persistent data stores
 * Runs the commands. (commanding watchdog relay states)
 *
 */
void run_watchdog_control_iteration() {

    // -------- COMMAND RELAYS -------- //
    for (int i = 0; i < RELAY_COUNT; i++) {

        int state = relay_watchdog_states[i];

        digitalWrite(RELAY_PINS[i], state ? HIGH : LOW);

        String outgoing_key =
            "relay_" + String(i + RELAY_NUM_START) + "_actual_state";

        outgoing_data[outgoing_key] = state;
    }
}