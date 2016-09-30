#include "include/IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

// Se les concatena el peer PID y pthread.
// Se asegura unicidad entre procesos y threads.
#define FIFO_RESPONSE_PATH "/tmp/fifo_response_%d%lu"
#define FIFO_LISTEN_PATH "/tmp/fifo_listen_%d%lu"

#define FLAGS 0666

static t_responseADT read_response(char * fifo);
static int write_to_fifo(char * fifo, void * content, int size);
static void * read_from_fifo(char * fifo, int size);
static void send_disconnect_signal(t_connectionADT con);

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

struct t_response{
  char msg[BUFSIZE];
};

t_addressADT create_address(const char * path) {
  t_addressADT addr = malloc(sizeof(struct t_address));
  strcpy(addr->path, path);
  return addr;
}

void free_address(t_addressADT addr) {
  if (addr != NULL)
    free(addr);
}

// Recibe en buffer nombre de fifo a crear
t_connectionADT accept_peer(t_addressADT addr) {
  char buffer[BUFSIZE];
  int fd, n = 0;
  t_connectionADT con;

  fd = open(addr->path, O_RDWR); // solo leerá pero así siempre hay un write
                                 // read no devolverá cero si se hace close del write.

  if (fd < 0)
    return NULL;

  n = read(fd, buffer, BUFSIZE);
  close(fd);

  if (n < 1)
    return NULL;

  mkfifo(buffer, FLAGS);
  con = malloc(sizeof(struct t_connection));
  strcpy(con->path, buffer);

  return con;
}

t_connectionADT connect_peer(t_addressADT addr) {
  char buffer[BUFSIZE];
  t_connectionADT con;
  int n, fd = open(addr->path, O_WRONLY);

  if (fd < 0)
    return NULL;

  sprintf(buffer, FIFO_LISTEN_PATH, getpid(), pthread_self());
  n = write(fd, buffer, sizeof(buffer));
  close(fd);

  if (n < 0)
    return NULL;

  con = malloc(sizeof(struct t_connection));
  strcpy(con->path, buffer);

  return con;
}

void unaccept(t_connectionADT con) {
  IF (con != NULL)
    free(con);
}

void disconnect(t_connectionADT con) {
  if (con != NULL) {
    send_disconnect_signal(con);
    unlink(con->path);
    free(con);
  }
}

static void send_disconnect_signal(t_connectionADT con) {
  int fd = open(con->path, O_WRONLY);
  close(fd);
}

int listen_peer(t_addressADT addr) {
  if (mkfifo(addr->path, FLAGS) < 0)
    return -1;
  return 0;
}

// Cierra el file descriptor de la conexión y borra el fifo asociado.
void unlisten_peer(t_addressADT addr) {
  if (addr != NULL)
    unlink(addr->path);
}

void free_response(t_responseADT res) {
  if (res != NULL)
    free(res);
}

t_responseADT create_response() {
  return malloc(sizeof(struct t_response));
}

t_requestADT create_request() {
  struct t_address res_addr;
  t_requestADT req = malloc(sizeof(struct t_request));
  sprintf(res_addr.path, FIFO_RESPONSE_PATH, getpid(), pthread_self());
  req->res_addr = res_addr;

  if(mkfifo(res_addr.path, FLAGS) < 0)  // Crea fifo
    return NULL;

  return req;
}

void free_request(t_requestADT req) {
  if (req != NULL) {
    unlink(req->res_addr.path);
    free(req);
  }
}

void set_request_msg(t_requestADT req, const char *msg) {
  strcpy(req->msg, msg);
}

void set_response_msg(t_responseADT res, const char *msg) {
  strcpy(res->msg, msg);
}

void get_response_msg(t_responseADT res, char *buffer) {
  strcpy(buffer, res->msg);
  free(res);
}

t_responseADT send_request(t_connectionADT con, t_requestADT req) {
  int n, fd = open(con->path, O_WRONLY);
  t_responseADT res = NULL;

  if (fd < 0)
    return NULL;

  n = write(fd, req, sizeof(struct t_request));
  close(fd);

  if (n > 0)
    res = read_response(req->res_addr.path);

  return res;
}

// Devuelve la respuesta a un request.
static t_responseADT read_response(char * fifo) {
  return read_from_fifo(fifo, sizeof(struct t_response));
}

t_requestADT read_request(t_connectionADT con) {
  return read_from_fifo(con->path, sizeof(struct t_request));
}

// Lee algo de tamaño size de fifo
static void * read_from_fifo(char * fifo, int size) {
  int n, fd = open(fifo, O_RDONLY);
  void * res;

  if (fd < 0)
    return NULL;

  res = malloc(size);

  n = read(fd, res, size);
  close(fd);

  if (n < 1) {
    free(res);
    return NULL;
  }

  return res;
}

// Escribe algo de tamaño size al fifo
static int write_to_fifo(char * fifo, void * content, int size) {
  int n, fd = open(fifo, O_WRONLY);

  if (fd < 0)
    return -1;

  n = write(fd, content, size);
  close(fd);

  if (n < 1)
    return -1;

  return 0;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_responseADT res) {
  int rc = write_to_fifo(req->res_addr.path, res, sizeof(struct t_response));
  if (rc == 0)
    free(req);

  return rc;
}
