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

//TODO: manejo de errores
t_connectionADT accept_peer(t_addressADT addr) {
  char buffer[BUFSIZE];
  int fd, n = 0;
  t_connectionADT con;

  fd = open(addr->path, O_RDWR); // solo leerá pero así siempre hay un write
                                 // read no devolverá cero si se hace close del write.

  if (fd < 0)
    return NULL;

  n = read(fd, buffer, BUFSIZE);

  if (n == 0)
    printf("didnt read shit\n");

  close(fd);


  mkfifo(buffer, 0666);
  con = malloc(sizeof(struct t_connection));
  strcpy(con->path, buffer);

  return con;
}

t_connectionADT connect_peer(t_addressADT addr) {
  char buffer[BUFSIZE];
  t_connectionADT con = malloc(sizeof(struct t_connection));
  int fd = open(addr->path, O_WRONLY), pid = getpid(), n;

  if (fd < 0)
    return NULL;

  sprintf(buffer, FIFO_LISTEN_PATH, pid);
  n = write(fd, buffer, sizeof(buffer));
  close(fd);

  if (n < 0) {
    printf("didnt write shit\n");
    return NULL;
  }

  strcpy(con->path, buffer);

  return con;
}

void disconnect(t_connectionADT con) {
  unlink(con->path);
  free(con);
}

int listen_peer(t_addressADT addr) {
  if (mkfifo(addr->path, 0666) < 0)
    return -1;
  return 0;
}

// Cierra el file descriptor de la conexión y borra el fifo asociado.
void unlisten_peer(t_addressADT addr) {
  unlink(addr->path);
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
  write(fd, req, sizeof(struct t_request));
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
