
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

sessionADT start_session(char * path) {
  sessionADT se = malloc(sizeof(struct session));
  t_addressADT sv_addr = create_address(path);
  t_connectionADT con = connect_peer(sv_addr);
  se->req = create_request();

  free_address(sv_addr);

  if (con == NULL) {
    free(se);
    free_request(se->req);
    se = NULL;
  }
  else
    se->con = con;

  return se;
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
    return ABORT;
  }

  return atoi(res);
}

int send_like(sessionADT se, int tweet_id) {
  char req_bytes[BUFSIZE], res[BUFSIZE];

  sprintf(req_bytes, "%s%s%d", OPCODE_LIKE, SEPARATOR, tweet_id);

  if (send_op(se, req_bytes, res) == 0) {
    return ABORT;
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

    count = str_to_tweets(res, tws + *size);
    *size += count;
    from_id = tws[count-1].id+1;
  } while (count == MAX_TW_REFRESH);

  return tws;
}

int send_login (sessionADT se, const char *username) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  sprintf(req_bytes, "%s%s%s", OPCODE_LOGIN, SEPARATOR, username);

  if (send_op(se, req_bytes, res) == 0) {
    return ABORT;
  }

  return 1;
}

int send_logout (sessionADT se) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  sprintf(req_bytes, "%s%s", OPCODE_LOGOUT, SEPARATOR);

  if (send_op(se, req_bytes, res) == 0) {
    return ABORT;
  }

  return 1;
}

t_tweet send_show (sessionADT se, int tweet_id) {
  char req_bytes[SHORTBUF], res[SHORTBUF];
  t_tweet tw;

  tw.msg[0] = '\0'; // sirve como flag de id inválido para el cliente

  sprintf(req_bytes, "%s%s%d%s", OPCODE_SHOW, SEPARATOR, tweet_id, SEPARATOR);

  if (send_op(se, req_bytes, res) == 0) {
    tw.user[0] = '\0';
    return tw;
  }

  str_to_tweets(res, &tw);

  return tw;
}

int send_delete(sessionADT se, int tweet_id) {
  char req_bytes[SHORTBUF], res[SHORTBUF];

  sprintf(req_bytes, "%s%s%d%s", OPCODE_DELETE, SEPARATOR, tweet_id, SEPARATOR);

  if (send_op(se, req_bytes, res) == 0) {
    return ABORT;
  }

  return atoi(res);
}


static int send_op(sessionADT se, char * op_bytes, char res_bytes[BUFSIZE]) {
  t_responseADT res;
  set_request_msg(se->req, op_bytes);
  res = send_request(se->con, se->req);
  if (res != NULL)
    get_response_msg(res, res_bytes);
  return res != NULL;
}
