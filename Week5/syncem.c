#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "list.h"

void *thread_job(void *args) {
    int kValP = *(int *) args;
    for (int i = 1; i <= kValP; ++i) {
        sleep(1);
        printf("%10u\t%d\t%ld\n", (unsigned int) pthread_self(), getpid(), i);
    }
    return NULL;
}

int main(int argc, char *argv[], char *envp[]) {
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
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    list_t *thread_list = list_init();

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
        pthread_t threadID;
        pthread_create(&threadID, NULL, &thread_job, &kValP);
        char threadIDString[20];
        sprintf(threadIDString, "%lu", threadID);
        list_append(thread_list, threadIDString);
    }

    for (list_elem *curr = thread_list->first; curr; curr = curr->next) {
        pthread_join(atol(curr->data), NULL);
    }

    time(&now);
    printf("Ende: %s", ctime(&now));

    list_finit(thread_list);
    return 0;
}