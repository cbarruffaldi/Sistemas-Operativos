#ifndef _MARSHALLING_
#define _MARSHALLING_

#define SEPARATOR ":"

#define OPCODE_TWEET "1"
#define OPCODE_LIKE "2"
#define OPCODE_REFRESH "3"

#define TW_COLUMNS 4

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
