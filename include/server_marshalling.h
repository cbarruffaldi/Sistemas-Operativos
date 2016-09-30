#ifndef _SERVER_MARSHALLING_
#define _SERVER_MARSHALLING_

#include "marshalling.h"

#define CMDS_SIZE 3

#define SUPPORTED 1
#define UNSUPPORTED -1
#define UNSUPPORTED_MSG ""

/* Representa una conexión cliente -> servidor */
typedef struct t_session * t_sessionADT;

/* Representa la conexión por la cual el servidor atiende las nuevas
** conexiones de clientes. */
typedef struct t_master_session * t_master_sessionADT;

/* Configura la master session a partir de un char * que constituye la
** dirección del servidor. */
t_master_sessionADT setup_master_session(char *sv_path);

/* Espera nuevas conecciones a master session, devuelve una sesión con
** la nueva conexión. */
t_sessionADT accept_client(t_master_sessionADT master_session);

/* Lee constantemente nuevas requests a través de la sesión dada, y
** las procesa según su OPCODE */
int attend(t_sessionADT se);

// TODO: Interpreta los datos del thread ???
void set_session_data(t_sessionADT session, void * se_data);

#endif
