#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PEER_PATH "fifo_peer_%d"  // Se le concatena el peer PID. Se asegura unicidad.

static t_response read_response(char * fifo);

struct t_address {
  char path[BUFSIZE];
};

struct t_connection {
  struct t_address addr;
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

void free_address(t_addressADT addr) {
  free(addr);
}

t_connectionADT connect_peer(t_addressADT a) {
  t_connectionADT con = malloc(sizeof(struct t_connection));
  con->fd = open(a->path, O_WRONLY);

  if (fd < 0)
    return NULL;

  con->addr = *a;
  return con;
}

void disconnect(t_connectionADT con) { // void?
  close(con->fd);
  free(con);
}

t_connectionADT listen_peer(t_addressADT addr) {
  if (mkfifo(addr->path, 0666) != 0)
    return NULL;

  t_connectionADT con = malloc(sizeof(struct t_connection));
  strcpy(con->addr.path, addr->path);
  con->fd = open(addr->path, O_RDWR);   // Solo lee pero es RDWR para que siempre esté
  return con;                           // abierto pendiente de un write
}

// Cierra el file descriptor de la conexión y borra el fifo asociado.
void unlisten(t_connectionADT con) {
  unlink(con->addr.path);
  disconnect(con);
}

t_requestADT create_request() {
  t_requestADT req = malloc(sizeof(struct t_request));
  struct t_address res_addr;
  sprintf(res_addr.path, FIFO_PEER_PATH, getpid());
  req->res_addr = res_addr;

  if(mkfifo(res_addr.path, 0666) != 0);  // Crea fifo
    return NULL;

  return req;
}

// Libera el request y borra el fifo asociado.
void free_request(t_requestADT req) {
  unlink(req->res_addr.path);
  free(req);
}

void set_request_msg(t_requestADT req, char *msg) {
  strcpy(req->msg, msg);
}

t_response send_request(t_connectionADT con, t_requestADT req) {
  write(con->fd, req, sizeof(struct t_request));
  return read_response(req->res_addr.path);
}

// Devuelve la respuesta a un request.
static t_response read_response(char * fifo) {
  t_response res;
  int fd = open(fifo, O_RDONLY);
  read(fd, &res, sizeof(t_response));
  close(fd);
  return res;
}

t_requestADT read_request(t_connectionADT con) {
  t_requestADT req = malloc(sizeof(struct t_request));
  read(con->fd, req, sizeof(struct t_request));
  return req;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_response res) {
  int fd = open(req->res_addr.path, O_WRONLY);
  if (fd < 0)
    return -1;
  write(fd, &res, sizeof(res));
  close(fd);
  free(req);
  return 0;
}
