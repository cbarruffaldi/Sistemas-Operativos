#include "IPC.h"

#define SERVER_FIFO_PATH "fifo_server"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
  int server_file = mkfifo(SERVER_FIFO_PATH, 0666);

  if (server_file < 0) {
    printf("Unable to create a fifo\n");
  } else {
    printf("Fifos created successfully\n");
  }
}
