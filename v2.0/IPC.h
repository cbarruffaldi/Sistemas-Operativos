#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 1024

typedef struct {
  char msg[BUFSIZE];
} t_response;

// Representa un paquete a enviar.
typedef struct t_request * t_requestADT;

// Representa la dirección de un peer.
typedef struct t_address * t_addressADT;

// Crea nuevo request.
t_requestADT create_request();

// Settea el mensaje del request.
void set_request_msg(t_requestADT req, char *msg);

// Envia un request y devuelve su respuesta.
// En caso de error t_response.msg es un string vacío.
t_response send_request(t_addressADT addr, t_requestADT req);

// send_request(t_connectionADT con, t_addressADT addr, char *msg);
// Esto podría reemplazar a create_request + set_request_msg + send_request

// TODO: ??? Que hace free_request segun la api? Porque ademas de usarla para memoria la usamos para cerrar el fifo en la implementacion.
// Libera la memoria y recursos asociados al request.
void free_request(t_requestADT req);

// Crea el address correspondiente al path dado.
t_addressADT create_address(char * path);

// Libera la memoria y recursos asociados al address.
void free_address(t_addressADT addr);

// Permite que puedan leerse request a partir de la address pasada como parámetro.
int listen_peer(t_addressADT addr);

// Cierra la conexión por la cual el peer leía requests.
void unlisten_peer(t_addressADT addr);

// Lee request de un address previamente abierto para leer a partir de listen_peer.
// Se bloquea hasta que se envíe alguno.
t_requestADT read_request(t_addressADT addr);

// Getter de la info en el request. Copia el mensaje en buffer. Buffer debe tener 1024 bytes de espacio.
void get_request_msg(t_requestADT req, char *buffer);

// Responde al peer que envió el request dado como parámetro.
int send_response(t_requestADT req, t_response res);

#endif
