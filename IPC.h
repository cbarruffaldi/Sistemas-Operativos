#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 1024

typedef struct {
  char msg[BUFSIZE];
} t_response;

typedef struct t_request * t_requestADT;

// Funciones orientadas a client.c
int server_WR_fd();
t_requestADT create_request();
void set_request_msg(t_requestADT req, char *msg);
t_response send_request(int fd, t_requestADT req);
void free_request(t_requestADT req);

// Funciones orientadas a server.c
int server_RD_fd();
t_requestADT read_request(int fd);
void get_request_msg(t_requestADT req, char *buffer);
void send_response(t_requestADT req, t_response res);

#endif