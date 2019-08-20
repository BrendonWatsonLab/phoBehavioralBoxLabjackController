#pragma once

#include "External/civetweb/include/CivetServer.h"

#define DOCUMENT_ROOT "."
#define PORT "8081"
#define EXAMPLE_URI "/example"
#define EXIT_URI "/exit"


/* Exit flag for main loop */
//volatile bool exitNow = false;




void runServer();
