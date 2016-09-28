#include "IPC.h"
#include "client_marshalling.h"

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

char * send_op(sessionADT se, char * op_bytes); // static?
int atoi(char * arr);
char * next_value(char * a);
char * n_next_value(char * a, int steps);

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

int send_tweet(sessionADT se, char * user, char * msg) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_TWEET;
  sprintf(req_bytes + 1, "%s%s%s", user, SEPARATOR, msg);

  res = send_op(se, req_bytes);

  return atoi(res);
}

int send_like(sessionADT se, int tweet_id) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_LIKE;
  sprintf(req_bytes + 1, "%d", tweet_id);

  res = send_op(se, req_bytes);

  return atoi(res);
}

t_tweet * send_refresh(int tw_count) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_REFRESH;

  res = send_op(se, req_bytes);

  return process_tweets(res);
}

t_tweet * process_tweets(char * res) {
  t_tweet * tweets = malloc(BUFSIZE);
  t_tweet aux;
  char str[BUFSIZE];
  memcpy(str, res, BUFSIZE);
  int i = 0;

  char * token = strtok(str, SEPARATOR);
  while (token != NULL) {
    aux = tweets[i];
    strcpy(aux.user, token);
    strcpy(aux.msg, strtok(NULL, SEPARATOR));
    aux.id = atoi(strtok(NULL, SEPARATOR));
    aux.likes = atoi(strtok(NULL, SEPARATOR));

    printf("User: %s\n", aux.user);
    printf("Msg: %s\n", aux.msg);
    printf("Id: %d\n", aux.id);
    printf("Likes: %d\n", aux.likes);

    token = strtok(NULL, SEPARATOR);
    i++;
  }
}

char * send_op(sessionADT se, char * op_bytes) {
  char * res_bytes = malloc(BUFSIZE);

  set_request_msg(se->req, op_bytes);

  t_responseADT res = send_request(se->con, se->req);
  get_request_msg(res, res_bytes);

  return res_bytes;
}
