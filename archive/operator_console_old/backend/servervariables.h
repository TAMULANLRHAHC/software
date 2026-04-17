//
// Created by kemptonburton on 8/30/2025.
//

#ifndef SERVERVARIABLES_H
#define SERVERVARIABLES_H

#include <string>
#include <crow.h>
#include "websockets.h"

///SETTINGS///
#define DEFAULT_LOOP_TARGET_FREQUENCY 10.0
#define DEFAULT_CLIENT_UPDATE_TARGET_FREQUENCY 20.0

///SERVER VARIABLES///
inline std::string dataSourceName = "HAOS";

///IMPORTANT SERVER OBJECTS///
//App
extern crow::SimpleApp* app; //declare as pointer

//Socket
//extern CrowSocket* crow_socket; //declare as pointer

#endif //SERVERVARIABLES_H
