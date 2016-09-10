#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_PATH "fifo_server"
#define CID_FIFO "fifo_cid"
#define ID_SIGNAL "GIVE_ME_ID"

#define MAX_CLIENTS 32
#define CLIENT_FIFO_LEN 16

typedef struct {
  int msg_len;
  char * msg;
  int fifo_len;
  char * res_fifo;
} t_request;

typedef struct {
  int msg_len;
  char * msg;
} t_response;
