//marshalling.c
#include "include/marshalling.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int tweet_to_str(char * str, t_tweet tw);

int str_to_tweets (char * str, t_tweet * tws) {
  char aux[BUFSIZE]; // para proteger a res de las modificaciones que le hace strtok()
  int i = 0;
  strcpy(aux, str);

  char * token = strtok(aux, SEPARATOR);
  while (token != NULL) {

    // Saca los atributos del tweet.
    tws[i].id = atoi(token);
    strcpy(tws[i].user, strtok(NULL, SEPARATOR));
    strcpy(tws[i].msg, strtok(NULL, SEPARATOR));
    tws[i].likes = atoi(strtok(NULL, SEPARATOR));

    token = strtok(NULL, SEPARATOR);
    i++;
  }

  return i;
}

static int tweet_to_str(char * str, t_tweet tw) {
  sprintf(str, "%d%s%s%s%s%s%d%s",tw.id, SEPARATOR, tw.user, SEPARATOR, tw.msg, SEPARATOR, tw.likes, SEPARATOR);
  return strlen(str);
}


void tweets_to_str (char * str, t_tweet * tws, int size) {
  int i;
  int cursor = 0;
  for (i = 0; i < size; i++) {
    cursor = tweet_to_str(str + cursor, tws[i]);
  }
}
