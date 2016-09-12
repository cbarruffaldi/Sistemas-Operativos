#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_PATH "fifo_server"
#define CLIENT_FIFO_PATH "fifo_client%d"  // Se le concatena el client PID. Se asegura unicidad.

static t_response read_response(char * fifo);

struct t_request {
  char msg[BUFSIZE];
  char res_fifo[BUFSIZE];
};

int server_WR_fd() {
  return open(SERVER_FIFO_PATH, O_WRONLY);
}

int server_RD_fd() {
  return open(SERVER_FIFO_PATH, O_RDWR); // Abre RDWR para que siempre exista abierto para Write
}

// Crea nuevo request
t_requestADT create_request() {
  t_requestADT req = malloc(sizeof(struct t_request));

  sprintf(req->res_fifo, CLIENT_FIFO_PATH, getpid());  // Crea nombre unico para fifo del cliente
  mkfifo(req->res_fifo, 0666);  // Crea fifo

  return req;
}

// Libera el request y borra el fifo asociado.
void free_request(t_requestADT req) {
  unlink(req->res_fifo);
  free(req);
}

// Settea el mensaje del request.
void set_request_msg(t_requestADT req, char *msg) {
  strcpy(req->msg, msg);
}

// Envia un request y devuelve su respuesta.
t_response send_request(int fd, t_requestADT req) {
  write(fd, req, sizeof(struct t_request));
  return read_response(req->res_fifo);
}

// Lee request. Se bloquea hasta que se envíe alguno.
t_requestADT read_request(int fd) {
  t_requestADT req = malloc(sizeof(struct t_request));
  read(fd, req, sizeof(struct t_request));
  return req;
}

// Devuelve la respuesta a un request. 
t_response read_response(char * fifo) {
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
  // TODO: Si es muy costoso abrir y cerrar asi lo hacemos de otra forma
  int fd = open(req->res_fifo, O_WRONLY);
  write(fd, &res, sizeof(res));
  close(fd);
  free(req);
}