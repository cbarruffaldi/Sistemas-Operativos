// client marshaling
#include <stdio.h>
#include "IPC.h"
#include "marshalling.h"

struct session {
  t_connectionADT con;
  t_requestADT req;
  char[USER_SIZE] usr;
};

sessionADT start_session(char * a) {

  sessionADT se = malloc(sizeof(struct session))
  t_addressADT sv_addr = create_address(a);

  t_requestADT = create_request();
  se->con = connect_peer(sv_addr);

  if (con == NULL) {
    printf("failed to connect\n");
    return 1;
  }

  return se;
}

void set_username(char* user){
  se->usr = user;
}

//¿Desconectar o poner el user en null?
void end_session(sessionADT se) {
  disconnect(se->con);
  free(se);

}


// Recibe como respuesta el id asignado al tweet enviado. Si hubo error devuelve -1.
int tweet_send(sessionADT se, char * msg) {
  char * req_bytes = malloc(BUFSIZE),
  t_response res;

  req_bytes[0] = (char) OPCODE_TWEET;
  sprintf(req_bytes + 1, "%s%s%s", user, SEPARATOR, msg);

  res=send_instruction(se,req_bytes);

  //VER RESPUESTA DEL SERVIDOR

}

int like_send(sessionADT se, int tweet_id) {
  char * req_bytes = malloc(BUFSIZE),
  char * res;

  req_bytes[0] = (char) OPCODE_LIKE;
  sprintf(req_bytes + 1, "%d", tweet_id);

  res = send_instruction(se, req_bytes);

  // VER RESPUESTA DEL SERVIDOR
}

char* send_instruction(sessionADT se, char * instruction){
  char * res = malloc(BUFSIZE);
  t_response res;

  set_request_msg(se->req,req_bytes);
  res = send_request(se->con,se->req);

  return res;
}

// char ** refresh_send(t_connectionADT con, int num) {
//
// }
