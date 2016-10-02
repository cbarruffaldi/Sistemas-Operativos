#ifndef _MARSHALLING_
#define _MARSHALLING_

#include "IPC.h"

#define MAX_TW_REFRESH 10
#define SHORTBUF 256

#define ABORT -127  /* Hubo problema de IPC */

#define INVALID_MSG "invalid"

#define SEPARATOR ":"

#define OPCODE_TWEET "0"
#define OPCODE_LIKE "1"
#define OPCODE_REFRESH "2"
#define OPCODE_LOGIN "3"
#define OPCODE_LOGOUT "4"
#define OPCODE_SHOW "5"
#define OPCODE_DELETE "6"

#define USER_SIZE 32
#define MSG_SIZE 140

typedef struct {
  char user[USER_SIZE];
  char msg[MSG_SIZE];
  unsigned int likes;
  unsigned int id;
} t_tweet;

/** Guarda en la memoria comenzando a partir de *str el arreglo de tweets tws
** en forma de string (no se encarga de reservar memoria).
** Para formar el string, usa los atributos de la estructura t_tweet,
** y los inserta como texto, separándolos con el string SEPARATOR. Entre los atributos de
** dos tweets distintos del arreglo, también se ubica un SEPARATOR. */
void tweets_to_str (char * str, t_tweet * tws, int size);

/** Lo inverso a tweets_to_str. Devuelve la cantidad de tweets leídos. */
int str_to_tweets (const char * str, t_tweet * tws);

#endif
