//database_marshalling.c

void * attend(void * p) {
  char req_str[BUFSIZE];
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

    printf("[BD M]: Reading request\n");
    req = read_request(con);

    if (req == NULL) {
      printf("[BD M]: server disconnected\n");
      free_response(res);
      unaccept(con);
      pthread_exit(NULL);
    }

    get_request_msg(req, req_str);

    printf("[BD M]: Received %s\n", req_str);

    /* Comienzo de zona crítica */
    pthread_mutex_lock(mutex);
    execute(req_str, res, param?);
    // sqlite3_exec(db, sql, callback, &param, &errmsg);
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
