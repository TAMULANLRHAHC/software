#include "main.h"

void apply_watchdog_relay_states() {
    for (int i = 0; i < RELAY_COUNT; ++i) {
        digitalWrite(RELAY_PINS[i], relay_watchdog_states[i] ? HIGH : LOW);
    }

    watchdogTriggered = true;
}
