#include "IPC.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente

int main()
{
  int server_fd = server_WR_fd();
  char buffer[BUFSIZE] = "\0"; // Buffer de input de stdin
  t_response res;
  t_requestADT req = create_request();

  while (1) {
    fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar
    buffer[strlen(buffer)-1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      close(server_fd); // TODO: tendría que ser parte de la interfaz IPC.h
      free_request(req);
      return 0;
    }

    set_request_msg(req, buffer);     // settea el input en el request
    printf("Sending request...\n");
    res = send_request(server_fd, req);    // envía request y recibe respuesta
    printf("Request sent by client and received response:\n%s\n", res.msg);
  }

  return 1;
}