#include "IPC.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define MAX_CONNECTIONS 10
#define PORT 5001

struct t_address {
  int listen_fd; // solo se usa si pasó por un listen
  struct sockaddr_in sockaddr;
};

struct t_connection {
  int fd;
};

struct t_request {
  char msg[BUFSIZE];
  int res_fd;
};

t_requestADT create_request() {
  t_requestADT req = malloc(sizeof(struct t_request));
  return req;
}

void set_request_msg(t_requestADT req, char *msg) {
  strcpy(req->msg, msg);
}

t_response send_request(t_connectionADT con, t_requestADT req) {
  t_response res = {.msg = "\0"};

  // Pide respuesta si se pudo enviar el request
  if (send(con->fd, req, (sizeof(struct t_request)), 0) > 0)
    recv(con->fd, &res, (sizeof(res)), 0);

  return res;
}

void free_request(t_requestADT req) {
  free(req);
}

t_addressADT create_address(char * hostname) {
  struct hostent *he;
  struct sockaddr_in sockaddr;
  t_addressADT addr;

 if ((he = gethostbyname(hostname)) == NULL)
    return NULL;

  addr = malloc(sizeof(struct t_address));

  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(PORT);
  sockaddr.sin_addr = *((struct in_addr *) he->h_addr);
  bzero(&(sockaddr.sin_zero), sizeof(sockaddr.sin_zero));

  addr->sockaddr = sockaddr;

  return addr;
}

void free_address(t_addressADT addr) {
  free(addr);
}

t_connectionADT accept_peer(t_addressADT addr) {
  struct sockaddr_in peer_sockaddr;
  unsigned int peer_len = sizeof(peer_sockaddr);
  int peer_sock;
  t_connectionADT con;

  if ((peer_sock = accept(addr->listen_fd, (struct sockaddr *) &peer_sockaddr, &peer_len)) < 0)
    return NULL;

  con = malloc(sizeof(struct t_connection));
  con->fd = peer_sock;

  return con;
}

t_connectionADT connect_peer(t_addressADT addr) {
  t_connectionADT con = malloc(sizeof(struct t_connection));
  con->fd = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(con->fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0) {
    free(con);
    close(con->fd);
    return NULL;
  }

  return con;
}

void disconnect(t_connectionADT con) {
  free(con);
}

int listen_peer(t_addressADT addr) {
  addr->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(addr->listen_fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0 ||
                listen(addr->listen_fd, MAX_CONNECTIONS) < 0) {
    close(addr->listen_fd);
    return -1;
  }

  return 0;
}

void unlisten_peer(t_connectionADT con) {
  free(con);
}

t_requestADT read_request(t_connectionADT con) {
  t_requestADT req = malloc(sizeof(struct t_request));

  if (recv(con->fd, req, sizeof(struct t_request), 0) < 1) {
    free(req);
    return NULL;
  }

  req->res_fd = con->fd;
  return req;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_response res) {
  if (send(req->res_fd, &res, sizeof(res), 0) < 1)
    return -1;

  free_request(req);
  return 0;
}
