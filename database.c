#include <sqlite3.h>
#include "IPC.h"

typedef struct {
  t_connectionADT con;
  pthread_mutex_t *mutex;
} pthread_data;

int create_thread(t_connectionADT con, pthread_mutex_t *mutex);
void * attend(void * p);

int main(int argc, char *argv[]) {
  t_connectionADT con;
  t_addressADT db_addr;
  pthread_mutex_t *mutex;
  int rc;

  mutex = malloc(sizeof(*mutex));
  pthread_mutex_init(mutex, NULL);

  db_addr = create_address(argv[1]);

  if (listen_peer(db_addr) < 0) {
    fprintf(stderr, "Cannot listen\n");
    return 1;
  }

  while (1) {
    printf("Awaiting accept...\n");

    con = accept_peer(db_addr);

    if (con == NULL) {
      printf("Accept failed.\n");
      return 0;
    }

    printf("Accepted!\n");

    rc = create_thread(con, mutex);

    if (rc) {
      printf("Failed to create thread\n");
      return 1;
    }
  }
}

int create_thread(t_connectionADT con, pthread_mutex_t *mutex) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->con = con;
  thdata->mutex = mutex;
  return pthread_create(&thread, NULL, attend, thdata);
}

void * attend(void * p) {
  char msg[BUFSIZE];
  pthread_data *data = (pthread_data *) p;
  t_connectionADT con = data->con;
  pthread_mutex_t *lock = data->mutex;

  free(p);
  

}
