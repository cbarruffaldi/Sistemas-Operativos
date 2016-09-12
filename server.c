#include "IPC.h"

#define SHUTDOWN "shutdown" // Si recibe "shutdown" se apaga el server

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
  t_response res = {.msg = "HEY!"};
  t_requestADT req;

  char msg[BUFSIZE];

  printf("Opening pipe...\n");
  // int fd = server_RD_fd();
  t_addressADT server_addr = create_address(SERVER_FIFO_PATH);

  t_connectionADT sv_con = listen(server_addr);

  // if (fd < 0) {
  //   printf("Could not open pipe\n");
  //   return 1;
  // }

  printf("Server listening\n");

  while (1) {
    printf("Reading request...\n");
    req = read_request(sv_con);
    get_request_msg(req, msg);      // Copia el mensaje de req en msg
    printf("Request read by server\n");
    printf("msg: %s\n", msg);

    send_response(req, res);    // Responde a cliente

    if (strcmp(SHUTDOWN, msg) == 0) {
      printf("Shutting down...\n");
      disconnect(sv_con);
//    unlink(SERVER_FIFO_PATH); TODO: función que borre fifos
      return 0;
    }
  }
}
