#ifndef _CLIENT_MARSHALLING_
#define _CLIENT_MARSHALLING_

#define REFRESHING 1
#define ENDED 0

#include "marshalling.h"

typedef struct session * sessionADT;

sessionADT start_session(char * a);

void end_session(sessionADT se);

int send_tweet(sessionADT se, char * user, char * msg);

int send_like(sessionADT se, int tweet_id);

int send_refresh(sessionADT se, t_tweet tws[]);

#endif
