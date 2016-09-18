#include "IPC.h"

#define SHUTDOWN "shutdown" // Si recibe "shutdown" se apaga el server
#define ARG_COUNT 2

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{

  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
  return 1;
  }

  t_response res = {.msg = "HEY!"};
  t_requestADT req;

  char msg[BUFSIZE];

  printf("Opening channel...\n");
  t_addressADT server_addr = create_address(argv[1]);

  t_connectionADT sv_con = listen_peer(server_addr);

  printf("Server listening\n");

  while (1) {
    printf("Reading request...\n");

    if ((req = read_request(sv_con)) == NULL)  {
      fprintf(stderr, "error reading request\n");
      return 1;
    }

    get_request_msg(req, msg);      // Copia el mensaje de req en msg
    printf("Request read by server\n");
    printf("msg: %s\n", msg);

    if (send_response(req, res) < 0) {   // Responde a cliente
      fprintf(stderr, "error sending response\n");
      return 1;
    }

    if (strcmp(SHUTDOWN, msg) == 0) {
      printf("Shutting down...\n");
      unlisten(sv_con);
      return 0;
    }
  }
}