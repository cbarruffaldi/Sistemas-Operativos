#ifndef _CLIENT_MARSHALLING_
#define _CLIENT_MARSHALLING_

#define ABORT -127  /* Hubo problema al enviar algo al servidor */

#include "marshalling.h"

/* Representa una sesión de conexión cliente -> servidor */
typedef struct session * sessionADT;

/* Crea e inicia la sesión */
sessionADT start_session(char * a);

/* Finaliza la sesión */
void end_session(sessionADT se);

/* Envía mensaje con el encoding y el opcode apropiado,
** a través de la sesión dada como parámetro.
** Devuelve el ID del nuevo tweet. */
int send_tweet(sessionADT se, const char * msg);

/* Envía el id del tweet al cuál se quiere likear.
** Devuelve el número de likes que le quedaron al tweet seleccionado o -1 si no existe el tweet */
int send_like(sessionADT se, int tweet_id);

/* Devuelve un arreglo con todos los tweets registrados en la Base de Datos.
** En size coloca la longuitud del arreglo copiado (la cantidad de tweets). */
t_tweet * send_refresh(sessionADT se, int *size);

/* Envía usuario con el que se conectará el cliente.
** Todo tweet se guardará bajo este nombre de usuario */
int send_login (sessionADT se, const char *username);

/* Desloguea al usuario. */
int send_logout (sessionADT se);

/* Devuelve un tweet dado un id.
** Si el id no existe devuelve tweet con msg[0] = '\0'. */
t_tweet send_show(sessionADT se, int tweet_id);

/* Borra de la base de datos el tweet con el id dado.
** Solo puede borrar un tweet el usuario que lo escribió (del mismo username).
** Devuelve 1 si se pudo borrar el tweet y -1 si no. */
int send_delete(sessionADT se, int tweet_id);

#endif
