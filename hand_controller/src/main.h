//
// Created by kemptonburton on 11/24/2025.
//

#ifndef MAIN_H
#define MAIN_H

#include <ArduinoJson.h>

JsonDocument incoming_data;
JsonDocument outgoing_data;

void run_control_iteration();

#endif //MAIN_H
