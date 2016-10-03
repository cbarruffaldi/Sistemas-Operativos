
#include "include/IPC.h"

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

#define HOSTNAME_SIZE 64
#define MAX_CONNECTIONS 1024
#define DELIMITATOR ':'

static t_addressADT create_address_port(const char * hostname, int port);

struct t_request {
  char msg[BUFSIZE];
  int res_fd;
};

struct t_address {
  int listen_fd; // solo se usa si pasó por un listen
  struct sockaddr_in sockaddr;
};

struct t_connection {
  int fd;
};

struct t_response {
  char msg[BUFSIZE];
};

t_requestADT create_request() {
  t_requestADT req = malloc(sizeof(struct t_request));
  return req;
}

t_responseADT create_response() {
  return malloc(sizeof(struct t_response));
}

t_addressADT create_address(const char * host) {
  char hostname[HOSTNAME_SIZE];
  char * occurrence;
  strcpy(hostname, host);
  occurrence = strchr(hostname, DELIMITATOR);
  if (occurrence == NULL)
    return NULL;
  *occurrence = '\0';
  return create_address_port(hostname, atoi(occurrence+1));
}

static t_addressADT create_address_port(const char * hostname, int port) {
  struct hostent *he;
  struct sockaddr_in sockaddr;
  t_addressADT addr;

 if ((he = gethostbyname(hostname)) == NULL || port < 1)
    return NULL;

  addr = malloc(sizeof(struct t_address));

  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(port);
  sockaddr.sin_addr = *((struct in_addr *) he->h_addr);
  bzero(&(sockaddr.sin_zero), sizeof(sockaddr.sin_zero));

  addr->sockaddr = sockaddr;

  return addr;
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

int listen_peer(t_addressADT addr) {
  addr->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(addr->listen_fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0 ||
                listen(addr->listen_fd, MAX_CONNECTIONS) < 0) {
    close(addr->listen_fd);
    return -1;
  }

  return 0;
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

void set_response_msg(t_responseADT res, const char *msg) {
  strcpy(res->msg, msg);
}

void set_request_msg(t_requestADT req, const char *msg) {
  strcpy(req->msg, msg);
}

void get_response_msg(t_responseADT res, char *buffer) {
  strcpy(buffer, res->msg);
  free(res);
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

t_responseADT send_request(t_connectionADT con, t_requestADT req) {
  int n = 0;
  t_responseADT res = malloc(sizeof(struct t_response));

  // Pide respuesta si se pudo enviar el request
  if (send(con->fd, req, (sizeof(struct t_request)), 0) > 0)
    n = recv(con->fd, res, (sizeof(struct t_response)), 0);

  if (n < 1) {
    free(res);
    return NULL;
  }

  return res;
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

int send_response(t_requestADT req, t_responseADT res) {
  if (send(req->res_fd, res, sizeof(struct t_response), 0) < 1)
    return -1;

  free(req);
  return 0;
}

void disconnect(t_connectionADT con) {
  if (con != NULL) {
    close(con->fd);
    free(con);
  }
}

void unaccept(t_connectionADT con) {
  if (con != NULL) {
    close(con->fd);
    free(con);
  }
}

void unlisten_peer(t_addressADT addr) {
  if (addr != NULL)
    close(addr->listen_fd);
}

void free_response(t_responseADT res) {
  if (res != NULL)
    free(res);
}

void free_request(t_requestADT req) {
  if (req != NULL)
    free(req);
}

void free_address(t_addressADT addr) {
  if (addr != NULL)
    free(addr);
}