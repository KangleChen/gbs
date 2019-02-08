#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "memory.h"

struct command {
    enum {
        ALLOC_CMD, FREE_CMD
    } cmd;
    int arg;
};

void print_pointer(void *p){
    printf("%p, ", p);
}


int main(int argc, char *argv[]) {

    list_t *commandList = list_init();

    enum mem_algo mem_strat = NONE;

    int total = 1048576;
    int block = 4096;

    struct command *cmdP;

    char option;
    while ((option = getopt(argc, argv, "m:b:a:f:1w")) != -1) {
        switch (option) {
            case 'm':
                total = atoi(optarg);
                break;
            case 'b':
                block = atoi(optarg);
                break;
            case 'a':
                cmdP = malloc(sizeof(struct command));
                cmdP->cmd = ALLOC_CMD;
                cmdP->arg = atoi(optarg);
                list_append(commandList, cmdP);
                break;
            case 'f':
                cmdP = malloc(sizeof(struct command));
                cmdP->cmd = FREE_CMD;
                cmdP->arg = atoi(optarg);
                list_append(commandList, cmdP);
                break;
            case '1':
                mem_strat = FIRST_FIT;
                break;
            case 'w':
                mem_strat = WORST_FIT;
                break;
            case '?':
                if (optopt == 'm' || optopt == 'b' || optopt == 'a' || optopt == 'f')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return -1;
            default:
                abort();
        }
    }

    if (mem_strat == NONE) {
        fprintf(stderr, "You must specify an algorithm!");
        return -1;
    }

    list_t *allocList = list_init();

    mem_init(total, block, mem_strat);
    mem_dump();

    for (struct list_elem *curr = commandList->first; curr;) {
        struct command *currData = (struct command *)curr->data;
        switch (currData->cmd){
            case ALLOC_CMD:
                list_append(allocList, mem_alloc(currData->arg));
                break;
            case FREE_CMD: {
                //list_print(allocList, print_pointer);
                struct list_elem *currI = allocList->first;
                for (int i = 1; currI != NULL; i++) {
                    if (i == currData->arg) break;
                    currI = currI->next;
                }
                if (currI != NULL) {
                    //printf("Free %p\n", currI->data);
                    mem_free(currI->data);
                } else {
                    //printf("Cant be freed.\n");
                }
            }
                break;
            default:
                break;
        }

        struct list_elem *tElem = curr->next;
        list_remove(commandList, curr);
        curr = tElem;

        mem_dump();
    }

    list_finit(commandList);
    list_finit(allocList);
}