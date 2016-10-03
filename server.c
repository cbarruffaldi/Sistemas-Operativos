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
#define LOGGER_PROCESS "log.bin"
#define PATH_SIZE 64

#define QUEUE_NAME  "/SV_TWITTER_QUEUE"


typedef struct {
  char db_path[PATH_SIZE];
  t_sessionADT session;
} pthread_data;

typedef struct {
  t_DBsessionADT db_se;
  t_sessionADT sv_se;
  char user[USER_SIZE];
} t_session_data;

//argv[1] = nombre del path a server,
//argv[2] = nombre del path a  base de datos

static void * run_thread(void * p);
static int create_thread(char * db_path, t_sessionADT session);
static int logged(t_session_data * data);
static void send_mq(char * msg, int priority);
static void close_thread(t_session_data * data);

mqd_t mq; //Para la MQ

int main(int argc, char *argv[])
{
  t_master_sessionADT master_session;
  t_sessionADT session;

  if (argc != ARG_COUNT) {
    printf("Usage: %s <server_path> <database_path>\n", argv[0]);
    return 1;
  }

  if (fork() == 0) { // forkea para iniciar el daemon de logging
    execl(LOGGER_PROCESS, LOGGER_PROCESS, QUEUE_NAME, NULL);
  }

 /* open the mail queue */
 /* Intenta abrir la MQ hasta que el proceso forkeado termine de crearla. */
  while ( (mq = mq_open(QUEUE_NAME, O_WRONLY)) == (mqd_t) -1);

  master_session = setup_master_session(argv[1]);

  if (master_session == NULL) {
    send_mq(CANNOT_INIT_SESSION, ERROR);
    return 1;
  }

  while (1) {
    send_mq(SERVER_READY,INFO);
    session = accept_client(master_session);

    if (session != NULL) {
      send_mq(CLIENT_ACCEPTED, INFO);
      create_thread(argv[2], session);
    }
    else 
      send_mq(CLIENT_NOT_ACCEPTED,ERROR);
  }

  end_master_session(master_session);
  return 1;
}

static void send_mq(char * msg, int priority) {
  char buffer[MAX_NOTIFICATION];
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

  free(p); // Ya se extrajeron los datos --> se libera

  if (db_se == NULL) {
    send_mq(CANNOT_CONNECT_DB, ERROR);
    unaccept_client(session);
    pthread_exit(NULL);
  }

  send_mq(ACHIEVED_CONNECT_DATABSE, INFO);

  t_session_data se_data;

  se_data.db_se = db_se;
  se_data.user[0] = '\0';
  se_data.sv_se = session;

  set_session_data(session, &se_data);

  valid = attend(session);

  if (valid == 0) {
    send_mq(CLIENT_DISCONNECTED, INFO);
  }
  else if (valid == -1) {
    send_mq(CANNOT_SEND_RESPONSE, WARNING);
  }

  send_mq(CLOSE_CONNECT_DB, INFO);
  close_thread(&se_data);
  return NULL;
}

int sv_login(void * p, const char * username) {
  char * user = ((t_session_data *) p)->user;
  char mq_msg[MAX_NOTIFICATION];

  if (!logged(p) && strlen(username) < USER_SIZE) {
    strcpy(user, username);

    sprintf(mq_msg,LOGIN_NOTIFICATION,user);
    send_mq(mq_msg,INFO);

    return 1;
  }
  else {
    sprintf(mq_msg, CANNOT_LOGIN_NOTIFICATION, user);
    send_mq(mq_msg, WARNING);
  }

  return 0;
}

int sv_logout(void * p) {
  char mq_msg[MAX_NOTIFICATION];
  char *user = ((t_session_data *) p)->user;
  if (!logged(p))
    return 0;

  sprintf(mq_msg,LOGOUT_NOTIFICATION,user);
  send_mq(mq_msg,INFO);

  user[0] = '\0';

  return 1;
}

int sv_tweet(void * p, const char * msg) {
  char mq_msg[MAX_NOTIFICATION];
  char *username = ((t_session_data *) p)->user;
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;
  int id;

  if (!logged(p))
    return -1;

  sprintf(mq_msg,TWEET_NOTIFICATION, username, msg);
  send_mq(mq_msg,INFO);

  id = send_tweet(db_se, username, msg);

  if (id == ABORT) {
    send_mq(LOST_CONNECT_DB, ERROR);
    close_thread(p);
  }

  return id;
}

int sv_refresh(void * p, int from_id, t_tweet tws[]) {
  char mq_msg[MAX_NOTIFICATION];
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;
  int n;

  if (!logged(p))
    return -1;

  //Si supera la max cantidad de tweet por refresh va a recibir varios
  sprintf(mq_msg,REFRESH_NOTIFICATION);
  send_mq(mq_msg,INFO);

  n = send_refresh(db_se, from_id, tws);

  if (n == ABORT) {
    send_mq(LOST_CONNECT_DB, ERROR);
    close_thread(p);
  }

  return n;
}

int sv_like(void * p, int id) {
  char mq_msg[MAX_NOTIFICATION];
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;
  int valid;

  if (!logged(p))
    return -1;

  valid = send_like(db_se, id);

  if(valid != NOT_VALID) {
    sprintf(mq_msg,LIKE_NOTIFICATION,id);
    send_mq(mq_msg,INFO);
  }
  else if (valid == ABORT){
    send_mq(LOST_CONNECT_DB, ERROR);
    close_thread(p);
  }

  return valid;
}

int sv_delete(void * p, int id) {
  char mq_msg[MAX_NOTIFICATION];
  char *username = ((t_session_data *) p)->user; // solo puede borrar los tweets de su mismo usuario
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;
  int valid;

  if (!logged(p))
    return -1;

  valid = send_delete(db_se, username, id);
  if (valid != NOT_VALID) {
    sprintf(mq_msg, DELETE_NOTIFICATION, id, username);
    send_mq(mq_msg, INFO);
  }
  else if (valid == ABORT) {
    send_mq(LOST_CONNECT_DB, ERROR);
    close_thread(p);
  }

  return valid;
}

t_tweet sv_show(void * p, int id) {
  int valid;
  char mq_msg[MAX_NOTIFICATION];
  t_tweet tw;
  t_DBsessionADT db_se = ((t_session_data *) p)->db_se;

  sprintf(mq_msg,SHOW_NOTIFICATION,id);
  send_mq(mq_msg,INFO);

  tw.msg[0] = '\0';
  valid = send_show(db_se, id, &tw);

  if (valid == ABORT) {
    send_mq(LOST_CONNECT_DB, ERROR);
    close_thread(p);
  }

  return tw;
}

static int logged(t_session_data * data) {
  return data->user[0] != '\0';
}

static void close_thread(t_session_data * data) {
  end_DBsession(data->db_se);
  unaccept_client(data->sv_se);
  pthread_exit(NULL);
}