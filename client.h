#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "client_marshalling.h"

typedef struct t_user {
  char username[USER_SIZE];
  int twit_id;
} t_user;

#endif