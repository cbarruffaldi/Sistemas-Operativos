//server_marshalling.c

#include "IPC.h"
#include "marshalling.h"
#include <stdio.h>

void process(char * msg) {
  char op = msg[0];
  msg += 1;

  switch (op) {
    case OPCODE_TWEET:
    char * usr = malloc(BUFSIZE), * tw_msg = malloc(BUFSIZE);
    sscanf(msg, "%s::%s", usr, tw_msg);
    // tweet(usr, tw_msg);
    break;

    case OPCODE_LIKE:
    int id = atoi(msg);
    //like(id);
    break;

    case OPCODE_REFRESH:
    int num = atoi(msg);
    //refresh(num);
    break;

    default:
    fprintf(stderr, "Operation not supported\n");
    break;
  }
}
