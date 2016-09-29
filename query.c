
#include <stdio.h>
#include "include/query.h"

#define Q_REFRESH "SELECT * FROM "TABLE_TWEET" \
                      WHERE "ATR_TWEET_ID" > %d;"

#define Q_LIKE "UPDATE "TABLE_TWEET" \
                    SET "ATR_TWEET_LIKES"=1+( \
                   		SELECT "ATR_TWEET_LIKES" \
                   		FROM "TABLE_TWEET" \
                   		WHERE "ATR_TWEET_ID" = %d) \
                   	WHERE "ATR_TWEET_ID" = %d;"

#define Q_INSERT "INSERT INTO "TABLE_TWEET" values (\
                  (SELECT 1+COALESCE(MAX("ATR_TWEET_ID"),0) FROM "TABLE_TWEET"), '%s', '%s', 0);"

void query_refresh(char *buffer, int id) {
	sprintf(buffer, Q_REFRESH, id);
}

void query_like(char *buffer, int id) {
	sprintf(buffer, Q_LIKE, id);
}

void query_insert(char *buffer, char *username, char *msg) {
	sprintf(buffer, Q_INSERT, username, msg);
}
