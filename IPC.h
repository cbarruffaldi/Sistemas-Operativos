#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 1024

typedef struct {
  char msg[BUFSIZE];
} t_response;

// Representa un paquete a enviar.
// Debe crearse con el t_address hacía el cual se responderá el request.
typedef struct t_request * t_requestADT;

// Representa la dirección de un peer.
typedef struct t_address * t_addressADT;

// Representa una conexión con un peer.
// Dependiendo de si fue creada con listen() o connect(),
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

// Abre una nueva conexión hacia el address dado.
// El peer que invoca esta función para conectarse sólo podrá escribir a
// través de esta conexión.
t_connectionADT connect(t_addressADT addr);

// Cierra la conexión.
void disconnect(t_connectionADT con);

// Abre una nueva conexión por la cual el peer leerá requests que sean enviados
// al address pasado como parámetro.
t_connectionADT listen(t_addressADT addr);

// Cierra la conexión por la cual el peer leía requests.
void unlisten(t_connectionADT con);

// Lee request de una conexión.
// Se bloquea hasta que se envíe alguno.
t_requestADT read_request(t_connectionADT con);

// Getter de la info en el request. Copia el mensaje en buffer. Buffer debe tener 1024 bytes de espacio.
void get_request_msg(t_requestADT req, char *buffer);

// Responde al peer que envió el request dado como parámetro.
void send_response(t_requestADT req, t_response res);

#endif
