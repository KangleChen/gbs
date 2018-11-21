#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "list.h"

void *thread_job(void *threadNumber) {
    int myNumber= *(int *)threadNumber;
    char fileName[20];
    sprintf(fileName, "%d", myNumber);
    strcat(fileName, ".txt");
    int file=open(fileName, O_RDONLY);
    if (file<0){
        perror("Cannot open file");
        return NULL;
    }
    char buffer[64];
    int i=0;
    ssize_t bytesRead;
    while((bytesRead=read(file, buffer, 64))){
        char prefix[20];
        sprintf(prefix, "[%02d] %03d\t", myNumber, i);
        if (write(fileno(stdout), prefix, strlen(prefix)+1)==-1){
            perror("There was an error while writing to stdout");
            abort();
        }
        write(fileno(stdout), buffer, (int) bytesRead);
        write(fileno(stdout), "\n", 1);
        i++;
    }
    close(file);
}

int main(int argc, char *argv[], char *envp[]) {
    int numThreads = 1;
    int lFlag = 0;
    int rFlag=0;

    int c;

    while ((c = getopt(argc, argv, "n:lr")) != -1) {
        switch (c) {
            case 'n':
                numThreads=atoi(optarg);
                if (numThreads>10){
                    perror("Can only support maximum of 10 threads");
                    abort();
                }
                break;
            case 'l':
                lFlag=1;
                break;
            case 'r':
                rFlag=1;
                break;
            case '?':
                if (optopt == 'n')
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
    //this is a very subltle bug, we need a new variable for laufendeNummer, because we can't guarantee when the thread will deference the pointer, leading to false number passed
    //Variables declared inside a loop won't be repeatedly allocated
    int *threadNumber=calloc(10, sizeof(int));

    for (int laufendeNummer = 0; laufendeNummer < numThreads; ++laufendeNummer) {
        pthread_t threadID;
        *threadNumber=laufendeNummer;
        pthread_create(&threadID, NULL, &thread_job, threadNumber);
        list_append(thread_list, &threadID);
        threadNumber++;
    }

    for (list_elem *curr = thread_list->first; curr; curr = curr->next) {
        pthread_join(*((pthread_t *) curr->data), NULL);
    }

    list_finit(thread_list);
    return 0;
}