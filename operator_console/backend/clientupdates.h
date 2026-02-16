//
// Created by kemptonburton on 8/29/2025.
//

#ifndef CLIENTUPDATES_H
#define CLIENTUPDATES_H

#include <haos.h>
#include <websockets.h>

void setup_socket_connections_for_client_updates(CrowSocket &socket);

//update loop functions
void send_client_updates(CrowSocket &socket);

#endif //CLIENTUPDATES_H
