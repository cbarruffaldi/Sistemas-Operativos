#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PEER_PATH "/tmp/fifo_peer_%d"  // Se le concatena el peer PID. Se asegura unicidad.

static t_response read_response(char * fifo);

struct t_address {
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

t_requestADT read_request(t_addressADT addr) {
  t_requestADT req;
  int n, fd = open(addr->path, O_RDONLY);

  if (fd < 0)
    return NULL;

  req = malloc(sizeof(struct t_request));
  n = read(fd, req, sizeof(struct t_request));

  close(fd);

  if (n < 1) {
    free(req);
    req = NULL;
  }

  return req;
}

int listen_peer(t_addressADT addr) {
  if (mkfifo(addr->path, 0666) < 0)
    return -1;
  return 0;
}

// Borra el fifo asociado.
void unlisten_peer(t_addressADT addr) {
  unlink(addr->path);
}

t_requestADT create_request() {
  struct t_address res_addr;
  t_requestADT req = malloc(sizeof(struct t_request));
  sprintf(res_addr.path, FIFO_PEER_PATH, getpid());
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

t_response send_request(t_addressADT addr, t_requestADT req) {
  t_response res = {.msg = "\0"};
  int n, fd = open(addr->path, O_WRONLY);

  if (fd < 0)
    return res;

  n = write(fd, req, sizeof(struct t_request));
  close(fd);

  if (n > 0)
    res = read_response(req->res_addr.path);

  return res;
}

// Devuelve la respuesta a un request.
static t_response read_response(char * fifo) {
  t_response res = {.msg = "\0"};
  int fd = open(fifo, O_RDONLY);

  if (fd < 0)
    return res;

  read(fd, &res, sizeof(t_response));
  close(fd);
  return res;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_response res) {
  int n, fd = open(req->res_addr.path, O_WRONLY);

  if (fd < 0)
    return -1;

  n = write(fd, &res, sizeof(res));

  close(fd);
  free(req);
  return n < 1 ? -1 : 0;
}