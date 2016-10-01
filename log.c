#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#include "include/log.h"

//Idea sacada de :
//http://man7.org/tlpi/download/TLPI-52-POSIX_Message_Queues.pdf
//http://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system

int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_NOTIFICATION + 1];
    int must_stop = 0;

    /* Inicializar atributos */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_NOTIFICATION;
    attr.mq_curmsgs = 0;

    /* Crear la MQ */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
    printf("MQ INITIALIZED...\n");

    do {
        ssize_t bytes_read;

        /* recibir mensaje */
        bytes_read = mq_receive(mq, buffer, MAX_NOTIFICATION, NULL);
        CHECK(bytes_read >= 0);

        buffer[bytes_read] = '\0';
        if (! strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
        {
            must_stop = 1;
        }
        else
        {
            printf("%s\n", buffer);
        }
    } while (!must_stop);

    /* clean*/
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

    return 0;
}
