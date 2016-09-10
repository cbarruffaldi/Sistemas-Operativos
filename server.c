// server.c

#include "fifo_data.h"

char * build_client_fifo(int id);

int main()
{
  int cid = 0, sig;
  int server_file, client_id_file;

  // cliente manda señal para pedir CID por server fifo y el server devuelve
  // un cid por el fifo client_id
  server_file = open(SERVER_FIFO_PATH, O_RDONLY);
  client_id_file = open(CID_FIFO, O_RDWR);

  if (server_file < 1 || client_id_file < 1) {
    printf("Error opening file");
    exit(1);
  }

  printf("Server listening\n");

  while (1) {
    read(server_file, &sig, sizeof(int));
    printf("sig number: %d\n", sig);

    if (sig == ID_SIGNAL) {
      // crear FIFO con el nombre que se pasa, para que lo pueda usar el client
      write(client_id_file, build_client_fifo(cid), CLIENT_FIFO_LEN);
      cid++;
    }
    sleep(1);
  }

  close(client_id_file);
  close(server_file);
}


char * build_client_fifo(int id) {
  char * fifo = malloc(CLIENT_FIFO_LEN);
  sprintf(fifo, "%s%06d", "client_n_", id);
  return fifo;
}
