#include "include/server_marshalling.h"
#include "include/db_marshalling.h"
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

#include <sys/stat.h>
#include <mqueue.h>

#define ARG_COUNT 3
#define DATABASE_PROCESS "database.bin"
#define PATH_SIZE 64


typedef struct {
  char db_path[PATH_SIZE];
  t_sessionADT session;
} pthread_data;

typedef struct {
  t_DBsessionADT db_se;
  char user[USER_SIZE];
} t_session_data;

//argv[1] = nombre del path a server,
//argv[2] = nombre del path a  base de datos

static void * run_thread(void * p);
static int create_thread(char * db_path, t_sessionADT session);
static int logged(t_session_data * data);
static void send_mq(char * msg, int priority);

mqd_t mq; //Para la MQ

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

 /* open the mail queue */
  mq = mq_open(QUEUE_NAME, O_WRONLY);
  CHECK((mqd_t)-1 != mq);
  
  master_session = setup_master_session(argv[1]);

  if (master_session == NULL) {
    printf("Cannot init session\n");
    send_mq(CANNOT_INIT_SESSION,ERROR);
    return 0;
  }

  send_mq(DATASE_CORRECT_NOTIFICATION,INFO); 


  while (1) {
    printf("[SV]: Accepting...\n");
    session = accept_client(master_session);
    if (session != NULL) {
      printf("[SV]: ACCEPTED!\n");
      send_mq(CLIENT_ACCEPTED,INFO); 
   
      create_thread(argv[2], session);
    }
    else {
      printf("[SV]: Couldn't open session\n");
      send_mq(CANNOT_OPEN_SESSION,ERROR);
    }
  }
}

static void send_mq(char * msg, int priority) {
  char buffer[MAX_NOTIFICATION]; //TODO: ver tamaño

  strcpy(buffer, msg);

  CHECK(0 <= mq_send(mq, buffer, MAX_NOTIFICATION, priority));
}

static int create_thread(char * db_path, t_sessionADT session) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  strcpy(thdata->db_path, db_path);
  thdata->session = session;
  return pthread_create(&thread, NULL, run_thread, thdata);
}

static void * run_thread(void * p) {
  int valid;
  pthread_data *thdata = (pthread_data *) p;
  t_sessionADT session = thdata->session;
  t_DBsessionADT db_se = start_DBsession(thdata->db_path);

  pthread_detach(pthread_self()); // Se liberan los recursos del thread al cerrarse

  if (db_se == NULL) {
    printf("Failed to connect to DB\n");
    send_mq(CANNOT_CONNECT_DB, ERROR);
  }

  t_session_data se_data;

  free(p); // Ya se extrajeron los datos --> se libera

  printf("Thread created!\n");

  se_data.db_se = db_se;
  se_data.user[0] = '\0';

  set_session_data(session, &se_data);

  valid = attend(session);

  if (valid == 0) {
    printf("Client disconnected\n");
    send_mq(CLIENT_DISCONNECTED, INFO);
  }
  else if (valid == -1) {
    printf("Failed to send response\n");
    send_mq(CANNOT_SEND_RESPONSE, WARNING);
  }

  end_DBsession(db_se);
  unaccept_client(session);
  pthread_exit(NULL);
}

int sv_login(void * p, const char * username) {
  char * user = ((t_session_data *) p)->user;
  char mq_msg[MAX_NOTIFICATION];
  printf("[SV]: RECEIVED SV_LOGIN from %s\n", username);

  if (!logged(p) && strlen(username) < USER_SIZE) {
    printf("[SV]: User logged in as %s\n", username);
    strcpy(user, username);

    sprintf(mq_msg,LOGIN_NOTIFICATION,user);
    send_mq(mq_msg,INFO);

    return 1;
  }

  return 0;
}

int sv_logout(void * p) {
  char mq_msg[MAX_NOTIFICATION];
  char *user = ((t_session_data *) p)->user;
  if (!logged(p))
    return 0;
  printf("User %s logged out\n", user);
  
  sprintf(mq_msg,LOGOUT_NOTIFICATION,user);
  send_mq(mq_msg,INFO);

  user[0] = '\0';

  return 1;
}

int sv_tweet(void * p, const char * msg) {
  char mq_msg[MAX_NOTIFICATION];
  char *username = ((t_session_data *) p)->user;
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;

  printf("RECEIVED SV_TWEET_WITH \nuser:%s \nmsg:%s\n", username, msg);

  if (!logged(p))
    return -1;

  /** TODO: actualizar; ya no se usa más res **/
//  sprintf(mq_msg,TWEET_NOTIFICATION, username, res, msg);
//  send_mq(mq_msg,INFO);

  return send_tweet(db_se, username, msg);
}

//TODO: MQ
int sv_refresh(void * p, int from_id, t_tweet tws[]) {
  char mq_msg[MAX_NOTIFICATION];
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;

  printf("RECEIVED SV_REFRESH WITH \nid:%d\n", from_id);

  if (!logged(p))
    return -1;

  return send_refresh(db_se, from_id, tws);
}

int sv_like(void * p, int id) {
  char mq_msg[MAX_NOTIFICATION];
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;

  printf("RECEIVED SV_LIKE WITH \nid:%d\n", id);

  if (!logged(p))
    return -1;

  sprintf(mq_msg,LIKE_NOTIFICATION,id);
  send_mq(mq_msg,INFO);

  return send_like(db_se, id);
}

// TODO: por ahora recibe res y devuelve void; en realidad deberia devolver un tweet
t_tweet sv_show(void * p, int id) {
  char mq_msg[MAX_NOTIFICATION]; 
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;

  sprintf(mq_msg,SHOW_NOTIFICATION,id);
  send_mq(mq_msg,INFO);

  return send_show(db_se, id);
}

static int logged(t_session_data * data) {
  return data->user[0] != '\0';
}
