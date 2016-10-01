
#include "include/IPC.h"
#include "include/client_marshalling.h"
#include "include/marshalling.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

struct session {
  t_connectionADT con;
  t_requestADT req;
};

static int send_op(sessionADT se, char * op_bytes, char res_bytes[BUFSIZE]);

static int process_tweets(char res[], t_tweet * tws);

sessionADT start_session(char * path) {
  sessionADT se = malloc(sizeof(struct session));
  t_addressADT sv_addr = create_address(path);
  se->req = create_request();
  se->con = connect_peer(sv_addr);
  free_address(sv_addr);
  return se->con == NULL ? NULL : se;
}

void end_session(sessionADT se) {
  if (se != NULL) {
    disconnect(se->con);
    free_request(se->req);
    free(se);
  }
}

int send_tweet(sessionADT se, const char * msg) {
  char req_bytes[BUFSIZE], res[BUFSIZE];

  sprintf(req_bytes, "%s%s%s", OPCODE_TWEET, SEPARATOR, msg);

  if (send_op(se, req_bytes, res) == 0) {
    return -1;
  }

  return atoi(res);
}

int send_like(sessionADT se, int tweet_id) {
  char req_bytes[BUFSIZE], res[BUFSIZE];

  sprintf(req_bytes, "%s%s%d", OPCODE_LIKE, SEPARATOR, tweet_id);

  if (send_op(se, req_bytes, res) == 0) {
    return -1;
  }

  return atoi(res);
}

t_tweet * send_refresh(sessionADT se, int *size) {
  char req_bytes[SHORTBUF], res[BUFSIZE]; // Debe ser BUFSIZE porque agarra muchos tweets
  int count, from_id;
  t_tweet * tws = malloc(MAX_TW_REFRESH);

  count = from_id = *size = 0;

  do {
    tws = realloc(tws, (MAX_TW_REFRESH + *size) * sizeof(*tws));
    sprintf(req_bytes, "%s%s%d%s", OPCODE_REFRESH, SEPARATOR, from_id, SEPARATOR);

    if (send_op(se, req_bytes, res) == 0)
      return NULL;

    count = process_tweets(res, tws + *size);
    *size += count;
    from_id += MAX_TW_REFRESH;
  } while (count == MAX_TW_REFRESH);

  return tws;
}

int send_login (sessionADT se, const char *username) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  sprintf(req_bytes, "%s%s%s", OPCODE_LOGIN, SEPARATOR, username);

  if (send_op(se, req_bytes, res) == 0) {
    return -1;
  }

  return 1;
}

int send_logout (sessionADT se) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  sprintf(req_bytes, "%s%s", OPCODE_LOGOUT, SEPARATOR);

  if (send_op(se, req_bytes, res) == 0) {
    return -1;
  }

  return 1;
}

t_tweet send_show (sessionADT se, int tweet_id) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  t_tweet tw;

  tw.id = -1; // sirve como flag de error para el cliente

  sprintf(req_bytes, "%s%s%d", OPCODE_SHOW, SEPARATOR, tweet_id);

  if (send_op(se, req_bytes, res) == 0) {
    return tw;
  }

  process_tweets(res, &tw);

  return tw;
}

static int process_tweets(char res[], t_tweet * tws) {
  char str[BUFSIZE]; // para proteger a res de las modificaciones que le hace strtok()
  int i = 0;
  strcpy(str, res);

  char * token = strtok(str, SEPARATOR);
  while (token != NULL) {

    // Saca los atributos del tweet.
    tws[i].id = atoi(token);
    strcpy(tws[i].user, strtok(NULL, SEPARATOR));
    strcpy(tws[i].msg, strtok(NULL, SEPARATOR));
    tws[i].likes = atoi(strtok(NULL, SEPARATOR));

    token = strtok(NULL, SEPARATOR);
    i++;
  }

  return i;
}

static int send_op(sessionADT se, char * op_bytes, char res_bytes[BUFSIZE]) {
  set_request_msg(se->req, op_bytes);
  t_responseADT res = send_request(se->con, se->req);
  if (res != NULL)
    get_response_msg(res, res_bytes);
  return res != NULL;
}
