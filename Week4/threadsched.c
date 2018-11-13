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

#define ROUND_ROBIN 1
#define PRIORITY_ROUND_ROBIN 2
#define SHORTEST_REMAINING_TIME_NEXT 3

typedef struct threadInfo {
    int tNum;
    int tPrio;
    int tStart;
    int tRunTime;
    int tFullTime;
} threadInfo_t;

struct list_elem *getNextThread(list_t *list) {

}

int main(int argc, char *argv[], char *envp[]) {

    //printf("Pid: %d", getpid());

    int nVal = -1;
    int timeStepVal = -1;
    int timeQVal = -1;
    int algoVal = -1;

    int c;

    while ((c = getopt(argc, argv, "n:t:q:a:")) != -1) {
        switch (c) {
            case 'n':
                nVal = atoi(optarg);
                break;
            case 't':
                timeStepVal = atoi(optarg);
                break;
            case 'q':
                timeQVal = atoi(optarg);
                break;
            case 'q':
                switch (optarg) {
                    case "RR":
                        algoVal = ROUND_ROBIN;
                        break;
                    case "PRR":
                        algoVal = PRIORITY_ROUND_ROBIN;
                        break;
                    case "SRTN":
                        algoVal = SHORTEST_REMAINING_TIME_NEXT;
                        break;
                    default:
                        fprintf(stderr,
                                "Possible algorithms: \n\tRound-Robin (\"RR\")\n\tPriority Round-Robin (\"PRR\")\n\tShortest Remaining Time Next(\"SRTN\")\n");
                        abort();
                }
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

    if (nVal == -1 || timeStepVal == -1 || timeQVal == -1 || algoVal == -1) {
        fprintf(stderr, "usage: threadsched -n <N Threads> -t <time step> -q <time quantum> -a <algorithm>");
        abort();
    }

    list_t *list1 = list_init();

    threadInfo_t *threadInfo;

    for (int j = 0; j < nVal; ++j) {
        threadInfo = malloc(sizeof(threadInfo_t));

        int prio;
        int start;
        int runTime;
        scanf("%d %d %d\n", &prio, &start, &runTime);

        threadInfo->tNum = j;
        threadInfo->tPrio = prio;
        threadInfo->tStart = start;
        threadInfo->tRunTime = 0;
        threadInfo->tFullTime = runTime;

        list_append(list1, threadInfo);

    }

    struct list_elem *elem;
    threadInfo_t *cTInfo;
    while (list1->first != NULL) {
        elem = getNextThread(list1);
        cTInfo = elem->data;
        if (cTInfo->tRunTime >= cTInfo->tFullTime) {
            list_remove(list1, elem);
        }
    }

    list_finit(list1);
}