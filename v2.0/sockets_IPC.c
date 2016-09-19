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

#define MAX_CONNECTIONS 10
#define PORT 5001

struct t_address {
  char hostname[BUFSIZE];
  int listen_fd;
  struct sockaddr_in sockaddr;
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

t_response send_request(t_addressADT addr, t_requestADT req) {
  t_response res = {.msg = "\0"};
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0)
    return res;

  // Pide respuesta si se pudo enviar el request
  if (send(fd, req, (sizeof(struct t_request)), 0) > 0)
    recv(fd, &res, (sizeof(res)), 0);

  close(fd);

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

  strcpy(addr->hostname, hostname);

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

int listen_peer(t_addressADT addr) {
  addr->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(addr->listen_fd, (struct sockaddr *) &(addr->sockaddr), sizeof(addr->sockaddr)) < 0 ||
                listen(addr->listen_fd, MAX_CONNECTIONS) < 0) {
    return -1;
  }

  return 0;
}

void unlisten_peer(t_addressADT addr) {
  close(addr->listen_fd);
}

t_requestADT read_request(t_addressADT addr) {
  struct sockaddr_in peer_sockaddr;
  unsigned int peer_len = sizeof(peer_sockaddr);
  int peer_sock;
  t_requestADT req;

  if ((peer_sock = accept(addr->listen_fd, (struct sockaddr *) &peer_sockaddr, &peer_len)) < 0)
    return NULL;

  req = malloc(sizeof(struct t_request));
  if (recv(peer_sock, req, sizeof(struct t_request), 0) < 1) {
    free(req);
    return NULL;
  }

  req->res_fd = peer_sock;

  return req;
}

void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

int send_response(t_requestADT req, t_response res) {
  int n = send(req->res_fd, &res, sizeof(res), 0);

  close(req->res_fd);
  free_request(req);

  return n < 1 ? -1 : 0;
}
