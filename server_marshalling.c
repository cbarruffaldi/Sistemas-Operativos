//server_marshalling.c

#include "IPC.h"
#include "marshalling.h"
#include <stdio.h>

typedef struct {
    const char * name;  /* Nombre del comando */
    void (*function) (const char *str);  /* Funcion correspondiente al comando */
} command;

typedef struct client_req {
    t_requestADT req;
    char * msg;
} client_req;

//cantidad de id's para los tweets
int id;

static command commands[]= {{OPCODE_TWEET, tweet},
                            {OPCODE_LIKE, like},
                            {OPCODE_REFRESH, refresh}
                            };


int execute(const char *args, t_requestADT req) {
    int i;
    for (i = 0; i < CMDS_SIZE; i++) {
        if (strcmp(args[0], commands[i].name) == 0)
            return (* commands[i].function) (args+1), req;
    }
    return UNSUPPORTED;
}

int receive() {
    
    client_req clireq = malloc(sizeof(client_req));

    clireq->req = read_request(con) //falta crear la conexion, ¿donde va eso?

    get_request_msg(req, clireq->msg);

    return execute(clireq->msg, clireq->req); //falta handle de error

}

void tweet( char * msg , t_requestADT req) {
    char * usr = malloc(BUFSIZE);
    char * tw_msg = malloc(BUFSIZE);
    tweet tw;
    char * posted_tweet ;

    sscanf(msg, "%s::%s", usr, tw_msg);

    tw = create_tweet(usr, tw_msg);

    //base de datos: Guardar el tweet

    //Devuelve? El tweet posteado?

    posted_tweet = deploy_tweet(tw);
    respond(posted_tweet, req);

}

void like(char * msg, t_requestADT req) {
    int id = atoi(msg);

    //base de datos : +1 a likes del tweet por id

    //Devuelve? El tweet likeado?
    respond(??????,req);
}

void refresh(char * msg, t_requestADT req) {
    int num = atoi(msg);

    //base de datos: retorna los tweets.

    //Devuelve? Los tweets como texto?
    respond(?????,req);

}

tweet create_tweet(char * usr, char * msg) {
    tweet tw = calloc(sizeof(struct tweet));
    strcpy(tw->usr, usr);
    strcpy(tw-> msg,msg);
    tw->id = id++;

    return tw;
}

void respond(char * msg, t_requestADT req) {
    t_responseADT res = create_response();
    set_response_msg(res,msg);
    send_response(req,res);
    free_response(res);

}

/* Pasa un tweet a un String como id+user+message+likes */
char * deploy_tweet (tweet tw) {
    char * buffer;
    sprintf(buffer, "%d%s%s%d", tw->id, SEPARATOR, tw->usr, SEPARATOR, tw->msg, SEPARATOR, tw->likes);
    return buffer;

}

