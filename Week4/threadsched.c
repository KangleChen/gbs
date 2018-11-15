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

int cmpPointer(const void *p1, const void *p2) {
    return p1 - p2;
}

struct list_elem *getNextThread(list_t *list, struct list_elem *currT, int algo) {
    if (algo == ROUND_ROBIN) {
        if (currT == NULL || currT->next == NULL) {
            return list->first;
        }
        return currT->next;
    } else if (algo == PRIORITY_ROUND_ROBIN || algo == SHORTEST_REMAINING_TIME_NEXT) {
        if (currT != NULL) {
            //printf("Curr Prio: %d\nFirst Prio: %d\n", ((threadInfo_t *)currT->data)->tPrio, ((threadInfo_t *)list->first->data)->tPrio);
        }
        if (currT == NULL || currT->next == NULL) {
            return list->first;
        }
        threadInfo_t *nextI = currT->next->data;
        threadInfo_t *firstI = list->first->data;
        if ((algo == PRIORITY_ROUND_ROBIN && nextI->tPrio != firstI->tPrio) ||
            (algo == SHORTEST_REMAINING_TIME_NEXT &&
             (nextI->tFullTime - nextI->tRunTime) != (firstI->tFullTime - firstI->tRunTime))) {
            return list->first;
        }
        return currT->next;
    } else {
        return NULL;
    }
}

void printPointer(void *ptr) {
    printf("%p:\n\tThread Num: %d\n\tThread prio: %d\n", ptr, ((threadInfo_t *) ptr)->tNum,
           ((threadInfo_t *) ptr)->tPrio);
}

void sortReady(list_t *ready, int algo) {
    if (algo == ROUND_ROBIN) {
        return;
    }
    list_t *resultList = list_init();

    threadInfo_t *currTI;
    threadInfo_t *min;
    struct list_elem *minElem;
    while (ready->first != NULL) {
        min = NULL;
        for (struct list_elem *curr = ready->first; curr != NULL; curr = curr->next) {
            currTI = curr->data;
            if (min == NULL ||
                (algo == PRIORITY_ROUND_ROBIN && currTI->tPrio < min->tPrio) ||
                (algo == SHORTEST_REMAINING_TIME_NEXT &&
                 (currTI->tFullTime - currTI->tRunTime) < (min->tFullTime - min->tRunTime))) {
                min = currTI;
                minElem = curr;
            }
        }
        list_append(resultList, min);
        list_remove(ready, minElem);
    }

    ready->first = resultList->first;
    ready->last = resultList->last;
    free(resultList);

    //list_print(ready, printPointer);
}

int readyThreads(int time, list_t *waiting, list_t *ready, int algo) {
    int n = 0;
    struct list_elem *next;
    for (struct list_elem *curr = waiting->first; curr != NULL;) {
        threadInfo_t *cThreadInfo = ((threadInfo_t *) curr->data);
        if (cThreadInfo->tStart <= time) {
            next = curr->next;
            list_remove(waiting, curr);
            list_append(ready, cThreadInfo);
            n++;
            curr = next;
            //printf("Starting thread %d at time %6d\n", cThreadInfo->tNum, time);
        } else {
            curr = curr->next;
        }
    }
    if (n > 0) {
        sortReady(ready, algo);
    }
    return n;
}

void print_time_step(int time, int thread_num) {
    static int first_time = 1;
    int i;

    if (first_time) {
        printf("  Time |  1  2  3  4  5  6  7  8  9  10\n");
        printf("-------+--------------------------------\n");
        first_time = 0;
    }
    printf("%06d |", time);
    if (thread_num) {
        for (i = 1; i < thread_num; i++)
            printf("   ");
        printf("  %d\n", thread_num);
    } else
        printf("\n");
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
            case 'a':
                if (strcmp(optarg, "RR") == 0) {
                    algoVal = ROUND_ROBIN;
                } else if (strcmp(optarg, "PRR") == 0) {
                    algoVal = PRIORITY_ROUND_ROBIN;
                } else if (strcmp(optarg, "SRTN") == 0) {
                    algoVal = SHORTEST_REMAINING_TIME_NEXT;
                } else {
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


    list_t *ready = list_init();
    list_t *waiting = list_init();


    threadInfo_t *threadInfo;

    for (int j = 1; j <= nVal; ++j) {
        threadInfo = malloc(sizeof(threadInfo_t));

        int prio;
        int start;
        int runTime;
        scanf("%d %d %d", &prio, &start, &runTime);
        //printf("%d %d %d\n", prio, start, runTime);

        /*if(start>10000){
            abort();
        }*/

        threadInfo->tNum = j;
        threadInfo->tPrio = prio;
        threadInfo->tStart = start;
        threadInfo->tRunTime = 0;
        threadInfo->tFullTime = runTime;


        list_append(waiting, threadInfo);


    }

    readyThreads(0, waiting, ready, algoVal);

    struct list_elem *lastElem = NULL;
    struct list_elem *elem = getNextThread(ready, NULL, algoVal);
    threadInfo_t *cTInfo;
    int thread_num = 0;

    int time = 0;

    while (ready->first != NULL || waiting->first != NULL) {


        thread_num = 0;

        if (elem != NULL) {
            cTInfo = elem->data;
            thread_num = cTInfo->tNum;
        }

        for (int i = 0; i < timeQVal; i += timeStepVal) {
            print_time_step(time, thread_num);
            time += timeStepVal;
            if (thread_num) {
                cTInfo->tRunTime += timeStepVal;
                /*if (cTInfo->tRunTime >= cTInfo->tFullTime) {
                    break;
                }*/
            } else {
                break;
            }
        }

        if (readyThreads(time, waiting, ready, algoVal) > 0 && thread_num) {
            elem = list_find(ready, cTInfo, cmpPointer);
        };

        if (elem != NULL) {
            lastElem = malloc(sizeof(struct list_elem));
            lastElem->data = elem->data;
            lastElem->next = elem->next;
        } else {
            lastElem = NULL;
        }
        if (thread_num && cTInfo->tRunTime >= cTInfo->tFullTime) {
            list_remove(ready, elem);
            //list_print(ready, printPointer);
        }
        elem = getNextThread(ready, lastElem, algoVal);

    }

    list_finit(ready);
    list_finit(waiting);
}