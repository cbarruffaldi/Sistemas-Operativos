#include "IPC.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente
#define ARG_COUNT 2

int main(int argc, char *argv[])
{
  t_responseADT res;
  t_connectionADT con;
  t_addressADT sv_addr;
  t_requestADT req;
  char answer[BUFSIZE];
  char buffer[BUFSIZE];

  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
	return 1;
  }

  sv_addr = create_address(argv[1]);
  req = create_request();
  con = connect_peer(sv_addr);

  if (con == NULL) {
    printf("failed to connect\n");
    return 1;
  }

  printf("Client communicated to server\n");
  while (1) {
    printf("> ");
    fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar
    buffer[strlen(buffer) - 1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      free_request(req);
      free_address(sv_addr);
      disconnect(con);
      return 0;
    }

    set_request_msg(req, buffer);     // settea el input en el request

    printf("Sending request...\n");
    res = send_request(con, req);    // envía request y recibe respuesta

    if (res == NULL)
      fprintf(stderr, "Failed to send request\n");
    else {
      get_response_msg(res, answer);
      printf("Request sent by client and received response:\n%s\n", answer);
    }
  }

  return 1;
}
