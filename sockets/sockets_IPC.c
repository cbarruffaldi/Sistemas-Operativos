#include "sockets_data.h"
#include "../IPC.h"

t_addressADT create_address(char * path) {
  
}

t_requestADT create_request(t_addressADT addr);
void set_request_msg(t_requestADT req, char *msg);
t_response send_request(t_connectionADT con, t_requestADT req);
void free_request(t_requestADT req);
t_connectionADT connect(t_addressADT addr);
void disconnect(t_connectionADT con);
t_connectionADT listen(t_addressADT addr);


t_requestADT read_request(t_connectionADT con);
void get_request_msg(t_requestADT req, char *buffer);
void send_response(t_requestADT req, t_response res);
