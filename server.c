#include "include/server_marshalling.h"
#include "include/IPC.h"
#include "include/server.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARG_COUNT 3
#define DATABASE_PROCESS "database.bin"

typedef struct {
  char *db_path;
  t_sessionADT session;
} pthread_data;

typedef struct {
  t_connectionADT db_con;
  t_requestADT req;
} t_session_data;

//argv[1] = nombre del path a server,
//argv[2] = nombre del path a  base de datos

void * run_thread(void * p);
void print_session_data(t_session_data *data);

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
    printf("[SV]: Accepting...\n");
    session = accept_client(master_session);
    printf("[SV]: ACCEPTED!\n");
    create_thread(argv[2], session);
  }
}

int create_thread(char * db_path, t_sessionADT session) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->db_path = db_path;
  thdata->session = session;
  return pthread_create(&thread, NULL, run_thread, thdata);
}

void * run_thread(void * p) {
  pthread_data *thdata = (pthread_data *) p;
  t_addressADT addr = create_address(thdata->db_path);
  t_connectionADT con = connect_peer(addr);
  t_sessionADT session = thdata->session;
  t_requestADT req = create_request();

  t_session_data se_data;

  free(p);

  printf("Thread created!\n");

  se_data.db_con = con;
  se_data.req = req;

  set_session_data(session, &se_data);

  attend(session);
}


t_tweet * sv_tweet(void * p, char * user, char * msg, int last_id) {
  printf("RECEIVED SV_TWEET_WITH \nuser:%s \nmsg:%s \nid:%d\n", user, msg, last_id);
  print_session_data(p);
  return NULL;
}

t_tweet * sv_refresh(void * p, int last_id) {
  printf("RECEIVED SV_REFRESH WITH \nid:%d\n", last_id);
  print_session_data(p);
  return NULL;
}

void sv_like(void * p, int id) {
  printf("RECEIVED SV_LIKE WITH \nid:%d\n", id);
  print_session_data(p);
}

void print_session_data(t_session_data *data) {
  printf("connection: %p\n", data->db_con);
  printf("request: %p\n", data->req);
}
