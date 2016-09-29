#ifndef _SERVER_H_
#define _SERVER_H_

t_tweet * sv_tweet(void * p, char * user, char * msg, int last_id);
t_tweet * sv_refresh(void * p, int last_id);
void sv_like(void * p, int id);

#endif