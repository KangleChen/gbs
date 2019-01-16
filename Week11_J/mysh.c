#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"
#include <errno.h>

char *currentFolder;

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


    int pipeA2[2];
    if (myParsePipe(cmdList, args2, pipeA2) >= 0) {
        int *cP = malloc(sizeof(int));
        *cP = pipeA2[0];
        list_append(cL, cP);
        cP = malloc(sizeof(int));
        *cP = pipeA2[1];
        list_append(cL, cP);
        processCmd(tL, args2, envp, pipeA2[0], cL);
    } else {
        pipeA2[0] = -1;
        pipeA2[1] = -1;
    }

    myParseStg2(cmdList, outFileP, inFileP);

    int numberArguments = list_length(cmdList);
    char *argv[list_length(cmdList) + 1];
    list_to_array(cmdList, (void **) argv);
    argv[list_length(cmdList)] = NULL;
    list_finit(cmdList);
    if (strcmp(argv[0], "cd") == 0 && numberArguments == 0 || strcmp(argv[0], "pwd") == 0) {
        printf("%s\n", currentFolder);
        return 0;
    } else if (strcmp(argv[0], "cd") == 0 && numberArguments > 2) {
        printf("Usage: cd <dir>\n");
        return 0;
    } else if (strcmp(argv[0], "cd") == 0 && numberArguments == 2) {
        int returnCode = chdir(argv[1]);
        if (returnCode != 0) {
            printf("cd: %s: %s\n", argv[1], strerror(errno));
        }
        getcwd(currentFolder, 1024);
        return 0;
    }

    if ((cpid = fork()) == 0) {

        char delimiter[] = ":";
        char *ptr;
        char *string = getenv("PATH");
        if (string == NULL) {
            fprintf(stderr, "No PATH variable");
            exit(-1);
        }

        if (pipeA2[1] > 0) {
            dup2(pipeA2[1], STDOUT_FILENO);
        }
        if (inPipe > 0) {
            dup2(inPipe, STDIN_FILENO);
        }

        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int*)cC->data));
            free(cC->data);
            list_remove(cL, cC);
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
                //ls  path=/usr/bin , ls /usr/bin/ls
                sprintf(cmd, "%s/%s", ptr, argv[0]);

                execve(cmd, argv, envp);

                // naechsten Abschnitt erstellen
                ptr = strtok(NULL, delimiter);
            }

            fprintf(stderr, "Command '%s' not found\n", argv[0]);
            exit(-1);
        }
    } else {
        int *cpidP = malloc(sizeof(int));
        *cpidP = cpid;
        list_append(tL, cpidP);
    }

    return 0;
}

int main(int argc, char *argv[], char *envp[]) {
    currentFolder = malloc(1024);
    getcwd(currentFolder, 1024);

    char str[1024];

    list_t *tL = list_init();
    list_t *cL = list_init();
    list_t *paras = list_init();

    printf("%s $", currentFolder);
    fflush(stdout);
    while (fgets(str, 1024, stdin) != NULL) {
        if (paras != NULL) {
            myParse(paras, str, envp);
        }
        if (strcmp(str, "exit\n") == 0) {
            list_finit(tL);
            list_finit(cL);
            exit(0);
        }

        processCmd(tL, paras, envp, -1, cL);


        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int*)cC->data));
            free(cC->data);
            list_remove(cL, cC);
        }


            int cpid;
        while (tL->first != NULL) {
            cpid = wait(NULL);
            list_remove(tL, list_find(tL, &cpid, intPcmp));
        }

        printf("%s $", currentFolder);
        fflush(stdout);
    }
}
