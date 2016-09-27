#include "IPC.h"
#include "client_marshalling.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente
#define ARG_COUNT 2

int main(int argc, char *argv[])
{
  sessionADT se;
  char buffer[BUFSIZE];

  if (argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
	   return 1;
  }

  se = start_session(argv[1]);

  if (se == NULL) {
    printf("failed to connect\n");
    return 1;
  }

  printf("Client communicated to server\n");
  while (1) {
    printf("> ");

    // fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar

    fgets(buffer, MSG_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      end_session(se);
      return 0;
    }

    int res = send_tweet(se, "lusho", buffer);

    printf("Respuesta al tweet: %d\n", res);
  }

  return 1;
}
