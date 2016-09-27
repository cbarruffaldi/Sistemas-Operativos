#include "IPC.h"

#define SHUTDOWN "shutdown" // Si recibe "shutdown" se apaga el server
#define LOCK "lock"
#define UNLOCK "unlock"
#define ARG_COUNT 2

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
  int id;
  t_connectionADT con;
  pthread_mutex_t *lock;
} pthread_data;

void * attend(void * con);
int create_thread(int id, t_connectionADT con, pthread_mutex_t *lock);
int read_msg(t_connectionADT con, char msg[]);

int main(int argc, char *argv[])
{
  t_connectionADT con;
  t_addressADT sv_addr;
  pthread_mutex_t *lock;
  int rc, id = 0;

  /* inicializa mutex en espacio donde todos los threads lo pueden compartir: heap */
  lock = malloc(sizeof(*lock));
  pthread_mutex_init(lock, NULL);

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

    rc = create_thread(id++, con, lock);

    if (rc) {
		  printf("Failed to create thread\n");
		  return 1;
    }
  }
}

int create_thread(int id, t_connectionADT con, pthread_mutex_t *lock) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->id = id;
  thdata->con = con;
  thdata->lock = lock;
  return pthread_create(&thread, NULL, attend, thdata);
}

void * attend(void * p) {
	pthread_data *data = (pthread_data *) p;
  char msg[BUFSIZE];
  int id = data->id;
	t_connectionADT con = data->con;
  pthread_mutex_t *lock = data->lock;

	// ya se copiaron los datos en id y en con; se libera data
  // Otra variante es no usar las variables locales y manejarse con data
	free(data);

  while(1) {

    if (read_msg(con, msg) == -1)
      pthread_exit(NULL);

    if (strcmp(SHUTDOWN, msg) == 0) {
      printf("Shutting down...\n");
      disconnect(con);
      pthread_exit(NULL);
    }

    printf("Attempting to lock\n");
    pthread_mutex_lock(lock);
    printf("Locked\n");

    printf("[%d] Msg:%s\n", id, msg);

    if (strcmp(msg, LOCK) != 0) {
      pthread_mutex_unlock(lock);
      printf("Unlocked\n");
    }
    else {

      while (strcmp(msg, UNLOCK) != 0) {
        read_msg(con, msg);
        printf("[%d] Locked msg: %s\n", id, msg);
      }

      pthread_mutex_unlock(lock);
      printf("Unlocked\n");
    }
  }

  pthread_exit(NULL);
}

int read_msg(t_connectionADT con, char msg[]) {
  t_requestADT req;
  t_response res = {.msg = "HEY!"};

  printf("Reading request...\n");

  if ((req = read_request(con)) == NULL)  {
    fprintf(stderr, "Error reading request\n");
    return -1;
  }

  get_request_msg(req, msg);      // Copia el mensaje de req en msg

  if (send_response(req, res) < 0) {   // Responde a cliente
    fprintf(stderr, "error sending response\n");
    return -1;
  }

  return strlen(msg);
}
