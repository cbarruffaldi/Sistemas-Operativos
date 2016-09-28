#ifndef _SERVER_MARSHALLING_
#define _SERVER_MARSHALLING_

#include "marshalling.h"

#define CMDS_SIZE 3

#define UNSUPPORTED -1

typedef struct addr * addr_ADT;
typedef struct client_req * clireq_ADT;

int start_connection(addr_ADT addresses);
addr_ADT init(char * server, char * database);

#endif
