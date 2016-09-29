#include "include/server_marshalling.h"
#include "include/IPC.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_COUNT 3
#define DATABASE_PROCESS "database.bin"

typedef struct {
  char *db_path;
  t_sessionADT session;
} pthread_data;

//argv[1] = nombre del path a server,
//argv[2] = nombre del path a  base de datos

void * run_thread(void * p);

int main(int argc, char *argv[])
{
  t_master_sessionADT master_session;
  t_sessionADT session;

  if (argc != ARG_COUNT) {
    printf("Usage: %s <server_path> <database_path>\n", argv[0]);
    return 1;
  }

  if (fork() == 0) {
      execl(DATABASE_PROCESS, DATABASE_PROCESS, argv[2], NULL);
      printf("FORK no se debería imprimir\n");
  }

  master_session = setup_master_session(argv[1]);

  while (1) {
    session = accept_client(master_session);
    create_thread(argv[2], session);
  }
}

int create_thread(char * db_path, t_sessionADT session) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->db_path = db_path;
  thdata->session;
  return pthread_create(&thread, NULL, run_thread, thdata);
}

void * run_thread(void * p) {
  pthread_data *thdata = (pthread_data *) p;
  t_addressADT addr = create_address(thdata->db_path);
  t_connectionADT con = connect_peer(addr);
  t_sessionADT session = thdata->session;
  free(p);

  attend(session);
}