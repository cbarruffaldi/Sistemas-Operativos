#ifndef _MARSHALLING_
#define _MARSHALLING_

#define MAX_TW_REFRESH 10

#define SEPARATOR ":"
#define END_TOKEN "**end**"

#define OPCODE_TWEET "0"
#define OPCODE_LIKE "1"
#define OPCODE_REFRESH "2"

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
