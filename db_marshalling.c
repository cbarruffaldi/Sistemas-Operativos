//database_marshalling.c

#include "include/db_marshalling.h"
#include "include/query.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static int send_query(t_DBsessionADT se, const char *query, char response[]);

struct t_DBsession {
  t_connectionADT con;
  t_requestADT req;
};

t_DBsessionADT start_DBsession(const char * path) {
  t_connectionADT con;
  t_DBsessionADT se = malloc(sizeof(struct t_DBsession));
  t_addressADT sv_addr = create_address(path);

  if (sv_addr == NULL) {
    free(se);
    return NULL;
  }

  con = connect_peer(sv_addr);

  free_address(sv_addr);

  se->req = create_request();
  if (con == NULL) {
    free(se);
    free_request(se->req);
    se = NULL;
  }
  else
    se->con = con;

  return se;
}

void end_DBsession(t_DBsessionADT se) {
  if (se != NULL) {
    disconnect(se->con);
    free_request(se->req);
    free(se);
  }
}

int send_tweet(t_DBsessionADT se, const char *username, const char * msg) {
  char response[SHORTBUF];
  char query[QUERY_SIZE];

  query_insert(query, username, msg);

  if (!send_query(se, query, response))
    return ABORT;

  return atoi(response);
}

int send_like(t_DBsessionADT se, int tweet_id) {
  char response[SHORTBUF];
  char query[QUERY_SIZE];

  query_like(query, tweet_id);
  if (!send_query(se, query, response))
    return ABORT;

  return response[0] == '\0' ? NOT_VALID : atoi(response);
}

int send_delete(t_DBsessionADT se, char * username, int tweet_id) {
  char response[SHORTBUF];
  char query[QUERY_SIZE];

  query_delete(query, username, tweet_id);
  if (!send_query(se, query, response))
    return ABORT;

  return response[0] == '\0' ? NOT_VALID : VALID;
}

int send_refresh(t_DBsessionADT se, int from_id, t_tweet tws[]) {
  char response[SHORTBUF];
  char query[QUERY_SIZE];

  query_refresh(query, from_id);

  if (!send_query(se, query, response))
    return ABORT;

  return str_to_tweets(response, tws);
}

int send_show(t_DBsessionADT se, int tweet_id, t_tweet *tw) {
  char response[SHORTBUF];
  char query[QUERY_SIZE];

  query_show(query, tweet_id);

  if (!send_query(se, query, response))
    return ABORT;
  
  str_to_tweets(response, tw);

  return VALID;
}

static int send_query(t_DBsessionADT se, const char *query, char response[]) {
  t_responseADT res;
  set_request_msg(se->req, query);
  res = send_request(se->con, se->req);
  if (res != NULL)
    get_response_msg(res, response);
  return res != NULL;
}
