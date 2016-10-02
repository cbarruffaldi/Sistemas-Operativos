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

/* Espacio de "ancho" que tienen los tweets para imprirse, en cantidad de caracteres */
#define MSG_COLUMNS 60

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
static int show(const char *args, sessionADT se, t_user *uinfo);
static int delete(const char *args, sessionADT se, t_user *uinfo);

static void sanitize_msg(char msg[]);
static int valid_id(const char *args);
static int valid_username(const char *username);

static char * fill(char c, int length);
static void print_border();
static void print_user(char usr[]);
static void print_msg(char msg[]);
static void print_info(int id, int likes);
static void print_tweet(t_tweet tw);
static void print_tweets(t_tweet * tws, int count);
static void close_line(int filled);
static void start_of_line();

static void show_cmd_help(command cmd);

static command commands[]= {{"help", help, "", "Displays commands and descriptions"},
              {"login", login, "[username]", "Logs user with username."},
              {"tweet", tweet, "[msg]", "Tweets a tweet."},
			  			{"tw", tweet, "[msg]", "Tweets a tweet."},
              {"like", like, "[tweet_id]", "Likes a tweet."},
              {"refresh", refresh, "", "Prints most recent tweets."},
              {"logout", logout, "", "Logs out."},
              {"show", show, "[tweet_id]", "Prints tweet with tweet_id."},
			  			{"delete", delete, "[tweet_id]", "Deletes tweet with tweet_id if it was sent by the same user trying to delete"}
			  			};

static int logged(t_user *uinfo);

int run_command(const char *name, const char *args, sessionADT se, t_user *user) {
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

  if (len < USER_SIZE && len > 0 && valid_username(args)) {
    strcpy(uinfo->username, args);
    uinfo->twit_id = 0;
		send_login(se, args);
    return VALID;
  }
  else if (len == 0) {
    printf("You must provide a username.\n");
  }
  else if (len >= USER_SIZE) {
    printf("Username too long.\n");
  }
  else {
    printf("Invalid username.\n");
  }

  return INVALID_ARGS;
}

static int valid_username(const char *username) {
  return !isdigit(username[0]) && strstr(username, SEPARATOR) == NULL;
}

static int tweet(const char *args, sessionADT se, t_user *uinfo) {
  int len = strlen(args);
	char tw_msg[MSG_SIZE];

  if (!logged(uinfo))
    return NOT_LOGGED;

  if (len <= MSG_SIZE && len > 0) {
		strcpy(tw_msg, args);
		sanitize_msg(tw_msg);

	  send_tweet(se, tw_msg);

		refresh(args, se, uinfo);
    return VALID;
  }
  else if (len == 0) {
    printf("Can't tweet an empty tweet!\n");
  }
  else {
    printf("Tweet too long. Tweets can't exceed %d characters\n",MSG_SIZE);
  }

  return INVALID_ARGS;
}

static void sanitize_msg(char msg[]) {
	char * aux;
  int len = strlen(SEPARATOR);
  char * spaces = fill(' ', len);
	while ((aux = strstr(msg, SEPARATOR)) != NULL) {
		memcpy(aux, spaces, len);
	}
  free(spaces);
}

static int like(const char *args, sessionADT se, t_user *uinfo) {
  int id, likes;

  if (!logged(uinfo))
    return NOT_LOGGED;

  if (args[0] == '\0') {
    printf("Need tweet id in order to like it\nFor more information use 'help'\n");
    return INVALID_ARGS;
  }
  if (!valid_id(args)) {
    printf("Invalid tweet id\n");
    return INVALID_ARGS;
  }

  id = atoi(args);

	likes = send_like(se, id);

	if (likes == -1) {
		printf("No tweet with id %d\n", id);
		return INVALID_ARGS;
	}

  printf("That tweet ended up with %d like%c\n", likes, likes > 1 ? 's' : ' ');

  return VALID;
}

static int refresh(const char *args, sessionADT se, t_user *uinfo) {
	int count;
	t_tweet * tws;
  if (!logged(uinfo))
    return NOT_LOGGED;

	tws = send_refresh(se, &count);
	if (tws != NULL)
		print_tweets(tws, count);
	else
		printf("Received NULL tweet array\n");   //TODO:Sacarlo

	free(tws);
	return VALID;
}

static int logout(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;
  uinfo->username[0] = '\0';
	send_logout(se);
  return VALID;
}

static int show(const char *args, sessionADT se, t_user *uinfo) {
  if (!logged(uinfo))
    return NOT_LOGGED;
  if (!valid_id(args))
    return INVALID_ARGS;

  t_tweet tw = send_show(se, atoi(args));
  if (tw.msg[0] == '\0') {
    printf("Invalid id\n");
    return INVALID_ARGS;
  }
  print_tweets(&tw, 1);
  return VALID;
}

static int delete(const char *args, sessionADT se, t_user *uinfo) {
	int ans, id;
  if (!logged(uinfo))
    return NOT_LOGGED;
  if (!valid_id(args))
    return INVALID_ARGS;

	id = atoi(args);
  ans = send_delete(se, id);

	if (ans == -1) {
    printf("Can't delete that tweet\n");
    return INVALID_ARGS;
	}
	if (ans == 1) {
		printf("Deleted tweet with id %d\n", id);
	}

  return VALID;
}

static int logged(t_user *uinfo) {
  return uinfo->username[0] != '\0';
}

static int valid_id(const char *args) {
  int i;
  for (i = 0; args[i] != '\0'; i++)
    if (!isdigit(args[i]))
      return 0;
  return 1;
}

static void print_tweets(t_tweet * tws, int count) {
  int i;
  print_border();
  for (i = 0; i < count; i++) {
    print_tweet(tws[i]);
  }
}

static void print_tweet(t_tweet tw) {
  print_user(tw.user);
  print_msg(tw.msg);
	print_info(tw.id, tw.likes);
  print_border();
}

static void print_border() {
  char * border = fill('-', MSG_COLUMNS - 2);
  printf(" %s\n", border);
  free(border);
}

static void print_user(char usr[]) {
  char * spaces = fill(' ', MSG_COLUMNS - strlen(usr) - 4);
  printf("| %s:%s|\n", usr, spaces);
  free(spaces);
}

static void print_msg(char msg[]) {
  char * word = strtok(msg, " ");
  int filled = 0;

	start_of_line();
  while (word != NULL) {
    int wlen = strlen(word);
    if (wlen + filled < MSG_COLUMNS - 4 || \
      (filled == 0 && wlen > MSG_COLUMNS - 4)) { // la palabra no entra en un renglon entero.
      printf("%s ", word);
      filled += (wlen + 1);
      word = strtok(NULL, " ");
    }
    else {
			close_line(filled);
      start_of_line();
      filled = 0;
    }
  }
  close_line(filled);
}

static void print_info(int id, int likes) {
	char * spaces = fill(' ', MSG_COLUMNS - 30);
	printf("| id #%-6d | likes: %-5d |%s|\n", id, likes, spaces);
	free(spaces);
}

static void start_of_line() {
	printf("| ");
}

static void close_line(int filled) {
  int c = MSG_COLUMNS - filled - 3;
	c = (c >= 0) ? c : 0;
  char * spaces = fill(' ', c);
	printf("%s|\n", spaces);
  free(spaces);
}

static char * fill(char c, int length) {
  char * arr = malloc(length + 1);
  int i;
  for (i = 0; i < length; i++) {
    arr[i] = c;
  }
  arr[i] = '\0';
  return arr;
}
