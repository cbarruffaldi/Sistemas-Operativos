#ifndef _SERVER_H_
#define _SERVER_H_

int sv_tweet(void * p, const char * msg);

/* Coloca en el arreglo de tweets los tweets que van del id
** from_id hasta from_id+MAX_TW_REFRESH-1
** Devuelve la cantidad de elementos en el arreglo */
int sv_refresh(void * p, int from_id, t_tweet tws[]);

int sv_like(void * p, int id);

int sv_login(void * p, const char * username);

int sv_logout(void * p);

t_tweet sv_show(void * p, int id);

#endif
