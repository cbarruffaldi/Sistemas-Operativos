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

/* Envía usuario y mensaje, con el encoding y el opcode apropiado,
** a través de la sesión dada como parámetro.
** Devuelve el ID del último tweet registrado en la Base de Datos. */
int send_tweet(sessionADT se, char * user, char * msg);

/* Envía el id del tweet al cuál se quiere likear.
** Devuelve el número de likes que le quedaron al tweet seleccionado. */
int send_like(sessionADT se, int tweet_id);

/* En * tws copia un arreglo con todos los tweets registrados en la Base de Datos.
** Devuelve la longuitud del arreglo copiado (la cantidad de tweets). */
t_tweet * send_refresh(sessionADT se, int *size);

#endif
