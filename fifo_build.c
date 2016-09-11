#include "fifo_data.h"

int main()
{
  int server_file = mkfifo(SERVER_FIFO_PATH, 0666);

  if (server_file < 0) {
    printf("Unable to create a fifo\n");
  } else {
    printf("Fifos created successfully\n");
  }
}