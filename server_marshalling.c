//server_marshalling.c
#include "server_marshalling.h"
#include "IPC.h"
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
int tweet_id;

int execute(char *args, t_responseADT res);

int receive(t_connectionADT con);
tw create_tweet(char * usr, char * msg);
void respond(char * msg, t_requestADT req);
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
    return 1;
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

int execute(char *command, t_responseADT res) {
    int i;
    for (i = 0; i < CMDS_SIZE; i++) {
        if (command[0] == commands[i].name) {
          (* commands[i].function) (args+1, req);
          return 0;
        }
    }
    return UNSUPPORTED;
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

void tweet( char * msg , t_responseADT res) {

    char * usr = malloc(USER_SIZE);
    char * tw_msg = malloc(MSG_SIZE);
    //tweet tw;
    char * posted_tweet ;

    sscanf(msg, "%s::%s", usr, tw_msg);

    //tw = create_tweet(usr, tw_msg);

    //base de datos: Guardar el tweet

    //Devuelve? El tweet posteado?

    //posted_tweet = deploy_tweet(tw);
    //respond(posted_tweet, req);

}

void like(char * msg, t_responseADT res) {
    int id = atoi(msg);

    //base de datos : +1 a likes del tweet por id

    //Devuelve? El tweet likeado?
    //respond(??????,req);
}

void refresh(char * msg, t_responseADT res) {
    int num = atoi(msg);

    //base de datos: retorna los tweets.

    //Devuelve? Los tweets como texto?
    //respond(?????,req);

}

/*tweet create_tweet(char * usr, char * msg) {
    tweet tw = calloc(sizeof(struct tweet));
    strcpy(tw->usr, usr);
    strcpy(tw-> msg,msg);
    tw->id = tweet_id++;

    return tw;
}
*/

/*Pasa un tweet a un String como id+user+message+likes 
char * deploy_tweet (tweet twe) {
    char * buffer;
    sprintf(buffer, "%d%s%s%d", twe->id, SEPARATOR, twe->usr, SEPARATOR, twe->msg, SEPARATOR, twe->likes);
    return buffer;

}
*/

