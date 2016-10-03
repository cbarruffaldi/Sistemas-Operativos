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

static int create_thread(t_connectionADT con, pthread_mutex_t *mutex, sqlite3* db);
static int callback (void *params, int argc, char *argv[], char *azColName[]);
static void * run_thread(void * p);
static int setup_db(sqlite3* db);
static void concat_value(query_rows * q, char *value);
static void close_thread(t_connectionADT con, t_responseADT res);

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
    return 1;
  }

  if (setup_db(db) < 0) {
    sqlite3_close(db);
    return 1;
  }

  mutex = malloc(sizeof(*mutex));
  pthread_mutex_init(mutex, NULL);

  db_addr = create_address(argv[1]);

  if (db_addr == NULL) {
    return 1;
  }

  if (listen_peer(db_addr) < 0) {
    return 1;
  }

  while (1) {

    con = accept_peer(db_addr);

    if (con == NULL) {
      sqlite3_close(db);
      return 1;
    }


    rc = create_thread(con, mutex, db);

    if (rc) {
      sqlite3_close(db);
      return 1;
    }
  }
}

static int setup_db(sqlite3 *db) {
  char sql[BUFSIZE];
  sprintf(sql, TABLE_CREATE, USER_SIZE, MSG_SIZE);
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);

  if (errmsg != NULL) {
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

static int create_thread(t_connectionADT con, pthread_mutex_t *mutex, sqlite3 *db) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->con = con;
  thdata->mutex = mutex;
  thdata->db = db;
  return pthread_create(&thread, NULL, run_thread, thdata);
}

static void * run_thread(void * p) {
  char sql[BUFSIZE];
  char *errmsg;
  t_requestADT req;
  query_rows param;

  t_responseADT res = create_response();
  pthread_detach(pthread_self()); // Se liberan los recursos del thread al cerrarse

  pthread_data *data = (pthread_data *) p;
  sqlite3* db = data->db;
  t_connectionADT con = data->con;
  pthread_mutex_t *mutex = data->mutex;

  free(p);

  while (1) {
    param.n = param.rows = 0;

    req = read_request(con);

    if (req == NULL) {
      close_thread(con, res);
    }

    get_request_msg(req, sql);

    /* Comienzo de zona crítica */
    pthread_mutex_lock(mutex);
    sqlite3_exec(db, sql, callback, &param, &errmsg);
    pthread_mutex_unlock(mutex);
    /* Fin zona crítica */

    param.values[param.n - (param.n > 0)] = '\0';

    set_response_msg(res, param.values);

    if (send_response(req, res) < 0) {
      close_thread(con, res);
    }
  }

  close_thread(con, res);
  return NULL;
}

static void close_thread(t_connectionADT con, t_responseADT res) {
  unaccept(con);
  free_response(res);
  pthread_exit(NULL);  
}

static int callback (void *p, int argc, char *argv[], char *NotUsed[]) {
  int i;
  query_rows *param = (query_rows *) p;

  UNUSED(NotUsed); // Así no tira warning por parámetro no usado

  for (i = 0; i < argc; i++)
    concat_value(param, argv[i]);
  param->rows = param->rows+1;
  param->values[param->n-1] = ROW_SEPARATOR;

  return 0;
}

static void concat_value(query_rows * q, char *value) {
  int i, j;
  for (i = 0, j = q->n; value[i] != '\0'; i++, j++)
    q->values[j] = value[i];
  q->values[j++] = VALUE_SEPARATOR;
  q->n = j;
}
