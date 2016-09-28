#ifndef _SERVER_MARSHALLING_
#define _SERVER_MARSHALLING_

#define SEPARATOR ":"

#define OPCODE_TWEET "1"
#define OPCODE_LIKE "2"
#define OPCODE_REFRESH "3"

#define USER_SIZE 32
#define MSG_SIZE 140

#define CMDS_SIZE 3

#define UNSUPPORTED -1

typedef struct addr * addr_ADT;
typedef struct client_req * clireq_ADT;

int start_connection(addr_ADT addresses);
addr_ADT init(char * server, char * database);



#endif