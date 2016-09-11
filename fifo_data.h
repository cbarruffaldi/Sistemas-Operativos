#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_PATH "fifo_server"
#define CLIENT_FIFO_PATH "fifo_client%d"  // Se le concatena el client PID. Se asegura unicidad.

#define MAX_CLIENTS 32
#define CLIENT_FIFO_LEN 16

#define BUFSIZE 1024

typedef struct {
  char msg[BUFSIZE];
  char res_fifo[BUFSIZE];
} t_request;

typedef struct {
  char msg[BUFSIZE];
} t_response;

typedef t_request * t_requestADT;