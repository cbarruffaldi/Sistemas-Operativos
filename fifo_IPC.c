#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_RESPONSE_PATH "/tmp/fifo_response_%d"  // Se le concatena el peer PID. Se asegura unicidad.
#define FIFO_LISTEN_PATH "/tmp/fifo_listen_%d"

static t_response read_response(char * fifo);

struct t_address {
  char path[BUFSIZE];
};

struct t_connection {
  char path[BUFSIZE];
}

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

//TODO: manejo de errores
t_connectionADT accept_peer(t_addressADT addr) {
 char buffer[BUFSIZE];
 int pid, fd = open(addr->path, O_RDONLY);

 t_connectionADT con = malloc(sizeof(struct t_address));
 read(fd, buffer, BUFSIZE);
 close(fd);

 mkfifo(buffer, 0666);
 return con;
}

void free_address(t_addressADT addr) {
  free(addr);
}

//TODO: manejo de errores y decidir si hacer open y close todo el tiempo o solo una vez
t_connectionADT accept_peer(t_addressADT addr) {
 char buffer[BUFSIZE];
 int pid, fd;
 t_connectionADT con;

 fd = open(addr->path, O_RDONLY);
 read(fd, buffer, BUFSIZE);
 close(fd);
 mkfifo(buffer, 0666);

 con = malloc(sizeof(struct t_connection));
 con->fd = open(buffer, O_RDONLY);
 strcpy(con->addr.path, buffer);

 return con;
}

t_connectionADT connect_peer(t_addressADT addr) {
  char buffer[BUFSIZE];
  t_connectionADT con = malloc(sizeof(struct t_connection));
  int fd = open(addr->path, O_WRONLY), pid = getpid();

  if (fd < 0)
    return NULL;

  sprintf(buffer, FIFO_LISTEN_PATH, pid);
  write(fd, buffer, sizeof(buffer));
  close(fd);

  return con;
}

void disconnect(t_addressADT con) { // void?
  close(con->fd);
  free(con);
}

int listen_peer(t_addressADT addr) {
  if (mkfifo(addr->path, 0666) < 0)
    return -1;
  return 0;
}

// Cierra el file descriptor de la conexión y borra el fifo asociado.
void unlisten(t_addressADT con) {
  unlink(con->addr.path);
  disconnect(con);
}

t_requestADT create_request() {
  struct t_address res_addr;
  t_requestADT req = malloc(sizeof(struct t_request));
  sprintf(res_addr.path, FIFO_RESPONSE_PATH, getpid());
  req->res_addr = res_addr;

  if(mkfifo(res_addr.path, 0666) < 0)  // Crea fifo
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
  int fd = open(con->path, O_WRONLY);
  write(con->fd, req, sizeof(struct t_request));
  close(fd);
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

//TODO: manejo de errores
t_requestADT read_request(t_connectionADT con) {
  t_requestADT req = malloc(sizeof(struct t_request));
  int fd = open(con->path, O_RDONLY);
  read(fd, req, sizeof(struct t_request));
  close(fd);
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
