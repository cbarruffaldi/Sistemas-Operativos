#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 4096

typedef struct {
  char msg[BUFSIZE];
} t_response;

// Representa un paquete a enviar.
typedef struct t_request * t_requestADT;

// Representa la dirección de un peer.
typedef struct t_address * t_addressADT;

// Representa una conexión con un peer.
// Dependiendo de si fue creada con accept() o connect(),
// se puede leer o escribir respectivamente.
typedef struct t_connection * t_connectionADT;

// Crea nuevo request.
t_requestADT create_request();

// Settea el mensaje del request.
void set_request_msg(t_requestADT req, char *msg);

// Envia un request y devuelve su respuesta.
t_response send_request(t_connectionADT con, t_requestADT req);

// send_request(t_connectionADT con, t_addressADT addr, char *msg);
// Esto podría reemplazar a create_request + set_request_msg + send_request

// TODO: ??? Que hace free_request segun la api? Porque ademas de usarla para memoria la usamos para cerrar el fifo en la implementacion.
// Libera la memoria y recursos asociados al request.
void free_request(t_requestADT req);

// Crea el adress correspondiente al string dado.
t_addressADT create_address(char * path);

void free_address(t_addressADT addr);

// Abre una nueva conexión hacia el address dado.
// El peer que invoca esta función para conectarse sólo podrá escribir a
// través de esta conexión.
t_connectionADT connect_peer(t_addressADT addr);

// Cierra la conexión.
void disconnect(t_connectionADT con);

// Abre un nuevo canal por el cual el peer leerá requests que sean enviados
// al address pasado como parámetro.
int listen_peer(t_addressADT addr);

// Devuelve una connection cuando un peer hace connect a un address
// que previamente pasó por la función listen_peer.
t_connectionADT accept_peer(t_addressADT addr);

// Cierra la conexión por la cual el peer leía requests.
void unlisten_peer(t_addressADT con);

// Lee request de una conexión.
// Se bloquea hasta que se envíe alguno.
t_requestADT read_request(t_connectionADT con);

// Getter de la info en el request. Copia el mensaje en buffer. Buffer debe tener 1024 bytes de espacio.
void get_request_msg(t_requestADT req, char *buffer);

// Responde al peer que envió el request dado como parámetro.
int send_response(t_requestADT req, t_response res);

#endif
