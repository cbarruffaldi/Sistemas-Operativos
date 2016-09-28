//server_marshalling.c
#include "server_marshalling.h"
#include "IPC.h"
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

typedef struct {
    int id;
    int likes;
    char msg[140];
    char user[32];
} tw;

int thread_id;
//cantidad de id's para los tweets
int tweet_id;

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

static command commands[]= {{OPCODE_TWEET, tweet},
                            {OPCODE_LIKE, like},
                            {OPCODE_REFRESH, refresh}
                            };



addr_ADT init(char * server, char * database){
    
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

void tweet( char * msg , t_requestADT req) {

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

void like(char * msg, t_requestADT req) {
    int id = atoi(msg);

    //base de datos : +1 a likes del tweet por id

    //Devuelve? El tweet likeado?
    //respond(??????,req);
}

void refresh(char * msg, t_requestADT req) {
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
void respond(char * msg, t_requestADT req) {
    t_responseADT res = create_response();
    set_response_msg(res,msg);
    send_response(req,res);
    free_response(res);

}

/*Pasa un tweet a un String como id+user+message+likes 
char * deploy_tweet (tweet twe) {
    char * buffer;
    sprintf(buffer, "%d%s%s%d", twe->id, SEPARATOR, twe->usr, SEPARATOR, twe->msg, SEPARATOR, twe->likes);
    return buffer;

}
*/

