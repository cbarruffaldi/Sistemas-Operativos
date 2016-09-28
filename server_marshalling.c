//server_marshalling.c
#include "include/server_marshalling.h"
#include "include/IPC.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
  const char * name;  /* Nombre del comando */
  void (*function) (char* msg, t_requestADT req);  /* Funcion correspondiente al comando */
} command;

struct client_req {
  t_requestADT req;
  char * msg;
};

typedef struct {
  int id;
  t_connectionADT peer_con;
  t_connectionADT db_con;
} pthread_data;

struct addr {
  t_addressADT sv_addr;
  t_addressADT db_addr;
};

int thread_id;

int tw_id;

t_connectionADT connect_database(t_addressADT db_addr);
int start_connection(addr_ADT addresses);
int create_thread(int id, t_connectionADT con, t_connectionADT db_con);
void * attend(void * p);
int execute(char *args, t_requestADT req);
int receive(t_connectionADT con);
void tweet( char * msg , t_requestADT req);
void like(char * msg, t_requestADT req);
void refresh(char * msg, t_requestADT req);
tw create_tweet(char * usr, char * msg);
void respond(char * msg, t_requestADT req);
char * deploy_tweet (tw twe);
addr_ADT init(char* server, char* database);

//
char * fill(char c, int length);
void print_user(char * usr);
void print_msg(char * msg);
void print_line();
void print_tweet(t_tweet tw);


static command commands[]= {{OPCODE_TWEET, tweet},
{OPCODE_LIKE, like},
{OPCODE_REFRESH, refresh}
};



addr_ADT init(char * server, char * database){
  tw_id = 0;

  t_addressADT sv_addr, db_addr;
  addr_ADT a = malloc(sizeof(addr_ADT));

  sv_addr = create_address(server);
  if (sv_addr == NULL) {
    printf("Failed to create server address\n");

  }

  db_addr= create_address(database);
  if (db_addr == NULL) {
    printf("Failed to create database address\n");

  }

  a->sv_addr = sv_addr;
  a->db_addr = db_addr;

  return a;
}


int start_connection(addr_ADT addresses) {
  t_connectionADT con, db_con;
  int rc;

  printf("Opening channel...\n");

  if (listen_peer(addresses->sv_addr) < 0) {
    fprintf(stderr, "Cannot listen\n");
    return 1;
  }

  printf("Server listening\n");

  while (1) {

    printf("Awaiting accept...\n");

    con = accept_peer(addresses->sv_addr);
    db_con = connect_peer(addresses->db_addr);

    if (con == NULL) {
      printf("Accept failed.\n");
      return 0;
    }
    printf("Accepted!\n");

    rc = create_thread(thread_id++, con, db_con);

    if (rc) {
      printf("Failed to create thread\n");
      return 1;
    }
  }
  return 0;
}

int create_thread(int id, t_connectionADT con, t_connectionADT db_con) {
  pthread_t thread;
  pthread_data * thdata = malloc(sizeof(*thdata));
  thdata->id = id;
  thdata->peer_con = con;
  thdata->db_con = db_con;

  return pthread_create(&thread, NULL, attend, thdata);
}


void * attend(void * p) {

  pthread_data *data = (pthread_data *) p;
  char msg[BUFSIZE];
  int id = data->id;
  t_connectionADT con = data->peer_con;
  t_connectionADT db_con = data->db_con;

  free(data);

  //FALTA CONECTAR CON LA BASE DE DATOS

  receive(con);

  pthread_exit(NULL);
}


int execute(char *args, t_requestADT req) {
  int i;
  for (i = 0; i < CMDS_SIZE; i++) {
    if (strcmp(args[0], commands[i].name) == 0)
    (* commands[i].function) (args+1, req);
    return 0;
  }
  return UNSUPPORTED;
}

int receive(t_connectionADT con) {
  clireq_ADT clireq;
  printf("EN RECEIVE\n");

  clireq = malloc(sizeof(struct client_req));

  clireq->req = read_request(con);
  if ((clireq->req ) == NULL)  {
    fprintf(stderr, "Error reading request\n");
    return -1;
  }

  get_request_msg(clireq->req, clireq->msg);

  printf("%s\n", clireq->msg);

  return execute(clireq->msg, clireq->req); //falta handle de error
}

//TODO: Capaz tiene mas sentido que estén en server.c

void tweet(char * msg, t_requestADT req) {
  char usr[USER_SIZE];
  char msg[MSG_SIZE];
  char * res = malloc(BUFSIZE);
  t_tweet tw;

  strcpy(usr, strtok(str, SEPARATOR));
  strcpy(msg, strtok(NULL, SEPARATOR));

  t_tweet = create_tweet(usr, msg);
  //base de datos: Guardar el tweet
  print_tweet(t_tweet);

  //get last id from database
  sprintf(res, "%d", tw_id);

  respond(res, req);
}

void like(char * msg, t_requestADT req) {
  int id = atoi(msg);
  char * res = malloc(BUFSIZE); // donde almaceno lo que vuelve de DB

  //base de datos : +1 a likes del tweet por id
  // get # de likes de la bd con id
  sprintf(res, "%d", 0);

  respond(res, req);
}

void refresh(char * msg, t_requestADT req) {
  int num = atoi(msg);
  char * res = malloc(BUFSIZE);

  //base de datos: retorna los tweets.
  sprintf(res, "%d", 0);

  respond(res, req); //lo que viene de DB lo manda directo por el tubo porque tiene bien los separadores.
}

t_tweet create_tweet(char * usr, char * msg) {
  t_tweet tw = calloc(sizeof(struct t_tweet));
  strcpy(tw.usr, usr);
  strcpy(tw.msg, msg);
  tw.id = tweet_id++; // Pedirle ID a la base de datos
  tw.likes = 0;
  return tw;
}

void respond(char * msg, t_requestADT req) {
  t_responseADT res = create_response();
  set_response_msg(res, msg);
  send_response(req, res);
  free_response(res);
}

/*Pasa un tweet a un String como id+user+message+likes
char * deploy_tweet (tweet twe) {
char * buffer;
sprintf(buffer, "%d%s%s%d", twe->id, SEPARATOR, twe->usr, SEPARATOR, twe->msg, SEPARATOR, twe->likes);
return buffer;

}
*/

void print_tweet(t_tweet tw) {
  print_user(tw.user);
  print_msg(tw.msg);
  printf("| id:%5d | likes:%5d |%s|\n", tw.id, tw.likes, fill(' ', COLUMNS - 27));
  print_line();
}

void print_tweets(t_tweet * tws, int count) {
  print_line();
  for (size_t i = 0; i < count; i++) {
    print_tweet(tws[i]);
  }
}

void print_line() {
  printf(" %s\n", fill('-', COLUMNS - 2));
}

void print_user(char * usr) {
  printf("| %s:%s|\n", usr, fill(' ', COLUMNS - strlen(usr)%TW_COLUMNS - 4));
}

void print_msg(char * msg) {
  printf("| %s%s|\n", msg, fill(' ', COLUMNS - strlen(msg)%TW_COLUMNS - 3));
}

char * fill(char c, int length) {
  char * arr = malloc(length + 1);
  int i = 0;
  for (;i < length; i++) {
    arr[i] = c;
  }
  arr[i] = '\0';
  return arr;
}
