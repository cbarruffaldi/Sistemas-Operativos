#ifndef _IPC_H_
#define _IPC_H_

#define BUFSIZE 1024

typedef struct {
  char msg[BUFSIZE];
} t_response;

#define SERVER_FIFO_PATH "fifo_server"

typedef struct t_request * t_requestADT;
typedef struct t_address * t_addressADT;
typedef struct t_connection * t_connectionADT;

// Funciones orientadas a client.c
// int server_WR_fd();
t_requestADT create_request(t_addressADT addr);
void set_request_msg(t_requestADT req, char *msg);
t_response send_request(t_connectionADT con, t_requestADT req);
void free_request(t_requestADT req);
t_addressADT create_address(char * path);
t_connectionADT connect(t_addressADT addr);
void disconnect(t_connectionADT con);
t_connectionADT listen(t_addressADT addr);


// Funciones orientadas a server.c
// int server_RD_fd();
t_requestADT read_request(t_connectionADT con);
void get_request_msg(t_requestADT req, char *buffer);
void send_response(t_requestADT req, t_response res);

#endif
