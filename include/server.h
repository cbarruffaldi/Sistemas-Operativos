#ifndef _SERVER_H_
#define _SERVER_H_

/* Servidor procesa tweet a enviar.
** Coloca el nombre del usuario conectado. */
int sv_tweet(void * p, const char * msg);

/* Coloca en el arreglo de tweets los tweets que van del id
** from_id hasta from_id+MAX_TW_REFRESH-1
** Devuelve la cantidad de elementos en el arreglo */
int sv_refresh(void * p, int from_id, t_tweet tws[]);

/* Envia la accion de 'like' junto con el id del tweet a la base de datos */
int sv_like(void * p, int id);

/* Login de un usuario */
int sv_login(void * p, const char * username);

/* Logout de un usuario */
int sv_logout(void * p);

/* Devuelve un tweet especifico a mostrar según id.
** Si no existe tweet con dicho id devuelve un tweet
** con msg[0]='\0' */
t_tweet sv_show(void * p, int id);

/* ELimina un tweet */
int sv_delete(void * p, int id);

#endif
