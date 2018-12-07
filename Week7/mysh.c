#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "list.h"

extern list_t *myParse(list_t *res, char *str, char *envp[]);

extern int myParseStg2(list_t *args, char **outFileP, char **inFileP);

extern int myParsePipe(list_t *args, list_t *args2, int pipeA[]);

int intPcmp(const void *intP1, const void *intP2) {
    return (*(int *) intP1) - (*(int *) intP2);
}

int processCmd(list_t *tL, list_t *cmdList, char *envp[], int inPipe, list_t *cL) {
    char **outFileP = malloc(sizeof(char *));
    char **inFileP = malloc(sizeof(char *));
    int cpid;

    list_t *args2 = list_init();


    int pipeA[2];
    if (myParsePipe(cmdList, args2, pipeA) >= 0) {
        processCmd(tL, args2, envp, pipeA[0], cL);
    } else {
        pipeA[0] = -1;
        pipeA[1] = -1;
    }

    myParseStg2(cmdList, outFileP, inFileP);

    if ((cpid = fork()) == 0) {
        char *argv[list_length(cmdList) + 1];
        list_to_array(cmdList, (void **) argv);
        argv[list_length(cmdList)] = NULL;
        list_finit(cmdList);

        char delimiter[] = ":";
        char *ptr;
        char *string = getenv("PATH");
        if (string == NULL) {
            fprintf(stderr, "No PATH variable");
            exit(-1);
        }

        if (pipeA[0] >= 0) {
            close(pipeA[0]);
        }

        if (pipeA[1] >= 0) {
            dup2(pipeA[1], STDOUT_FILENO);
            close(pipeA[1]);
        }

        if (inPipe >= 0) {
            dup2(inPipe, STDIN_FILENO);
            close(inPipe);
        }

        int fd = -1;

        if (*outFileP != NULL && (fd = open(*outFileP, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) >= 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (*inFileP != NULL && (fd = open(*inFileP, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) >= 0) {
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (strchr(argv[0], '/') != NULL) {
            execve(argv[0], argv, envp);
        } else {

            ptr = strtok(string, delimiter);

            while (ptr != NULL) {
                char cmd[strlen(ptr) + strlen(argv[0]) + 2];
                sprintf(cmd, "%s/%s", ptr, argv[0]);

                execve(cmd, argv, envp);

                // naechsten Abschnitt erstellen
                ptr = strtok(NULL, delimiter);
            }

            fprintf(stderr, "Command '%s' not found\n", argv[0]);
            exit(-1);
        }
    } else {
        if (pipeA[1] >= 0) {
            list_append(cL, &pipeA[1]);
        }
        if (pipeA[0] >= 0) {
            list_append(cL, &pipeA[0]);
        }
        int *cpidP = malloc(sizeof(int));
        *cpidP = cpid;
        list_append(tL, cpidP);
    }

    return 0;
}

int main(int argc, char *argv[], char *envp[]) {

    char str[1024];

    list_t *tL = list_init();
    list_t *cL = list_init();
    list_t *paras = list_init();

    printf("$ ");
    fflush(stdout);
    while (fgets(str, 1024, stdin) != NULL) {
        if (paras != NULL) {
            myParse(paras, str, envp);
        }
        if (strcmp(str, "exit\n") == 0) {
            list_finit(tL);
            exit(0);
        }

        processCmd(tL, paras, envp, -1, cL);

        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int*)cC->data));
            list_remove(cL, cC);
        }

            int cpid;
        while (tL->first != NULL) {
            cpid = wait(NULL);
            list_remove(tL, list_find(tL, &cpid, intPcmp));
        }

        fprintf(stdout, "$ ");
        fflush(stdout);
    }
}
