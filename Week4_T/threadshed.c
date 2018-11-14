#include <list.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Thread {
    int id;
    int priority;
    int startTime;
    int runningTime;
    int targetTime;
} Thread;

int main(int argc, char **argvs) {
    int numThread;
    int numStep;
    int quantum;
    char *algorithm;
    int c;

    while ((c = getopt(argc, argvs, "n:t:q:a:")) != -1) {
        switch (c) {
            case 'n':
                numThread = atoi(optarg);
                break;
            case 't':
                numStep = atoi(optarg);
                break;
            case 'q':
                quantum = atoi(optarg);
                break;
            case 'a':
                algorithm = optarg;
                break;
            case '?':
                // TODO: fix the argument check safety later
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

    // Parsing done, create threads with parameters
    list_t *myThreads = list_init();
    for (int i = 0; i < numThread; i++) {
        Thread t;
        fscanf(stdin, "%d %d %d \n", t.priority, t.startTime, t.targetTime);
        if (t.priority < 1 || t.priority > 10) {
            perror("Priority out of bound");
            return -1;
        }
    }
}