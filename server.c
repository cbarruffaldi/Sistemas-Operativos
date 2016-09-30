#include "include/server_marshalling.h"
#include "include/IPC.h"
#include "include/server.h"
#include "include/query.h"
#include "include/log.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define ARG_COUNT 3
#define DATABASE_PROCESS "database.bin"
#define PATH_SIZE 64

typedef struct {
  long mtype;
  char mtext[BUFSIZE];
}queue_buffer;

typedef struct {
  char db_path[PATH_SIZE];
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
int send_mq(char * msg, int priority);

int msq_id; //id de la MQ

int main(int argc, char *argv[])
{
  t_master_sessionADT master_session;
  t_sessionADT session;
  key_t key; //Key de la MQ

  if (argc != ARG_COUNT) {
    printf("Usage: %s <server_path> <database_path>\n", argv[0]);
    return 1;
  }

  if (fork() == 0) {
      execl(DATABASE_PROCESS, DATABASE_PROCESS, argv[2], NULL);
      printf("FORK no se debería imprimir\n");
  }

  master_session = setup_master_session(argv[1]);


  printf("SETTING UP MQ\n");
  if ((key = ftok("server.c", 'B')) == -1) {
      perror("ftok");          exit(1);
  }

  if ((msq_id = msgget(key, 0644 | IPC_CREAT)) == -1) {
      perror("msgget");          exit(1);
  }
  send_mq(DATASE_CORRECT_NOTIFICATION,2); //TODO: cambiar el 2


  while (1) {
    printf("[SV]: Accepting...\n");
    session = accept_client(master_session);
    printf("[SV]: ACCEPTED!\n");
    send_mq(CLIENT_ACCEPTED,2); //TODO: VER LA PRIOIDAD (DEBE SER > 0 )
    create_thread(argv[2], session);
  }
}

int create_thread(char * db_path, t_sessionADT session) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  strcpy(thdata->db_path, db_path);
  thdata->session = session;
  return pthread_create(&thread, NULL, run_thread, thdata);
}

void * run_thread(void * p) {
  int valid;
  pthread_data *thdata = (pthread_data *) p;
  t_sessionADT session = thdata->session;
  t_requestADT req = create_request();
  t_addressADT addr = create_address(thdata->db_path);
  t_connectionADT con;

  if (addr == NULL) {
    printf("[SV]: Failed to create address\n");
    pthread_exit(NULL);
  }

  con = connect_peer(addr);

  if (con == NULL) {
    printf("[SV]: Failed to connect to DB\n");
    pthread_exit(NULL);
  }

  t_session_data se_data;

  free(p); // Ya se extrajeron los datos --> se libera

  printf("Thread created!\n");

  se_data.db_con = con;
  se_data.req = req;

  set_session_data(session, &se_data);

  valid = attend(session);

  if (valid == 0) {
    printf("Client disconnected\n");
    send_mq(CLIENT_DISCONNECTED,2);
  }
  else if (valid == -1) {
    printf("Failed to send response\n");
  }
  
  disconnect(con);
  free_request(req);
  free_address(addr);
  pthread_exit(NULL);
}

// TODO: CAMBIAR LOS BUFSIZE
int sv_tweet(void * p, char * user, char * msg) {
  char mq_msg[MAX_NOTIFICATION];
  char buffer[BUFSIZE], res[BUFSIZE];
  printf("RECEIVED SV_TWEET_WITH \nuser:%s \nmsg:%s\n", user, msg);
  query_insert(buffer, user, msg);
  send_query(p, buffer, res);

  sprintf(mq_msg,TWEET_NOTIFICATION,user,res,msg);
  send_mq(mq_msg,2); //TODO: cambiar el 2

  return atoi(res);
}

//TODO: por ahora usa res para guardar la respuesta de la BD, debería devolver arreglo de t_tweet
t_tweet * sv_refresh(void * p, int last_id, char res[]) {
  char buffer[BUFSIZE],mq_msg[MAX_NOTIFICATION];
  printf("RECEIVED SV_REFRESH WITH \nid:%d\n", last_id);
  query_refresh(buffer, last_id);
  send_query(p, buffer, res);
<<<<<<< HEAD

  sprintf(mq_msg,REFRESH_NOTIFICATION);
  send_mq(mq_msg,2); //TODO: cambiar el 2

=======
  printf("Received from DB REFRESH: %s \n", res);
>>>>>>> c7c4e9ac4570d6493d61582afcfaea1e07a0ea00
  return NULL;
}

int sv_like(void * p, int id) {
  char buffer[BUFSIZE], res[BUFSIZE];
  char mq_msg[MAX_NOTIFICATION];
  printf("RECEIVED SV_LIKE WITH \nid:%d\n", id);
  query_like(buffer, id);
  send_query(p, buffer, res);

  sprintf(mq_msg,LIKE_NOTIFICATION,id);
  send_mq(mq_msg,2); //TODO:cambiar el 2

  return atoi(res);
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

int send_mq(char * msg, int priority){
  queue_buffer * buf = malloc(sizeof(*buf));
  int len = strlen(msg);
  buf->mtype = priority;

  strcpy(buf->mtext, msg);
  if (msgsnd(msq_id, buf, len+1, 0) == -1) {
      perror("ERROR ON MSGSND...");
      printf ("%d, %d, %s, %d\n", msq_id, buf->mtype, buf->mtext, len);
      free(buf);
      return 1;
  }
  free(buf);
  return 0;
}
