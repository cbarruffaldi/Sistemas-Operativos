#include "sockets_data.h"

void send_request(int sockfd);
void read_response(int sockfd);

/* Los argumentos son el host (localhost)--> 127.0.0.1*/
int main(int argc, char *argv[]) {
   int sockfd, port_num;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   
   if (argc < 2) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
	
   port_num = PORT_NUMBER;
   
   /* Crea el socket */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      printf("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(argv[1]);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(port_num);
   
   /* Conecta al servidor */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("ERROR connecting");
      exit(1);
   }
 
 	send_request(sockfd);
  
  	read_response(sockfd);
   
   return 0;
}

void send_request(int sockfd) {
	char buffer[256];
	int n;
	printf("Please enter the message: ");

   	bzero(buffer,256);
    fgets(buffer,255,stdin);
   
  	/*Envia el mensaje */
   	n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) {
      printf("ERROR writing to socket");
      exit(1);
   }
   printf("Message Sent\n");
}

void read_response(int sockfd){
	char buffer[256];
	int n;
    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
   
    if (n < 0) {
      printf("ERROR reading from socket");
      exit(1);
   }
   printf("Got a Response:  ");
   printf("%s\n",buffer);
}