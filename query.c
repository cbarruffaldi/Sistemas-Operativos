#include <stdio.h>
#include "include/query.h"

#define Q_REFRESH "SELECT * FROM "TABLE_TWIT" \
                      WHERE "ATR_TWIT_ID" > %d;"

#define Q_LIKE "UPDATE "TABLE_TWIT" \
                    SET "ATR_TWIT_LIKES"=1+( \
                   		SELECT "ATR_TWIT_LIKES" \
                   		FROM "TABLE_TWIT" \
                   		WHERE "ATR_TWIT_ID" = %d) \
                   	WHERE "ATR_TWIT_ID" = %d;"

#define Q_INSERT "INSERT INTO "TABLE_TWIT" values (\
                  (SELECT 1+MAX("ATR_TWIT_ID") FROM "TABLE_TWIT"), '%s', '%s', 0);"

void query_refresh(char *buffer, int id) {
	sprintf(buffer, Q_REFRESH, id);
}

void query_like(char *buffer, int id) {
	sprintf(buffer, Q_LIKE, id);
}

void query_insert(char *buffer, char *username, char *msg) {
	sprintf(buffer, Q_INSERT, username, msg);
}
