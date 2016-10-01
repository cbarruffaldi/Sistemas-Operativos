#include "include/IPC.h"
#include "include/database.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#define UNUSED(x) (void)(x)

#define TABLE_CREATE "CREATE TABLE IF NOT EXISTS "TABLE_TWEET" ( \
                      "ATR_TWEET_ID" INT PRIMARY KEY, \
                      "ATR_TWEET_USER" VARCHAR(%d) NOT NULL, \
                      "ATR_TWEET_MSG" VARCHAR(%d) NOT NULL, \
                      "ATR_TWEET_LIKES" INT DEFAULT 0 NOT NULL);"

#define ARG_COUNT 2

typedef struct {
  t_connectionADT con;
  pthread_mutex_t *mutex;
  sqlite3* db;
} pthread_data;

typedef struct {
  int n;
  int rows;
  char values[BUFSIZE]; // las columnas se separan por VALUE_SEPARATOR y las filas por ROW_SEPARATOR
} query_rows;

int create_thread(t_connectionADT con, pthread_mutex_t *mutex, sqlite3* db);
int callback (void *params, int argc, char *argv[], char *azColName[]);
void * attend(void * p);
int setup_db(sqlite3* db);
void concat_value(query_rows * q, char *value);

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
    printf("[BD]: Could not open db\n%s", sqlite3_errmsg(db));
    return 1;
  }

  if (setup_db(db) < 0) {
    printf("[BD]: Failed to setup database\n");
    sqlite3_close(db);
    return 1;
  }

  mutex = malloc(sizeof(*mutex));
  pthread_mutex_init(mutex, NULL);

  db_addr = create_address(argv[1]);

  if (listen_peer(db_addr) < 0) {
    fprintf(stderr, "[BD]: Cannot listen\n");
    return 1;
  }

  while (1) {
    printf("[BD]: Awaiting accept...\n");

    con = accept_peer(db_addr);

    if (con == NULL) {
      printf("[BD]: Accept failed.\n");
      return 0;
    }

    printf("[BD]: Accepted!\n");

    rc = create_thread(con, mutex, db);

    if (rc) {
      printf("[BD]: Failed to create thread\n");
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
    printf("[BD]: error: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

//TODO: Repite código con el create_thread de server.c

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
  query_rows param;

  t_responseADT res = create_response();

  pthread_data *data = (pthread_data *) p;
  sqlite3* db = data->db;
  t_connectionADT con = data->con;
  pthread_mutex_t *mutex = data->mutex;

  free(p);

  while (1) {
    param.n = param.rows = 0;

    printf("[BD]: Reading request\n");
    req = read_request(con);

    if (req == NULL) {
      printf("[BD]: server disconnected\n");
      free_response(res);
      unaccept(con);
      pthread_exit(NULL);
    }

    get_request_msg(req, sql);

    printf("[BD]: Received %s\n", sql);

    /* Comienzo de zona crítica */
    pthread_mutex_lock(mutex);
    sqlite3_exec(db, sql, callback, &param, &errmsg);
    pthread_mutex_unlock(mutex);
    /* Fin zona crítica */

    if (errmsg != NULL)
      printf("[BD]: exec error: %s\n", errmsg);

    param.values[param.n - (param.n > 0)] = '\0';
    printf("LENGTH OF DB RESPONSE: %lu\n", strlen(param.values));
    printf("PARAM VALUES: %s\n", param.values);

    if (param.n > 0) {
      printf("[BD]: Hay %d fila%c\n", param.rows, param.rows == 1 ? '\0' : 's');
      printf("[BD]: %s\n", param.values);
    }

    set_response_msg(res, param.values);
    printf("[BD]: Sending response %s\n", param.values);

    if (send_response(req, res) < 0) {
      printf("[BD]: Failed to send response\n");
      pthread_exit(NULL);
    }
  }

  pthread_exit(NULL);
}

int callback (void *p, int argc, char *argv[], char *NotUsed[]) {
  int i;
  query_rows *param = (query_rows *) p;

  UNUSED(NotUsed); // Así no tira warning por parámetro no usado

  for (i = 0; i < argc; i++)
    concat_value(param, argv[i]);
  param->rows = param->rows+1;
  param->values[param->n-1] = ROW_SEPARATOR;

  return 0;
}

void concat_value(query_rows * q, char *value) {
  int i, j;
  for (i = 0, j = q->n; value[i] != '\0'; i++, j++)
    q->values[j] = value[i];
  q->values[j++] = VALUE_SEPARATOR;
  q->n = j;
}
