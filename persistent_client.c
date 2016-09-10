#include "fifo_data.h"

/** Como client.c pero cada 5 segundos le manda un mensaje al servidor
    a través del fifo que crea en el primer request.
*/

void send_request(char * fifo, t_request req);
t_response read_response(char * fifo);
void talk_to(char * fifo);

int main()
{
  int server_fd, client_fd , cid_fd, cid;
  char * client_fifo;
  // El fifo del cliente lo tiene que dar el servidor para asegurarse que cada
  // cliente tenga un nombre distinto.
  t_request req = { .msg_len = sizeof(ID_SIGNAL), .msg = ID_SIGNAL, \
    .fifo_len = sizeof(CID_FIFO), .res_fifo = CID_FIFO};

  send_request(SERVER_FIFO_PATH, req);
  t_response res = read_response(CID_FIFO);

  client_fifo = res.msg;
  talk_to(client_fifo);
}

void talk_to(char * fifo) {
  printf("talking listening with: %s\n", fifo);
  t_response a;
  int count = 0;
  while(1) {
    char * m = malloc(6);
    sprintf(m, "FP%03d", count);
    t_request r = { .msg_len = 6, .msg = m, .fifo_len = CLIENT_FIFO_LEN, .res_fifo = fifo };
    send_request(SERVER_FIFO_PATH, r);
    a = read_response(fifo);
    printf("Response from server -> persistent: %s / %d\n", a.msg, a.msg_len);

    sleep(5);
    count++;
  }
}

void send_request(char * fifo, t_request req) {
  int fd = open(fifo, O_RDWR);
  write(fd, &req.msg_len, sizeof(int));
  write(fd, req.msg, req.msg_len);
  write(fd, &req.fifo_len, sizeof(int));
  write(fd, req.res_fifo, req.fifo_len);
  close(fd);
}

t_response read_response(char * fifo) {
  t_response res;
  int fd = open(fifo, O_RDWR);
  read(fd, &res.msg_len, sizeof(int));
  res.msg = malloc(res.msg_len);
  read(fd, res.msg, res.msg_len);
  close(fd);
  return res;
}
