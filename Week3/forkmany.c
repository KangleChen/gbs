#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "list.h"
#include <string.h>
#include <ctype.h>

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
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    }

    list_t *list1 = list_init();
    int cpid;
    char cpidStr[12];

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
        if ((cpid = fork()) == 0) {
            for (int i = 1; i <= kValP; ++i) {
                sleep(1);
                printf("%d %d %d\n", getpid(), getppid(), i);
            }
            return (getpid() + kValP) % 100;
        } else {
            sprintf(cpidStr, "%d", cpid);
            list_append(list1, cpidStr);
        }
    }

    int stat;
    while (list1->first != NULL) {
        cpid = wait(&stat);
        sprintf(cpidStr, "%d", cpid);
        list_remove(list1, list_find(list1, cpidStr, strcmp));
        printf("Exit-Code: %d\n", WEXITSTATUS(stat));
    }

    time(&now);
    printf("Ende: %s", ctime(&now));

    list_finit(list1);
}