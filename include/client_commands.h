#ifndef _CLIENT_COMMANDS_H_
#define _CLIENT_COMMANDS_H_

#include "client.h"

/** Ejecuta el comando correspondiente al nombre name con los args como parámetros
** de dicho comando. En caso de ser necesario, envía una request usando la sesión
** y la información de usuario dada */
int run_command(const char *name, const char *args, sessionADT se, t_user *user);

#define VALID 1
#define UNSUPPORTED 0
#define INVALID_ARGS -1
#define NOT_LOGGED -2
#define ALREADY_LOGGED -3


#endif
