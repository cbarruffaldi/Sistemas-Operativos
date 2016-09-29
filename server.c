#include "include/server_marshalling.h"
#include "include/IPC.h"
#include "include/server.h"
#include "include/query.h"

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

void send_query(t_session_data * data, const char *sql, char result[]);
void * run_thread(void * p);
void print_session_data(t_session_data *data);
int create_thread(char * db_path, t_sessionADT session);

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
  int valid;
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

  valid = attend(session);

  if (valid == 0) {
    printf("Client disconnected\n");
  }
  else if (valid == -1) {
    printf("Failed to send response\n");
  }

  pthread_exit(NULL);
}

// TODO: CAMBIAR LOS BUFSIZE
int sv_tweet(void * p, char * user, char * msg) {
  char buffer[BUFSIZE], res[BUFSIZE];
  printf("RECEIVED SV_TWEET_WITH \nuser:%s \nmsg:%s\n", user, msg);
  query_insert(buffer, user, msg);
  send_query(p, buffer, res);
  return 27;
}

t_tweet * sv_refresh(void * p, int last_id, char res[]) {
  char buffer[BUFSIZE];
  printf("RECEIVED SV_REFRESH WITH \nid:%d\n", last_id);
  query_refresh(buffer, last_id);
  send_query(p, buffer, res);

  return NULL;
}

int sv_like(void * p, int id) {
  char buffer[BUFSIZE], res[BUFSIZE];
  printf("RECEIVED SV_LIKE WITH \nid:%d\n", id);
  query_like(buffer, id);
  send_query(p, buffer, res);
  return 35;
}

void send_query(t_session_data * data, const char *sql, char result[]) {
  t_requestADT req = data->req;
  t_connectionADT con = data->db_con;
  t_responseADT res;

  set_request_msg(req, sql);
  res = send_request(con, req);
  if (res == NULL) {
    printf("Error on database response\n");
    return;
  }

  get_response_msg(res, result);
  printf("%s\n", result);
}
