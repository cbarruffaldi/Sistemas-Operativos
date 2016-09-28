/*
** Estructura análogo al commands.c del TP de Arqui:
** https://github.com/cbarruffaldi/TPE-Arqui/blob/master/nanOS/Userland/SampleCodeModule/commands.c
*/

#include "client_marshalling.h"
#include "client.h"
#include "client_commands.h"

#include <stdio.h>
#include <string.h>

/* Cantidad de comandos */
#define CMDS_SIZE (sizeof(commands)/sizeof(commands[0]))


/* Estructura que representa un comando */
typedef struct {
	const char * name;  /* Nombre del comando */
	int (*function) (const char *str, sessionADT se, t_user *uinfo);  /* Funcion correspondiente al comando */
	const char * params;
	const char * help_msg;
} command;


static int help(const char *args, sessionADT se, t_user *uinfo);
static int login(const char *args, sessionADT se, t_user *uinfo);
static int twit(const char *args, sessionADT se, t_user *uinfo);
static int like(const char *args, sessionADT se, t_user *uinfo);
static int refresh(const char *args, sessionADT se, t_user *uinfo);
static int logout(const char *args, sessionADT se, t_user *uinfo);
static int valid_like(const char *args);

static void show_cmd_help(command cmd);

static command commands[]= {{"help", help, "", "Displays commands and descriptions"},
              {"login", login, "[username]", "Logs user with username."},
              {"twit", twit, "[msg]", "Twits a twit."},
              {"like", like, "[twit_id]", "Likes a twit."},
              {"refresh", refresh, "", "Prints most recent twits."},
              {"logout", logout, "", "Logs out."},
              };

static int logged(t_user *uinfo);

int execute(const char *name, const char *args, sessionADT se, t_user *user) {
	int i;
	for (i = 0; i < CMDS_SIZE; i++) {
		if (strcmp(name, commands[i].name) == 0)
			return (* commands[i].function) (args, se, user);
	}
	return UNSUPPORTED;
}

static int help(const char *args, sessionADT se, t_user *uinfo) {
  int i;
  for (i = 0; i < CMDS_SIZE; i++)
    show_cmd_help(commands[i]);
  return VALID;
}

static void show_cmd_help(command cmd) {
    printf("%s %s -- %s\n", cmd.name, cmd.params, cmd.help_msg);
}

static int login(const char *args, sessionADT se, t_user *uinfo) {
  int len = strlen(args);

  if(logged(uinfo)) {
    return ALREADY_LOGGED;
  }

  if (len <= USER_SIZE && len > 0) {
    strcpy(uinfo->username, args);
    uinfo->twit_id = 0;
    return VALID;
  }
  else if (len == 0) {
    printf("You must provide a username.\n");
  }
  else {
    printf("Username too long.\n");
  }

  return INVALID_ARGS;
}

static int twit(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;

  int len = strlen(args);
  if (len <= MSG_SIZE && len > 0) {
    printf("Received valid twit: %s\n", args);
 //   send_tweet(se, uinfo->username, args);
    return VALID;
  }
  else if (len == 0) {
    printf("Can't twit an empty twit!\n");
  }
  else {
    printf("Twit too long.\n");
  }

  return INVALID_ARGS;
}

static int like(const char *args, sessionADT se, t_user *uinfo) {
  int id;

  if (!logged(uinfo))
    return NOT_LOGGED;

  if (args[0] == '\0') {
    printf("Need twit id in order to like it\n");
    return INVALID_ARGS;
  }
  if (!valid_like(args)) {
    printf("Invalid twit id\n");
    return INVALID_ARGS;
  }

  id = atoi(args);
  printf("Received valid like %d\n", id);
  return VALID;
}

static int refresh(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;
  printf("Received refresh %s\n", args);
}

static int logout(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;
  uinfo->username[0] = '\0';
  return VALID;
}

static int logged(t_user *uinfo) {
  return uinfo->username[0] != '\0';
}

static int valid_like(const char *args) {
  int i;
  for (i = 0; args[i] != '\0'; i++)
    if (!isdigit(args[i]))
      return 0;
  return 1;
}