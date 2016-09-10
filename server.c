#include "fifo_data.h"

char * build_client_fifo(int cid);
void send_response(char * fifo, t_response res);
t_request read_request(char * fifo);

#define IS_CID_SIGNAL(s) (strcmp(s, ID_SIGNAL) == 0)

int main()
{
  int cid = 0;
  char * new_fifo;
  // Cliente manda señal por el server fifo para pedir CID y el server devuelve
  // el nombre del fifo por el cual se va a comunicar con ese determinado cliente.

  t_response res;
  t_request req;

  printf("Server listening\n");

  while (1) {
    req = read_request(SERVER_FIFO_PATH);
    printf("Request read by server\n");
    printf("msg: %d - %s\n", req.msg_len, req.msg);
    printf("fifo: %d - %s\n", req.fifo_len, req.res_fifo);

    if (IS_CID_SIGNAL(req.msg)) { // cliente nuevo pide CID
      new_fifo = build_client_fifo(cid);
      res.msg = new_fifo;
      res.msg_len = CLIENT_FIFO_LEN;

      mkfifo(new_fifo, 0666);
      cid++;
    } else {
      res.msg = "HEY!";
      res.msg_len = 5;
    }
    send_response(req.res_fifo, res);
  }
}

// Manda un int con el largo (bytes) del mensaje y luego el mensaje
void send_response(char * fifo, t_response res) {
  // TODO: Si es muy costoso abrir y cerrar asi lo hacemos de otra forma
  int fd = open(fifo, O_RDWR);
  write(fd, &res.msg_len, sizeof(int));
  write(fd, res.msg, res.msg_len);
  close(fd);
}

t_request read_request(char * fifo) {
  t_request req;
  int fd = open(fifo, O_RDWR);

  // lee largo del mensaje
  read(fd, &req.msg_len, sizeof(int));

  // crea espacio y lee mensaje según largo dado
  req.msg = malloc(req.msg_len);
  read(fd, req.msg, req.msg_len);

  // lee largo del named pipe
  read(fd, &req.fifo_len, sizeof(int));

  // crea espacio y lee path del pipe según largo dado
  req.res_fifo = malloc(req.fifo_len);
  read(fd, req.res_fifo, req.fifo_len);
  close(fd);
  return req;
}

char * build_client_fifo(int cid) {
  char * fifo = malloc(CLIENT_FIFO_LEN);
  sprintf(fifo, "%s%06d", "client_n_", cid);
  return fifo;
}
