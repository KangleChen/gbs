
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"
#include "list.h"

int main(int argc, char *argv[]){
    void *aufruf[10];
    for (int i = 0; i < 10; i++) {
        aufruf[i] = NULL;
    }
    int count = 1; //count how many mem_alloc() there are

    char option;
    int option_argument;
    while ((option = getopt(argc, argv, "m:n:a:f:1w")) != -1) {
        switch (option){
            case 'm': //-m <total_memory>
                option_argument=atoi(optarg);
                total_memory = option_argument;
                break;
            case 'n':
                option_argument = atoi(optarg);
                blocksize = option_argument;
                mem_init(total_memory, blocksize);
                mem_dump();
                //printf("\n");
                break;
            case 'a':
                option_argument=atoi(optarg);
                aufruf[count++] = mem_alloc(option_argument);
                mem_dump();
                //printf("\n");
                break;
            case 'f':
                option_argument=atoi(optarg);
                mem_free(aufruf[option_argument]);
                mem_dump();
                //printf("\n");
                break;
            case '1':
                myAlgo=FIRST_FIT;
                break;
            case 'w':
                myAlgo=WORST_FIT;
                break;
            default:
                abort();

        }
    }

}

