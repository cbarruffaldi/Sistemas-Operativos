
/*
** Estructura análogo al commands.c del TP de Arqui:
** https://github.com/cbarruffaldi/TPE-Arqui/blob/master/nanOS/Userland/SampleCodeModule/commands.c
*/

#include "include/client_marshalling.h"
#include "include/client.h"
#include "include/client_commands.h"
#include "include/marshalling.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Cantidad de comandos */
#define CMDS_SIZE (sizeof(commands)/sizeof(commands[0]))

#define COLUMNS 60

/* Estructura que representa un comando */
typedef struct {
	const char * name;  /* Nombre del comando */
	int (*function) (const char *str, sessionADT se, t_user *uinfo);  /* Funcion correspondiente al comando */
	const char * params;
	const char * help_msg;
} command;


static int help(const char *args, sessionADT se, t_user *uinfo);
static int login(const char *args, sessionADT se, t_user *uinfo);
static int tweet(const char *args, sessionADT se, t_user *uinfo);
static int like(const char *args, sessionADT se, t_user *uinfo);
static int refresh(const char *args, sessionADT se, t_user *uinfo);
static int logout(const char *args, sessionADT se, t_user *uinfo);
static int valid_like(const char *args);
static char * fill(char c, int length);
static void print_user(char * usr);
static void print_msg(char * msg);
static void print_line();
static void print_tweet(t_tweet tw);

static void show_cmd_help(command cmd);

static command commands[]= {{"help", help, "", "Displays commands and descriptions"},
              {"login", login, "[username]", "Logs user with username."},
              {"tweet", tweet, "[msg]", "Tweets a tweet."},
              {"like", like, "[tweet_id]", "Likes a tweet."},
              {"refresh", refresh, "", "Prints most recent tweets."},
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

static int tweet(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;

  int len = strlen(args);
  if (len <= MSG_SIZE && len > 0) {
    printf("Received valid tweet: %s\n", args);
	  send_tweet(se, uinfo->username, args); // retorna el ultimo id que tiene la bd
    return VALID;
  }
  else if (len == 0) {
    printf("Can't tweet an empty tweet!\n");
  }
  else {
    printf("Tweet too long.\n");
  }

  return INVALID_ARGS;
}

static int like(const char *args, sessionADT se, t_user *uinfo) {
  int id;

  if (!logged(uinfo))
    return NOT_LOGGED;

  if (args[0] == '\0') {
    printf("Need tweet id in order to like it\n");
    return INVALID_ARGS;
  }
  if (!valid_like(args)) {
    printf("Invalid tweet id\n");
    return INVALID_ARGS;
  }

  id = atoi(args);
  printf("Received valid like %d\n", id);

	send_like(se, id);  // Devuelve el número de likes del tweet pero no se usa.

  return VALID;
}

static int refresh(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;
	int count;
	t_tweet * tws = send_refresh(se, 10, &count); // TODO: cantidad de tws a refreshear

  printf("Received refresh %s\n", args);
	return 0;
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

static void print_tweet(t_tweet tw) {
  print_user(tw.user);
  print_msg(tw.msg);
  printf("| id:%5d | likes:%5d |%s|\n", tw.id, tw.likes, fill(' ', COLUMNS - 27));
  print_line();
}

static void print_tweets(t_tweet * tws, int count) {
  int i;
  print_line();
  for (i = 0; i < count; i++) {
    print_tweet(tws[i]);
  }
}

static void print_line() {
  printf(" %s\n", fill('-', COLUMNS - 2));
}

static void print_user(char * usr) {
  printf("| %s:%s|\n", usr, fill(' ', COLUMNS - strlen(usr) - 4));
}

static void print_msg(char * msg) {
  printf("| %s%s|\n", msg, fill(' ', COLUMNS - strlen(msg) - 3));
}

static char * fill(char c, int length) {
  char * arr = malloc(length + 1);
  int i = 0;
  for (;i < length; i++) {
    arr[i] = c;
  }
  arr[i] = '\0';
  return arr;
}
