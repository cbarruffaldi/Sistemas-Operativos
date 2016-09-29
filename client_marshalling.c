
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

static char * send_op(sessionADT se, char * op_bytes);

static t_tweet * process_tweets(char * res, int * count);

sessionADT start_session(char * a) {
  sessionADT se = malloc(sizeof(struct session));
  se->req = create_request();
  t_addressADT sv_addr = create_address(a);
  se->con = connect_peer(sv_addr);
  return se;
}

void end_session(sessionADT se) {
  disconnect(se->con);
  free_request(se->req);
  free(se);
}

// TODO: capaz se pueden modularizar

int send_tweet(sessionADT se, char * user, char * msg) {
  char * req_bytes = malloc(BUFSIZE), * res;

  sprintf(req_bytes, "%s%s%s%s%s", OPCODE_TWEET, SEPARATOR, user, SEPARATOR, msg);

  res = send_op(se, req_bytes);

  return atoi(res);
}

int send_like(sessionADT se, int tweet_id) {
  char * req_bytes = malloc(BUFSIZE), * res;

  sprintf(req_bytes, "%s%s%d", OPCODE_LIKE, SEPARATOR, tweet_id);

  res = send_op(se, req_bytes);

  return atoi(res);
}

t_tweet * send_refresh(sessionADT se, int ref_count, int * received_count) {
  char * req_bytes = malloc(BUFSIZE), * res;

  sprintf(req_bytes, "%s%s%d", OPCODE_REFRESH, SEPARATOR, ref_count);

  res = send_op(se, req_bytes);

  return process_tweets(res, received_count);
}

t_tweet * process_tweets(char * res, int * count) {
  t_tweet * tweets = malloc(BUFSIZE);
  char str[BUFSIZE];
  strcpy(str, res);
  int i = 0;

  char * token = strtok(str, SEPARATOR);
  while (token != NULL) {
    tweets[i].id = atoi(token);
    strcpy(tweets[i].user, strtok(NULL, SEPARATOR));
    strcpy(tweets[i].msg, strtok(NULL, SEPARATOR));
    tweets[i].likes = atoi(strtok(NULL, SEPARATOR));

    token = strtok(NULL, SEPARATOR);
    i++;
  }

  *count = i;
  return tweets;
}

char * send_op(sessionADT se, char * op_bytes) {
  char * res_bytes = malloc(BUFSIZE);

  set_request_msg(se->req, op_bytes);

  t_responseADT res = send_request(se->con, se->req);
  get_response_msg(res, res_bytes);

  return res_bytes;
}
