#include "IPC.h"
#include "client_marshalling.h"
#include "client_commands.h"
#include "client.h"
#include "marshalling.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define EXIT "exit"  // Si se envia EXIT se cierra el cliente
#define ARG_COUNT 2
#define SPACE 1
#define MAX_LEN 256 // Máximo tamaño de entrada permitido

#define UNSUPPORTED_COMMAND "Unsupported Command"
#define NOT_LOGGED_MSG "Must be logged in order to execute command.\nUse login [username]"
#define ALREADY_LOGGED_MSG "Can't login if already logged"

/*
** Estructura análoga al shell del tp de Arqui:
** https://github.com/cbarruffaldi/TPE-Arqui/blob/master/nanOS/Userland/SampleCodeModule/shell.c
*/

static void handle_validity(int valid);
static int readline_no_spaces(char *str, unsigned int maxlen);
static int extract_cmd_name(char * cmd_name, const char * str);


int main(int argc, char *argv[]) {
  char buffer[MAX_LEN];
  char cmd_name[MAX_LEN];
  int name_len;
  int arguments_flag; // 1 si se enviaron argumentos
  int valid;
  sessionADT se;
  t_user user = {"", 0};

  if (argc != ARG_COUNT) {
    fprintf(stderr, "Usage: %s <server_path>", argv[0]);
	   return 1;
  }

 // se = start_session(argv[1]);

  if (se == NULL) {
    printf("Failed to connect\n");
    return 1;
  }

  printf("Client communicated to server\n");

  while (1) {
    printf("> ");
    readline_no_spaces(buffer, MAX_LEN);

    if (strcmp(buffer, EXIT) == 0) { // se recibió EXIT --> nos vamos
      printf("Exiting...\n");
  //    end_session(se);
      return 0;
    }

    if (buffer[0] != '\0') { // Se escribió algo
      name_len = extract_cmd_name(cmd_name, buffer);
      arguments_flag = buffer[name_len] != '\0';
      valid = execute(cmd_name, buffer+name_len+arguments_flag, se, &user);
      handle_validity(valid);
    }

  }

  return 1;
}

/*
** Borra los espacios iniciales y finales de la cadena ingresada por entrada estándar
** como también los espacios repetidos.
** Se extrajo del TP de Arqui:
** https://github.com/cbarruffaldi/TPE-Arqui/blob/master/nanOS/Userland/SampleCodeModule/stdio.c
*/
static int readline_no_spaces(char *str, unsigned int maxlen) {
  unsigned int i = 0;
  int c;
  char state = SPACE;
  while ((c = getchar()) != '\n' && i < maxlen-1) {
    if (state != SPACE) {
      str[i++] = c;
      if (isspace(c))
        state = SPACE;
    }
    else if (!isspace(c)) {
      str[i++] = c;
      state = !SPACE;
    }
  }
  if (i > 0 && isspace(str[i-1]))
    i--;  // Se borra el utimo espacio si lo hay
  str[i] = '\0';
  return i;
}

/*
** Retorna la longitud del comando ingresado y guarda su contenido en un arreglo recibido como parámetro
** Se extrajo del TP de Arqui:
** https://github.com/cbarruffaldi/TPE-Arqui/blob/master/nanOS/Userland/SampleCodeModule/shell.c
*/
static int extract_cmd_name(char * cmd_name, const char * str) {
  int i;
  for (i = 0; str[i] != '\0' && str[i] != ' '; i++)
    cmd_name[i] = str[i];
  cmd_name[i] = '\0';
  return i;
}

/*
** Imprime mensaje de error
*/
static void handle_validity(int valid) {
  switch (valid) {
    case UNSUPPORTED:
      printf("%s\n", UNSUPPORTED_COMMAND);
      break;
    case NOT_LOGGED:
      printf("%s\n", NOT_LOGGED_MSG);
      break;
    case ALREADY_LOGGED:
      printf("%s\n", ALREADY_LOGGED_MSG);
      break;
  }
}
