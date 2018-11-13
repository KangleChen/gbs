#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "list.h"
#include <string.h>
#include <ctype.h>
#include <pthread.h>

void *countFunc(void *vargp) {

    int *kValP = (int *) vargp;

    for (long int i = 1; i <= *kValP; ++i) {
        sleep(1);
        printf("%10u\t%d\t%ld\n", (unsigned int) pthread_self(), getpid(), i);
    }

    return NULL;
}

int main(int argc, char *argv[], char *envp[]) {

    //printf("Pid: %d", getpid());

    int kVal = 10;
    int nVal = 1;
    int rFlag = 0;

    int c;

    while ((c = getopt(argc, argv, "k:n:r")) != -1) {
        switch (c) {
            case 'k':
                kVal = atoi(optarg);
                break;
            case 'n':
                nVal = atoi(optarg);
                break;
            case 'r':
                rFlag = 1;
                break;
            case '?':
                if (optopt == 'k' || optopt == 'n')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    }

    list_t *list1 = list_init();
    pthread_t *ctid = calloc(1, sizeof(pthread_t));

    srand(time(NULL));
    int maxR, minR = 0;
    if (rFlag) {
        minR = kVal / 2;
        maxR = 1.5 * kVal;
    }

    time_t now;
    time(&now);
    printf("Start: %s", ctime(&now));

    for (int j = 0; j < nVal; ++j) {
        int kValP = rFlag ? (rand() % (maxR + 1 - minR) + minR) : kVal;
        if (pthread_create(ctid, NULL, countFunc, &kValP) == 0) {
            list_append(list1, ctid);
            //printf("Thread %10u started\n", (unsigned int) *ctid);
        } else {
            fprintf(stderr, "Failed creating thread.");
        }
        ctid = calloc(1, sizeof(pthread_t));
    }

    struct list_elem *elem;
    while (list1->first != NULL) {
        elem = list1->first;
        ctid = elem->data;
        pthread_join(*ctid, NULL);
        //printf("Thread %10u exited\n", (unsigned int) *ctid);
        list_remove(list1, elem);
    }

    time(&now);
    printf("Ende: %s", ctime(&now));

    list_finit(list1);
}