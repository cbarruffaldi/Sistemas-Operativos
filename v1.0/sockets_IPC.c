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

static t_connectionADT init_connection(t_addressADT addr);

struct t_address {
  char hostname[BUFSIZE];
  struct sockaddr_in sockaddr;
};

struct t_connection {
  struct t_address addr;
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
  struct t_address addr = con->addr;

  if (connect(con->fd, (struct sockaddr *) &(addr.sockaddr), sizeof(addr.sockaddr)) < 0) {
    disconnect(con);
    perror("devolviendo NULL\n");
    return res;
  }

  printf("IPC sending req\n");
  if (send(con->fd, req, (sizeof(struct t_request)), 0) <= 0)
    perror("error sending req\n");
  else
    printf("IPC req sent\n");

  printf("IPC receiving resp\n");
  if (recv(con->fd, &res, (sizeof(res)), 0) <= 0)
    perror("error receiving resp\n");
  else  
    printf("IPC resp received\n");

  close(con->fd);
  con->fd = socket(AF_INET, SOCK_STREAM, 0);

  return res;
}

void free_request(t_requestADT req) {
  free(req);
}

t_addressADT create_address(char * hostname) {
  struct hostent *he;
  struct sockaddr_in sockaddr;
  t_addressADT addr;

 if ((he = gethostbyname(hostname)) == NULL) {
    perror("devolviendo NULL hostbyname\n");
    return NULL;
 }

  addr = malloc(sizeof(struct t_address));

  strcpy(addr->hostname, hostname);

  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(PORT);
  sockaddr.sin_addr = *((struct in_addr *) he->h_addr);
  bzero(&(sockaddr.sin_zero), sizeof(sockaddr.sin_zero));

  addr->sockaddr = sockaddr;

  return addr;
}

t_connectionADT connect_peer(t_addressADT addr) {
  t_connectionADT con = init_connection(addr);

  return con;
}

void disconnect(t_connectionADT con) {
  free(con);
}

t_connectionADT listen_peer(t_addressADT addr) {
  t_connectionADT con = init_connection(addr);

  if (bind(con->fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0 ||
                listen(con->fd, MAX_CONNECTIONS) < 0) {
    unlisten(con);
    perror("devolviendo NULL\n");
    return NULL;
  }

  return con;
}

void unlisten(t_connectionADT con) {
  free(con);
}

t_requestADT read_request(t_connectionADT con) {
  struct sockaddr_in peer_sockaddr;
  unsigned int peer_len = sizeof(peer_sockaddr);
  int peer_sock;
  t_requestADT req;

  printf("awaiting accept\n");
  if ((peer_sock = accept(con->fd, (struct sockaddr *) &peer_sockaddr, &peer_len)) < 0) {
    perror("devolviendo NULL\n");
    return NULL;
  }

  req = malloc(sizeof(struct t_request));
  recv(peer_sock, req, sizeof(struct t_request), 0);
  req->res_fd = peer_sock;

  return req;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_response res) {
  printf("Sending res\n");
  if (send(req->res_fd, &res, sizeof(res), 0) < 1)
    return -1;

  close(req->res_fd);
  free_request(req);

  return 0;
}

static t_connectionADT init_connection(t_addressADT addr) {
  t_connectionADT con = malloc(sizeof(struct t_connection));
  con->fd = socket(AF_INET, SOCK_STREAM, 0);
  con->addr = *addr;

  return con;
}