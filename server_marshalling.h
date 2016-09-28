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
#define UNSUPPORTED_MSG "Invalid Command."

typedef struct t_session * t_sessionADT;
typedef struct t_master_session * t_master_sessionADT;

t_master_sessionADT setup_master_session(char *sv_path);
t_sessionADT accept_client(t_master_sessionADT master_session);


#endif