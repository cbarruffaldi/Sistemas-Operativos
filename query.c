#include <stdio.h>
#include "include/query.h"

#define Q_REFRESH "\
SELECT * FROM "TABLE_TWEET" \
WHERE "ATR_TWEET_ID" >= %d AND "ATR_TWEET_ID" < %d;"

#define Q_LIKE "\
UPDATE "TABLE_TWEET" SET "ATR_TWEET_LIKES"= 1 + \
(SELECT "ATR_TWEET_LIKES" \
FROM "TABLE_TWEET" \
WHERE "ATR_TWEET_ID" = %d) \
WHERE "ATR_TWEET_ID" = %d; \
SELECT "ATR_TWEET_LIKES" \
FROM "TABLE_TWEET" \
WHERE "ATR_TWEET_ID" = %d;"

#define Q_INSERT "\
INSERT INTO "TABLE_TWEET" values \
((SELECT COALESCE(MAX("ATR_TWEET_ID") + 1, 0) FROM "TABLE_TWEET"), '%s', '%s', 0); \
SELECT MAX("ATR_TWEET_ID") FROM "TABLE_TWEET";"

void query_refresh(char *buffer, int first_id) {
	sprintf(buffer, Q_REFRESH, first_id, first_id + TW_BLOCK_SIZE);
}

void query_like(char *buffer, int id) {
	sprintf(buffer, Q_LIKE, id, id, id);
}

void query_insert(char *buffer, const char *username, const char *msg) {
	sprintf(buffer, Q_INSERT, username, msg);
}
