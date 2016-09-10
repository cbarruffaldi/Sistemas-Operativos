#include "fifo_data.h"


int main()
{
  int fifo_server, fifo_id, cid;
  char * fifo_client = malloc(16);

  fifo_server = open(SERVER_FIFO_PATH, O_WRONLY);
  fifo_id = open(CID_FIFO, O_RDONLY);

  if(fifo_server < 0 || fifo_id < 0) {
    printf("Error opening file");
    exit(1);
  }

  int signal = ID_SIGNAL;

  write(fifo_server, &signal, sizeof(int)); // manda señal para CID

  read(fifo_id, fifo_client, CLIENT_FIFO_LEN); // lee CID

  printf("fifo: %s\n", fifo_client);

  // abrir fifo con el path que le dio el server

  close(fifo_id);
  close(fifo_server);
}
