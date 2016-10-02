#ifndef _DB_MARSHALLING_H_
#define _DB_MARSHALLING_H_

#include "marshalling.h"

typedef struct t_DBsession * t_DBsessionADT;

t_DBsessionADT start_DBsession(const char *db_path);

void end_DBsession(t_DBsessionADT se);

/* Guarda tweet en la base de datos. Devuelve su ID. */
int send_tweet (t_DBsessionADT se, const char *username, const char *msg);

/* Likea un tweet según id.
** Devuelve la cantidad de likes del tweet y -1 si no existe el tweet */
int send_like(t_DBsessionADT se, int id);

/* Guarda en un arreglo de tweets una cantidad MAX_TW_REFRESH de tweets o menos.
** Devuelve la cantidad de elementos dentro del arreglo. */
int send_refresh(t_DBsessionADT se, int from_id, t_tweet tws[]);

/* Devuelve un tweet según id
** Si el tweet no existe devuelve un tweet con msg[0] = '\0'. */
t_tweet send_show(t_DBsessionADT se, int tweet_id);

/* Devuelve 1 si hay un tweet con el id y el username dados, y lo borra.
** Devuelve -1 si no se encuentra ese tweet. */
int send_delete(t_DBsessionADT se, char * username, int tweet_id);

#endif
