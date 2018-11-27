#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "list.h"

int intPcmp(const void *intP1, const void *intP2){
    return (*(int *)intP1) - (*(int *)intP2);
}

int main (int argc, char *argv [], char *envp []){

    char str[1024];

    list_t *tL = list_init();
    list_t *paras = list_init();

    printf("$ ");
    fflush(stdout);
    while(fgets(str, 1024, stdin) != NULL) {
        if(paras != NULL){
            myParse(paras, str, envp);
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

            if(strchr(argv[0], '/') != NULL){
                execve(argv[0], argv, envp);
            } else {

                ptr = strtok(string, delimiter);

                while (ptr != NULL) {
                    char cmd[strlen(ptr) + strlen(argv[0]) + 1];
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