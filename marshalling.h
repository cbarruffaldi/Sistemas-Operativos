##ifndef _MARSHALLING_
#define _MARSHALLING_

#define SEPARATOR "::"

#define OPCODE_TWEET 1
#define OPCODE_LIKE 2
#define OPCODE_REFRESH 3

#define USER_SIZE 20
#define TWEET_LENGHT 140 

typedef struct session * sessionADT;

struct tweet {
	int id;
	char[TWEET_LENGHT] msg;
	char[USER_SIZE] usr;
	int likes;
};

// solo de client?
sessionADT start_session(char * a);
void end_session(sessionADT se);
int tweet_send(sessionADT se, char * user, char * msg);
int like_send(sessionADT se, int tweet_id);


#endif
