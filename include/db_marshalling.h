#ifndef _DB_MARSHALLING_H_
#define _DB_MARSHALLING_H_

#include "marshalling.h"

#define VALID 1
#define NOT_VALID -1

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

/* Guarda en un puntero a t_tweet un tweet dado su id.
** Si el id no existe -1. */
int send_show(t_DBsessionADT se, int tweet_id, t_tweet *tw);

/* Devuelve 1 si hay un tweet con el id y el username dados, y lo borra.
** Devuelve -1 si no se encuentra ese tweet. */
int send_delete(t_DBsessionADT se, char * username, int tweet_id);

#endif
