#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "list.h"

int intPcmp(const void *intP1, const void *intP2) {
    return (*(int *) intP1) - (*(int *) intP2);
}

int main(int argc, char *argv[], char *envp[]) {

    list_t *tL = list_init();
    list_t *cL = list_init();

    int pipeA[2];

    pipe(pipeA);

    int *cP = malloc(sizeof(int));
    *cP = pipeA[0];
    list_append(cL, cP);
    cP = malloc(sizeof(int));
    *cP = pipeA[1];
    list_append(cL, cP);


    int cpid;

    if ((cpid = fork()) == 0) {
        char *argv[] = {"/bin/ping", "-c", "5", "localhost", NULL};

        if (pipeA[1] > 0) {
            dup2(pipeA[1], STDOUT_FILENO);
        }

        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int*)cC->data));
            free(cC->data);
            list_remove(cL, cC);
        }

        execve(argv[0], argv, envp);

        fprintf(stderr, "Command '%s' not found\n", argv[0]);
        exit(-1);
    } else {
        int *cpidP = malloc(sizeof(int));
        *cpidP = cpid;
        list_append(tL, cpidP);

        if ((cpid = fork()) == 0) {
            char *argv[2] = {"/usr/bin/rev", NULL};

            if (pipeA[0] > 0) {
                dup2(pipeA[0], STDIN_FILENO);
            }

            while (cL->first != NULL) {
                struct list_elem *cC = cL->first;
                close(*((int*)cC->data));
                free(cC->data);
                list_remove(cL, cC);
            }

            execve(argv[0], argv, envp);

            fprintf(stderr, "Command '%s' not found (%d)\n", argv[0], errno);
            exit(-1);
        } else {
            int *cpidP = malloc(sizeof(int));
            *cpidP = cpid;
            list_append(tL, cpidP);



        }
    }

    while (cL->first != NULL) {
        struct list_elem *cC = cL->first;
        close(*((int*)cC->data));
        free(cC->data);
        list_remove(cL, cC);
    }

    while (tL->first != NULL) {
        cpid = wait(NULL);
        list_remove(tL, list_find(tL, &cpid, intPcmp));
    }

}

