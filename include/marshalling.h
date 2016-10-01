#ifndef _MARSHALLING_
#define _MARSHALLING_

#include "IPC.h"

#define MAX_TW_REFRESH 10
#define SHORTBUF 256

#define INVALID_MSG "invalid"

#define SEPARATOR ":"

#define OPCODE_TWEET "0"
#define OPCODE_LIKE "1"
#define OPCODE_REFRESH "2"
#define OPCODE_LOGIN "3"
#define OPCODE_LOGOUT "4"
#define OPCODE_SHOW "5"

#define USER_SIZE 32
#define MSG_SIZE 140

#define COLUMNS 60

typedef struct {
  char user[USER_SIZE];
  char msg[MSG_SIZE];
  unsigned int likes;
  unsigned int id;
} t_tweet;

#endif
