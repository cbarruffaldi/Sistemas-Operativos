#ifndef _QUERY_H_
#define _QUERY_H_

#include "database.h"

#define QUERY_SIZE 512

/* Las funciones almacenan en buffer la query */
/* Las funciones generan las consultas SQL necesarias para enviar
   a la base de datos según el comando */

void query_refresh(char *buffer, int id, int max_count);

void query_like(char *buffer, int id);

void query_insert(char *buffer, const char *username, const char *msg);

void query_show(char *sql, int id);

void query_delete(char *sql, const char *username, int id);

#endif
