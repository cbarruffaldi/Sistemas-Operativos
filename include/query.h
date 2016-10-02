#ifndef _QUERY_H_
#define _QUERY_H_

#include "database.h"

#define QUERY_SIZE 512

#define TW_BLOCK_SIZE 10

/* Las funciones almacenan en buffer la query */

void query_refresh(char *buffer, int id);

void query_like(char *buffer, int id);

void query_insert(char *buffer, const char *username, const char *msg);

void query_show(char *sql, int id);

void query_delete(char *sql, const char *username, int id);

#endif
