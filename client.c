#include "IPC.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente
#define PEER_FIFO_PATH "fifo-peer-%d"  // Se le concatena el client PID. Se asegura unicidad.

int main()
{
  // int server_fd = server_WR_fd();
  t_addressADT sv_addr = create_address(SERVER_FIFO_PATH);

  t_connectionADT sv_conn = connect(sv_addr);

  char buffer[BUFSIZE] = "\0"; // Buffer de input de stdin
  t_response res;

  char peer_path[BUFSIZE];
  sprintf(peer_path, PEER_FIFO_PATH, getpid());
  t_addressADT peer_addr = create_address(peer_path);
  t_requestADT req = create_request(peer_addr);

  while (1) {
    fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar
    buffer[strlen(buffer)-1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      // close(sv_conn); // TODO: tendría que ser parte de la interfaz IPC.h
      free_request(req);
      return 0;
    }

    set_request_msg(req, buffer);     // settea el input en el request
    printf("Sending request...\n");
    res = send_request(sv_conn, req);    // envía request y recibe respuesta
    printf("Request sent by client and received response:\n%s\n", res.msg);
  }

  return 1;
}
