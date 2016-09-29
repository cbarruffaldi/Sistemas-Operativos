//server_marshalling.c
#include "include/server_marshalling.h"
#include "include/IPC.h"
#include "include/marshalling.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


/*
** Cada función decodifica msg, llama a la función del servidor y settea en
** el response la respuesta. NO LA ENVÍA. attend se encarga de enviarla.
*/
typedef struct {
    const char * name;  /* Nombre del comando */
    void (*function) (char* msg, t_responseADT res);  /* Funcion correspondiente al comando */
} command;

typedef struct {
    int id;
    int likes;
    char msg[140];
    char user[32];
} tw;

//cantidad de id's para los tweets
int tw_id;

int execute(char *args, t_responseADT res);

t_tweet create_tweet(char * usr, char * msg);
char * deploy_tweet (tw twe);

void tweet( char * msg , t_responseADT res);
void like(char * msg, t_responseADT res);
void refresh(char * msg, t_responseADT res);

static command commands[]= {{OPCODE_TWEET, tweet},
{OPCODE_LIKE, like},
{OPCODE_REFRESH, refresh}
};


struct t_session {
  t_connectionADT con;
  t_addressADT addr;
};

struct t_master_session {
  t_addressADT addr;
};

t_master_sessionADT setup_master_session(char *sv_path) {

  t_addressADT addr = create_address(sv_path);
  t_master_sessionADT se;

  printf("Opening channel...\n");

  printf("Server listening\n");

  if (listen_peer(addr) < 0) {
    fprintf(stderr, "Cannot listen\n");
    return NULL;
  }

  se = malloc(sizeof(struct t_session));
  se->addr = addr;
  return se;
}

t_sessionADT accept_client(t_master_sessionADT master_session) {
  t_connectionADT con = accept_peer(master_session->addr);
  t_sessionADT se;

  if (con == NULL) {
    printf("Accept failed.\n");
    return NULL;
  }

  printf("Accepted!\n");

  se = malloc(sizeof(struct t_session));

  se->con = con;
  se->addr = master_session->addr;

  return se;
}

int attend(t_sessionADT se) {
  char buffer[BUFSIZE];
  int valid;
  t_connectionADT con = se->con;
  t_responseADT res = create_response();
  t_requestADT req;

  while(1) {
    req = read_request(con);
    get_request_msg(req, buffer);
    valid = execute(buffer, res);
    if (valid == UNSUPPORTED)
      set_response_msg(res, UNSUPPORTED_MSG);
    send_response(req, res);
  }
}

int execute(char *request, t_responseADT res) {
    int i;
    for (i = 0; i < CMDS_SIZE; i++) {
        if (request[0] == commands[i].name[0]) {
          (* commands[i].function) (request+1, res);
          return 0;
        }
    }
    return UNSUPPORTED;
}

void tweet(char * str, t_responseADT res) {
  char usr[USER_SIZE];
  char msg[MSG_SIZE];
  char * response = malloc(BUFSIZE);
  t_tweet tw;

  strcpy(usr, strtok(str, SEPARATOR));
  strcpy(msg, strtok(NULL, SEPARATOR));

  tw = create_tweet(usr, msg);
  //base de datos: Guardar el tweet
  print_tweet(tw);

  //get last id from database
  sprintf(response, "%d", tw_id);

  set_response_msg(res, response);
}

void like(char * msg, t_responseADT res) {
  int id = atoi(msg);
  char * str = malloc(BUFSIZE); // donde almaceno lo que vuelve de DB

  //base de datos : +1 a likes del tweet por id
  // get # de likes de la bd con id
  sprintf(str, "%d", 0);
  set_response_msg(res, str);
}

void refresh(char * msg, t_responseADT res) {
  int num = atoi(msg);
  char * str = malloc(BUFSIZE);

  //base de datos: retorna los tweets.
  sprintf(str, "%d", 0);

  set_response_msg(res, str); //lo que viene de DB lo manda directo por el tubo porque tiene bien los separadores.
}

t_tweet create_tweet(char * usr, char * msg) {
  t_tweet tw;
  strcpy(tw.user, usr);
  strcpy(tw.msg, msg);
  tw.id = tw_id++; // Pedirle ID a la base de datos
  tw.likes = 0;
  return tw;
}
