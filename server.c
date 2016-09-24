#include "IPC.h"

#define SHUTDOWN "shutdown" // Si recibe "shutdown" se apaga el server
#define ARG_COUNT 2

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

typedef struct {
  int id;
  t_connectionADT con;
} pthread_data;

void * attend(void * con);
int create_thread(int id, t_connectionADT con);

int main(int argc, char *argv[])
{
  t_connectionADT con;
  t_addressADT sv_addr;
  int rc, id = 0;

  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
  return 1;
  }

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

    rc = create_thread(id++, con);

    if (rc) {
		  printf("Failed to create thread\n");
		  return 1;
    }
  }
}

int create_thread(int id, t_connectionADT con) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->id = id;
  thdata->con = con;
  return pthread_create(&thread, NULL, attend, thdata);
}

void * attend(void * p) {
	pthread_data *data = (pthread_data *) p;
  char msg[BUFSIZE];
  int id = data->id;
	t_connectionADT con = data->con;
  t_requestADT req;
  t_response res = {.msg = "HEY!"};

	// ya se copiaron los datos en id y en con; se libera data
	free(data);

  printf("thread con: %p\nthread id: %d\n", con, id);

  while(1) {

    printf("Reading request...\n");

    if ((req = read_request(con)) == NULL)  {
      fprintf(stderr, "error reading request\n");
      pthread_exit(NULL);
    }

    get_request_msg(req, msg);      // Copia el mensaje de req en msg
    printf("Request read by thread\n");
    printf("Thread id: %d\nMsg: %s\n", id, msg);

    if (send_response(req, res) < 0) {   // Responde a cliente
      fprintf(stderr, "error sending response\n");
      pthread_exit(NULL);
    }

    if (strcmp(SHUTDOWN, msg) == 0) {
      printf("Shutting down...\n");
      disconnect(con);
      pthread_exit(NULL);
    }
  }

  pthread_exit(NULL);
}
