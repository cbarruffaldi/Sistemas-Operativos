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
int get_ans_int(char * arr);

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

  return get_ans_int(res);
}

int send_like(sessionADT se, int tweet_id) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_LIKE;
  sprintf(req_bytes + 1, "%d", tweet_id);

  res = send_op(se, req_bytes);

  return get_ans_int(res);
}

int get_ans_int(char * arr) {
  return arr[0]; // TODO: Ver como mandamos los ints en la respuesta. Un byte no alcanza.
}

char * send_op(sessionADT se, char * op_bytes) {
  char * res_bytes = malloc(BUFSIZE);

  set_request_msg(se->req, op_bytes);

  t_responseADT res = send_request(se->con, se->req);
  get_request_msg(res, res_bytes);

  return res_bytes;
}

// char ** send_refresh(sessionADT se, int tw_count) {
//
// }
