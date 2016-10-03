#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 1024

/* Representa un paquete a enviar. */
typedef struct t_request * t_requestADT;

/* Representa la dirección de un peer. */
typedef struct t_address * t_addressADT;

/* Representa una conexión con un peer.
** Dependiendo de si fue creada con accept_peer() o connect_peer(),
** se puede leer o escribir respectivamente. */
typedef struct t_connection * t_connectionADT;

/* Representa una respuesta, ya sea a enviar o recibida. */
typedef struct t_response * t_responseADT;

/* Crea nuevo request. */
t_requestADT create_request();

/* Crea nuevo response. */
t_responseADT create_response();

/* Crea el adress correspondiente al string dado.
** El formate del path depende de la implementación. */
t_addressADT create_address(const char * path);

/* Abre una nueva conexión hacia el address dado para escribir.
** Devuelve NULL en caso de error. Se considera error que no
** se encuentre un peer esperando una conexión. */
t_connectionADT connect_peer(t_addressADT addr);

/* Abre un nuevo canal por el cual el peer leerá requests que sean enviados
** al address pasado como parámetro.
** Devuelve -1 en caso de error. */
int listen_peer(t_addressADT addr);

/* Devuelve una connection cuando un peer hace connect a un address
** que previamente pasó por la función listen_peer. */
t_connectionADT accept_peer(t_addressADT addr);

/* Settea el mensaje del response */
void set_response_msg(t_responseADT res, const char *bytes);

/* Settea el mensaje del request. */
void set_request_msg(t_requestADT req, const char *bytes);

/* Getter de la info en el response. Copia el mensaje en buffer. Buffer debe tener BUFISZE bytes de espacio.
** Libera los recursos del response. */
void get_response_msg(t_responseADT res, char buffer[]);

/* Getter de los bytes en el request. Copia el mensaje en buffer.  */
void get_request_msg(t_requestADT req, char buffer[]);

/* Envia un request y devuelve su respuesta.
** Bloqueante. */
t_responseADT send_request(t_connectionADT con, t_requestADT req);

/* Lee request de una conexión.
** Bloqueante. */
t_requestADT read_request(t_connectionADT con);

/* Responde al peer que envió el request dado como parámetro.
** Libera los recursos asociados al request.
** Bloqueante. */
int send_response(t_requestADT req, t_responseADT res);

/* Cierra una conexión adquirida por connect. */
void disconnect(t_connectionADT con);

/* Cierra una conexión adquirida mediante un accept. */
void unaccept(t_connectionADT con);

/* Cierra la conexión por la cual el peer leía requests. */
void unlisten_peer(t_addressADT con);

/* Libera los recursos asociados al response. */
void free_response(t_responseADT res);

/* Libera la memoria y recursos asociados al request. */
void free_request(t_requestADT req);

/* Libera el address y los recursos asociados a él. */
void free_address(t_addressADT addr);

#endif
