// client marshaling
#include <stdio.h>
#include "IPC.h"
#include "marshalling.h"

// TODO: podría también contener a request para no estar haciendo create_request y free todo el tiempo
struct session {
  t_connectionADT con;
}

sessionADT start_session(char * a) {
  sessionADT se = malloc(sizeof(struct session))
  t_addressADT sv_addr = create_address(a);
  se->con = connect_peer(sv_addr)
  return se;
}

void end_session(sessionADT se) {
  disconnect(se->con);
  free(se);
}

// TODO: ver nombres
// Recibe como respuesta el id asignado al tweet enviado. Si hubo error devuelve -1.
int tweet_send(sessionADT se, char * user, char * msg) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_TWEET;
  sprintf(req_bytes + 1, "%s%s%s", user, SEPARATOR, msg);

  res = send_op(se, req_bytes);
  return res[0];
}

int like_send(sessionADT se, int tweet_id) {
  char * req_bytes = malloc(BUFSIZE), * res;

  req_bytes[0] = (char) OPCODE_LIKE;
  sprintf(req_bytes + 1, "%d", tweet_id);

  res = send_op(se, req_bytes);
  return res[0];
}

char * send_op(sessionADT se, char * op_bytes) {
  char * res_bytes = malloc(BUFSIZE);

  t_requestADT req = create_request();
  req.set_request_msg(op_bytes);

  t_response res = send_request(se->con, req);
  get_request_msg(res_bytes, res);

  free_request(req)
  return res_bytes;
}

// char ** refresh_send(t_connectionADT con, int num) {
//
// }
