#ifndef _CLIENT_MARSHALLING_
#define _CLIENT_MARSHALLING_

#define REFRESHING 1
#define ENDED 0

#include "marshalling.h"

/* Representa una sesión de conexión cliente -> servidor */
typedef struct session * sessionADT;

/* Crea e inicia la sesión */
sessionADT start_session(char * a);

/* Finaliza la sesión */
void end_session(sessionADT se);

/* Envía mensaje con el encoding y el opcode apropiado,
** a través de la sesión dada como parámetro.
** Devuelve el ID del último tweet registrado en la Base de Datos. */
int send_tweet(sessionADT se, const char * msg);

/* Envía el id del tweet al cuál se quiere likear.
** Devuelve el número de likes que le quedaron al tweet seleccionado. */
int send_like(sessionADT se, int tweet_id);

/* Devuelve un arreglo con todos los tweets registrados en la Base de Datos.
** En size coloca la longuitud del arreglo copiado (la cantidad de tweets). */
t_tweet * send_refresh(sessionADT se, int *size);

/* Envía usuario con el que se conectará el cliente.
** Todo tweet se guardará bajo este nombre de usuario */
int send_login (sessionADT se, const char *username);

/* Desloguea al usuario. */
int send_logout (sessionADT se);

#endif
