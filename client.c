#include "fifo_data.h"

#define LEAVE "leave"  // Si se envia LEAVE se cierra el cliente

t_requestADT create_request();
void set_request_msg(t_requestADT req, char *msg);
t_response send_request(int fd, t_requestADT req);
t_response read_response(char * fifo);
void free_request(t_requestADT req);

int main()
{
  int server_fd;
  char buffer[BUFSIZE] = "\0"; // Buffer de input de stdin
  t_response res;
  t_requestADT req = create_request();

  server_fd = open(SERVER_FIFO_PATH, O_WRONLY);

  while (1) {
    fgets(buffer, BUFSIZE, stdin);    // lee input de entrada estandar
    buffer[strlen(buffer)-1] = '\0';  // borra el '\n' final

    if (strcmp(buffer, LEAVE) == 0) { // se recibió LEAVE --> nos vamos
      close(server_fd);
      free_request(req);
      return 0;
    }

    set_request_msg(req, buffer);     // settea el input en el request
    printf("Sendig request...\n");
    res = send_request(server_fd, req);    // envía request y recibe respuesta
    printf("Request sent by client and received response:\n%s\n", res.msg);
  }

  return 1;
}

// Crea nuevo request
t_requestADT create_request() {
  t_requestADT req = malloc(sizeof(t_request));

  sprintf(req->res_fifo, CLIENT_FIFO_PATH, getpid());  // Crea nombre unico para fifo del cliente
  
  mkfifo(req->res_fifo, 0666);  // Crea fifo
  return req;
}

void free_request(t_requestADT req) {
  unlink(req->res_fifo); // borra fifo
  free(req);
}

void set_request_msg(t_requestADT req, char *msg) {
  strcpy(req->msg, msg);
}

// Envia un request y devuelve su respuesta.
t_response send_request(int fd, t_requestADT req) {
  write(fd, req, sizeof(t_request));
  return read_response(req->res_fifo);
}

// Devuelve la respuesta a un request. 
// Solo debe ser llamado por send_request.
t_response read_response(char * fifo) {
  t_response res;
  int fd = open(fifo, O_RDONLY);
  read(fd, &res, sizeof(t_response));
  close(fd);
  return res;
}
