#ifndef _MARSHALLING_
#define _MARSHALLING_

#define SEPARATOR ":|:"

#define OPCODE_TWEET 1
#define OPCODE_LIKE 2
#define OPCODE_REFRESH 3

#define USER_SIZE 32
#define MSG_SIZE 140

typedef struct session * sessionADT;

sessionADT start_session(char * a);

void end_session(sessionADT se);

int send_tweet(sessionADT se, char * user, char * msg);

int send_like(sessionADT se, int tweet_id);

#endif
