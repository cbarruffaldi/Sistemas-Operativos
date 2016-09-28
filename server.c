#include "include/server_marshalling.h"
#include "include/IPC.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define DATABASE_PROCESS "database.bin"

//argv[1] = nombre del server,
//argv[2] = nombre de la base de datos

int main(int argc, char *argv[])
{
   addr_ADT addresses;
   pid_t pid;
   char * str;

  //sprintf(str,"%s %s",argv[1], argv[2]);
  pid = fork();

  if (pid == 0) {

      //char *args [] ={argv[2], argv[1]};
      execl(DATABASE_PROCESS, DATABASE_PROCESS, argv[2], NULL);
      printf("FORK\n");
  }

//  addresses = init(argv[1],argv[2]);

//  start_connection(addresses);

}
