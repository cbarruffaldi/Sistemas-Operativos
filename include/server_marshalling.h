#ifndef _SERVER_MARSHALLING_
#define _SERVER_MARSHALLING_

#include "marshalling.h"

#define CMDS_SIZE 3

#define UNSUPPORTED -1
#define UNSUPPORTED_MSG "Invalid Command."

typedef struct t_session * t_sessionADT;
typedef struct t_master_session * t_master_sessionADT;

/**/
t_master_sessionADT setup_master_session(char *sv_path);

/**/
t_sessionADT accept_client(t_master_sessionADT master_session);

/**/
int attend(t_sessionADT se);

void set_session_data(t_sessionADT session, void * se_data);

#endif