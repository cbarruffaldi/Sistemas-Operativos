#include "IPC.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente
#define ARG_COUNT 2

int main(int argc, char *argv[])
{
  
  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
	return 1;
  }

  t_addressADT sv_addr = create_address(argv[1]);

  char buffer[BUFSIZE] = "\0"; // Buffer de input de stdin
  t_response res;

  t_requestADT req = create_request();

  printf("Client communicated to server\n");
  while (1) {
    printf("> ");
    fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar
    buffer[strlen(buffer) - 1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      free_request(req); // TODO: raro el free_request
      free_address(sv_addr);
      return 0;
    }

    set_request_msg(req, buffer);     // settea el input en el request

    printf("Sending request...\n");
    res = send_request(sv_addr, req);    // envía request y recibe respuesta
    
    if (res.msg[0] == '\0')
      fprintf(stderr, "Failed to send request\n");
    else
      printf("Request sent by client and received response:\n%s\n", res.msg);
  }

  return 1;
}
