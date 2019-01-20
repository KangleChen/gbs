#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include "list.h"
#include "wildcard.h"

char internalPrefixFormat[] = "/>%06d#internal</";
// snprintf(NULL, 0, internalPrefixFormat, 0) + 1
char internalPrefix[20];

char *pwd;

list_t *tL;
list_t *cL;
list_t *paras;

// parser.c function declarations
extern list_t *myParse(list_t *res, char *str, char *envp[]);

extern int myParseStg2(list_t *args, char **outFileP, char **inFileP);

extern int myParsePipe(list_t *args, list_t *args2, int pipeA[]);
// END

// Compares two pointers like ints.
int intPcmp(const void *intP1, const void *intP2) {
    return (*(int *) intP1) - (*(int *) intP2);
}

void cleanExit(int status) {
    list_finit(tL);
    list_finit(cL);
    list_finit(paras);
    free(pwd);
    exit(status);
}

int changeDir(list_t *cmdList) {
    int argc = list_length(cmdList);
    if (argc == 1) {
        printf("%s\n", pwd);
    } else if (argc == 2) {
        char *dir = (char *)cmdList->first->next->data;
        char newPwd[strlen(pwd) + strlen(dir) + 2];
        sprintf(newPwd, "%s/%s", pwd, dir);
        char realNewPwd[4096];
        realpath(newPwd, realNewPwd);
        //printf("RealNewPwd: %s\n", realNewPwd);
        struct stat s;
        int err = stat(newPwd, &s);
        if (err != -1 && S_ISDIR(s.st_mode)) {
            strcpy(pwd, realNewPwd);
            //printf("pwd: %s\n", pwd);
            setenv("PWD", pwd, 1);
            chdir(pwd);
        } else {
            printf("cd: %s: %s\n", dir, strerror(ENOENT));
        }
    } else {
        printf("Usage: cd <dir>\n");
    }
    return 0;
}

int execCmd(const char *filename, char *const argv[], char *const envp[]) {
    /*
    if (strncmp(filename, internalPrefix, strlen(internalPrefix)) == 0) {
        if (strncmp(filename + strlen(internalPrefix), "/cd", 3) == 0) {
            return changeDir(argv, envp);
        }
    }
    */
    return execve(filename, argv, envp);
}


int processCmd(list_t *tL, list_t *cmdList, char *envp[], int inPipe, list_t *cL) {
    char **outFileP = malloc(sizeof(char *));
    char **inFileP = malloc(sizeof(char *));
    int cpid;

    if(list_length(cmdList) > 0 && strncmp((char *)cmdList->first->data, "cd", 2) == 0){
        return changeDir(cmdList);
    }

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
            cleanExit(-1);
        }

        /*
        char string[strlen(internalPrefix) + strlen(path) + 2];
        sprintf(string, "%s:%s", internalPrefix, path);
        */

        if (pipeA2[1] > 0) {
            dup2(pipeA2[1], STDOUT_FILENO);
        }
        if (inPipe > 0) {
            dup2(inPipe, STDIN_FILENO);
        }

        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int *) cC->data));
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
            execCmd(argv[0], argv, envp);
        } else {

            ptr = strtok(string, delimiter);

            while (ptr != NULL) {
                char cmd[strlen(ptr) + strlen(argv[0]) + 2];
                sprintf(cmd, "%s/%s", ptr, argv[0]);

                execCmd(cmd, argv, envp);

                // naechsten Abschnitt erstellen
                ptr = strtok(NULL, delimiter);
            }

            fprintf(stderr, "Command '%s' not found\n", argv[0]);
            cleanExit(-1);
        }
    } else {
        int *cpidP = malloc(sizeof(int));
        *cpidP = cpid;
        list_append(tL, cpidP);
    }

    return 0;
}

int main(int argc, char *argv[], char *envp[]) {

    srand(time(NULL));
    sprintf(internalPrefix, internalPrefixFormat, rand() % 900000 + 100000);

    char str[1024];

    tL = list_init();
    cL = list_init();
    paras = list_init();

    pwd = calloc(4096, sizeof(char));
    //printf("pwd size: %d\n", (int) (sizeof(pwd) / sizeof(pwd[0])));
    if (getcwd(pwd, 4096) == NULL) {
        sprintf(pwd, "/");
    }

    printf("%s $ ", pwd);
    fflush(stdout);
    while (fgets(str, 1024, stdin) != NULL) {
        if (paras != NULL) {
            myParse(paras, str, envp);
        }
        if (strcmp(str, "exit\n") == 0) {
            cleanExit(0);
        }

        expandWildcards(paras);

        processCmd(tL, paras, envp, -1, cL);


        while (cL->first != NULL) {
            struct list_elem *cC = cL->first;
            close(*((int *) cC->data));
            free(cC->data);
            list_remove(cL, cC);
        }


        int cpid;
        while (tL->first != NULL) {
            cpid = wait(NULL);
            list_remove(tL, list_find(tL, &cpid, intPcmp));
        }

        fprintf(stdout, "%s $ ", pwd);
        fflush(stdout);
    }
}
