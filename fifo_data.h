#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_PATH "fifo_server"
#define CID_FIFO "fifo_cid"

#define FIXED_LEN 20
#define ID_SIGNAL 421 // Porque si
#define CLIENT_FIFO_LEN 16
