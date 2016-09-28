#ifndef _QUERY_H_
#define _QUERY_H_

#include "database.h"

/* Las funciones almacenan en buffer la query */

void query_refresh(char *buffer, int id);

void query_like(char *buffer, int id);

void query_insert(char *buffer, char *username, char *msg);


#endif