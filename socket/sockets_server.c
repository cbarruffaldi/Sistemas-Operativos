#include "sockets_data.h"

void read_request(int sock);
void send_response(int sock);

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd;  /*File Descriptors */
   int port_num;    /*Numero de Puerto */
   int clilen;    /*Direccion del Cliente */
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n /*Numero de caracteres leidos o escritos*/, pid;
   
   /* Crea el socket y devuelve el file Descriptor*/
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      printf("ERROR opening socket\n");
      exit(1);
   }
   
   /* Inicializar el socket */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   port_num = PORT_NUMBER;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;      /*cualquier direccion*/
   serv_addr.sin_port = htons(port_num);
   
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      printf("ERROR on binding\n");
      exit(1);
   }
   
   /*Espera una conexion, permite que haya hasta 5 conexiones esperando */
   listen(sockfd,MAX_CONN);
   clilen = sizeof(cli_addr);
   
   printf("Server waiting...\n");

   while (1) {

      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);  /*Bloquea hasta que se conecte a un servidor */
		
      if (newsockfd < 0) {
         printf("ERROR on accept\n");
         exit(1);
      }
      
      /* Crea un nuevo proceso */
      pid = fork();
		
      if (pid < 0) {
         printf("ERROR on fork\n");
         exit(1);
      }
      
      if (pid == 0) {
         /* Es el proceso del cliente */
         printf("Forked Successfully\n");
         close(sockfd);
         read_request(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
		
   }
}

void read_request (int sock) {
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   
   if (n < 0) {
      printf("ERROR reading from socket");
      exit(1);
   }
   
   printf("Message Received: %s\n",buffer);

   send_response(sock);
   
}

void send_response(int sock){
   int n;

   n = write(sock,"I got your message",18);
   printf("Response Sent\n");
   if (n < 0) {
      printf("ERROR writing to socket\n");
      exit(1);
   }
}