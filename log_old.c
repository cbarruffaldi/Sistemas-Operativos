#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "include/log.h"

typedef struct msgbuf {
    long mtype;
    char mtext[MAX_NOTIFICATION];
}t_msg;

/* Idea de estructura : http://beej.us/guide/bgipc/output/html/multipage/mq.html */
int main(void)
{
    t_msg buf;
    int msqid;
    key_t key;

    if ((key = ftok("server.c", 'B')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }

    printf("READING MESSAGES...\n");

    while(1) {
        if (msgrcv(msqid, &buf, sizeof buf.mtext, 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("\"%s\"\n", buf.mtext);
    }

    return 0;
}
