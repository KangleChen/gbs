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

    int intPcmp(const void *intP1, const void *intP2){
    return (*(int *)intP1) - (*(int *)intP2);
}

int main (int argc, char *argv [], char *envp []){

    char str[1024];

    list_t *tL = list_init();
    list_t *paras = list_init();

    char **outFileP = malloc(sizeof(char*));
    char **inFileP = malloc(sizeof(char*));

    printf("$ ");
    fflush(stdout);
    while(fgets(str, 1024, stdin) != NULL) {
        if(paras != NULL){
            myParse(paras, str, envp);
            myParseStg2(paras, outFileP, inFileP);
        }
        if(strcmp(str, "exit\n") == 0){
            exit(0);
        }

        int cpid;

        if ((cpid = fork()) == 0) {
            char *argv[list_length(paras)+1];
            list_to_array(paras, (void **)argv);
            argv[list_length(paras)] = NULL;

            char delimiter[] = ":";
            char *ptr;
            char *string = getenv("PATH");
            if(string == NULL){
                fprintf(stderr, "No PATH variable");
                exit(-1);
            }

            int fd = -1;

            if(*outFileP != NULL && (fd = open(*outFileP, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) >= 0){
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            if(*inFileP != NULL && (fd = open(*inFileP, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) >= 0){
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if(strchr(argv[0], '/') != NULL){
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
            int *cpidP = malloc(sizeof(int));
            *cpidP = cpid;
            list_append(tL, cpidP);
        }

        while (tL->first != NULL) {
            cpid = wait(NULL);
            list_remove(tL, list_find(tL, &cpid, intPcmp));
        }

        fprintf(stdout, "$ ");
        fflush(stdout);
    }
}