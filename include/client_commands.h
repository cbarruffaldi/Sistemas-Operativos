#ifndef _CLIENT_COMMANDS_H_
#define _CLIENT_COMMANDS_H_

#include "client.h"

int execute(const char *name, const char *args, sessionADT se, t_user *user);

#define VALID 1
#define UNSUPPORTED 0
#define INVALID_ARGS -1
#define NOT_LOGGED -2
#define ALREADY_LOGGED -3

#endif