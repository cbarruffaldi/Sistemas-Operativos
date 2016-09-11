#include "fifo_data.h"

#define SHUTDOWN "shutdown"

void get_request_msg(t_requestADT req, char *buffer);
void send_response(t_requestADT, t_response res);
t_requestADT read_request(int fd);

int main() 
{
  t_response res = {.msg = "HEY!"};
  t_requestADT req;

  char msg[BUFSIZE];

  printf("Opening pipe...\n");
  int fd = open(SERVER_FIFO_PATH, O_RDWR);

  if (fd < 0) {
    printf("Could not open pipe\n");
    return 1;
  }

  printf("Server listening\n");

  while (1) {
    printf("Reading request...\n");
    req = read_request(fd);
    get_request_msg(req, msg);      // Copia el mensaje de req en msg
    printf("Request read by server\n");
    printf("msg: %s\n", msg);
    printf("fifo: %s\n", req->res_fifo);  // TODO: ver si hacer getter para el res_fifo

    send_response(req, res);    // Responde a cliente

    if (strcmp(SHUTDOWN, msg) == 0) {
      printf("Shutting down...\n");
      close(fd);
      unlink(SERVER_FIFO_PATH);
      return 0;
    }
  }
}

// Getter de msg. Copia el mensaje en buffer.
void get_request_msg(t_requestADT req, char *buffer) {
  strcpy(buffer, req->msg);
}

// Responde al cliente que envió req
void send_response(t_requestADT req, t_response res) {
  // TODO: Si es muy costoso abrir y cerrar asi lo hacemos de otra forma
  int fd = open(req->res_fifo, O_WRONLY);
  write(fd, &res, sizeof(res));
  close(fd);
  free(req);
}

// Lee request. Se bloquea hasta que se envíe alguno.
t_requestADT read_request(int fd) {
  t_requestADT req = malloc(sizeof(t_request));
  read(fd, req, sizeof(t_request));
  return req;
}