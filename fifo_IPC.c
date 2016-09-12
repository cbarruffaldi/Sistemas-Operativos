#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


static t_response read_response(char * fifo);


struct t_address {
  char path[BUFSIZE];
};

struct t_connection {
  int fd;
};

struct t_request {
  char msg[BUFSIZE];
  struct t_address res_addr;
};

t_addressADT create_address(char * path) {
  t_addressADT addr = malloc(sizeof(struct t_address));
  strcpy(addr->path, path);
  return addr;
}

t_connectionADT connect(t_addressADT addr) {
  t_connectionADT con = malloc(sizeof(struct t_connection));
  con->fd = open(addr->path, O_WRONLY);
  return con;
}

void disconnect(t_connectionADT con) { // void?
  close(con->fd);
  // free
}

t_connectionADT listen(t_addressADT addr) {
  mkfifo(addr->path, 0666);
  t_connectionADT con = malloc(sizeof(struct t_connection));
  con->fd = open(addr->path, O_RDWR);   // Solo lee pero es RDWR para que siempre esté
  return con;                           // abierto pendiente de un write
}

// int server_WR_fd() {
//   return open(SERVER_FIFO_PATH, O_WRONLY);
// }
//
// int server_RD_fd() {
//   return open(SERVER_FIFO_PATH, O_RDWR); // Abre RDWR para que siempre exista abierto para Write
// }

// Crea nuevo request
t_requestADT create_request(t_addressADT addr) {
  t_requestADT req = malloc(sizeof(struct t_request));
  req->res_addr = *addr;
  // sprintf(req->res_fifo, CLIENT_FIFO_PATH, getpid());  // Crea nombre unico para fifo del cliente

  mkfifo((req->res_addr).path, 0666);  // Crea fifo

  return req;
}

// Libera el request y borra el fifo asociado.
void free_request(t_requestADT req) {
  unlink((req->res_addr).path);
  free(req);
}

// Settea el mensaje del request.
void set_request_msg(t_requestADT req, char *msg) {
  strcpy(req->msg, msg);
}

// Envia un request y devuelve su respuesta.
t_response send_request(t_connectionADT con, t_requestADT req) {
  write(con->fd, req, sizeof(struct t_request));
  return read_response((req->res_addr).path);
}

// Lee request. Se bloquea hasta que se envíe alguno.
t_requestADT read_request(t_connectionADT con) {
  t_requestADT req = malloc(sizeof(struct t_request));
  read(con->fd, req, sizeof(struct t_request));
  return req;
}

// Devuelve la respuesta a un request.
static t_response read_response(char * fifo) {
  t_response res;
  int fd = open(fifo, O_RDONLY);
  read(fd, &res, sizeof(t_response));
  close(fd);
  return res;
}

// Getter de msg. Copia el mensaje en buffer.
void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

// Responde al cliente que envió req
void send_response(t_requestADT req, t_response res) {
  int fd = open((req->res_addr).path, O_WRONLY);
  write(fd, &res, sizeof(res));
  close(fd);
  free(req);
}
