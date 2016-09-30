#ifndef _SERVER_H_
#define _SERVER_H_

int sv_tweet(void * p, const char * msg);
t_tweet * sv_refresh(void * p, int last_id, char res[]);
int sv_like(void * p, int id);
int sv_login(void * p, const char * username);
int sv_logout(void * p);

#endif
