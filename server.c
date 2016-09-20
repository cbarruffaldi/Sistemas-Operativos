#include "IPC.h"

#define SHUTDOWN "shutdown" // Si recibe "shutdown" se apaga el server
#define ARG_COUNT 2

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int attend(t_connectionADT con);

int main(int argc, char *argv[])
{
  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
  return 1;
  }

  t_connectionADT con;
  t_addressADT sv_addr;

  printf("Opening channel...\n");
  sv_addr = create_address(argv[1]);

  if (sv_addr == NULL) {
    printf("Failed to create address\n");
    return 1;
  }

  if (listen_peer(sv_addr) < 0) {
    fprintf(stderr, "Cannot listen\n");
    return 1;
  }

  printf("Server listening\n");


  while (1) {
    printf("Awaiting accept...\n");

    con = accept_peer(sv_addr);

    if (con == NULL) {
      printf("Accept failed.\n");
      return 0;
    }

    printf("Accepted!\n");

    if (fork() == 0)
      return attend(con);
  }
}

int attend(t_connectionADT con) {
    char msg[BUFSIZE];
    int pid = getpid();
    t_requestADT req;
    t_response res = {.msg = "HEY!"};

    while(1) {

      printf("Reading request...\n");

      if ((req = read_request(con)) == NULL)  {
        fprintf(stderr, "error reading request\n");
        return 1;
      }

      get_request_msg(req, msg);      // Copia el mensaje de req en msg
      printf("Request read by server\n");
      printf("Server pid: %d\nmsg: %s\n", pid, msg);

      if (send_response(req, res) < 0) {   // Responde a cliente
        fprintf(stderr, "error sending response\n");
        return 1;
      }

      if (strcmp(SHUTDOWN, msg) == 0) {
        printf("Shutting down...\n");
        disconnect(con);
        return 0;
      }
    }

    return 1;
}
