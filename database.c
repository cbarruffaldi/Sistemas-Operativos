#include "include/IPC.h"
#include "include/database.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#define UNUSED(x) (void)(x)

// TODO: Cambiar twit por tweet. No lo quiero hacer ahora para no romper
#define TABLE_CREATE "CREATE TABLE IF NOT EXISTS "TABLE_TWIT" ( \
                      "ATR_TWIT_ID" INT PRIMARY KEY, \
                      "ATR_TWIT_USER" VARCHAR(%d) NOT NULL, \
                      "ATR_TWIT_MSG" VARCHAR(%d) NOT NULL, \
                      "ATR_TWIT_LIKES" INT DEFAULT 0 NOT NULL);"

#define ARG_COUNT 2

#define SEPARATOR ":|:" // TODO: podría sacarlo de marshalling.h pero se supone que es independiente

typedef struct {
  t_connectionADT con;
  pthread_mutex_t *mutex;
  sqlite3* db;
} pthread_data;

typedef struct {
  int pointer;
  char values[BUFSIZE];
} query_data;

int create_thread(t_connectionADT con, pthread_mutex_t *mutex, sqlite3* db);
int callback (void *params, int argc, char *argv[], char *azColName[]);
void * attend(void * p);
int setup_db(sqlite3* db);
void concat_value(query_data * qd, char *value);

int main(int argc, char *argv[]) {
  sqlite3 *db;
  t_connectionADT con;
  t_addressADT db_addr;
  pthread_mutex_t *mutex;
  int rc;

  if(argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>\n", argv[0]);
    return 1;
  }

  if (sqlite3_open(DATABASE_NAME, &db)) {
    printf("Could not open db\n%s", sqlite3_errmsg(db));
    return 1;
  }

  if (setup_db(db) < 0) {
    printf("Failed to setup database\n");
    sqlite3_close(db);
    return 1;
  }

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

    rc = create_thread(con, mutex, db);

    if (rc) {
      printf("Failed to create thread\n");
      return 1;
    }
  }
}

int setup_db(sqlite3 *db) {
  char sql[BUFSIZE];
  sprintf(sql, TABLE_CREATE, USER_SIZE, MSG_SIZE);
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg != NULL) {
    printf("error: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

int create_thread(t_connectionADT con, pthread_mutex_t *mutex, sqlite3 *db) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->con = con;
  thdata->mutex = mutex;
  thdata->db = db;
  return pthread_create(&thread, NULL, attend, thdata);
}

void * attend(void * p) {
  char sql[BUFSIZE];
  char *errmsg;
  t_requestADT req;
  // query_rows param;
  query_data query;

  t_responseADT res = create_response();

  pthread_data *data = (pthread_data *) p;
  sqlite3* db = data->db;
  t_connectionADT con = data->con;
  pthread_mutex_t *mutex = data->mutex;

  free(p);

  while (1) {
    query.pointer = 0;
    printf("Reading request\n");
    req = read_request(con);

    if (req == NULL) {
      printf("Failed to read request\n");
      pthread_exit(NULL);
    }

    get_request_msg(req, sql);

    printf("Received %s\n", sql);

    /* Comienzo de zona crítica */
    pthread_mutex_lock(mutex);
    sqlite3_exec(db, sql, callback, &query, &errmsg);
    pthread_mutex_unlock(mutex);
    /* Fin zona crítica */

    if (errmsg != NULL)
      printf("exec error: %s\n", errmsg);

    query.values[pointer - 1] = '\0';

    set_response_msg(res, query.values);
    printf("Sending response %s\n", query.values);

    if (send_response(req, res) < 0) {
      printf("Failed to send response\n");
      pthread_exit(NULL);
    }
  }

  pthread_exit(NULL);
}

int callback (void *p, int argc, char *argv[], char *NotUsed[]) {
  int i;
  query_data * qd = (query_data *) p;

  UNUSED(NotUsed); // Así no tira warning por parámetro no usado

  for (i = 0; i < argc; i++)
    concat_value(qd, argv[i]);

  return 0;
}

void concat_value(query_data * qd, char *value) {
  int i, sep_len = strlen(SEPARATOR);

  for (i = 0, j = qd->pointer; value[i] != '\0'; i++, j++)
    qd->values[j] = value[i];

  strcpy(qd->values + j, SEPARATOR);
  qd->pointer = j + sep_len;
}
