#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

extern list_t *myParse(list_t *res, char *str, char *envp[]);

void printString(char *data) {
    printf("%s\n", data);
}

int main (int argc, char *argv [], char *envp []){

    char str[1024];

    printf("$ ");
    list_t *paras = list_init();
    while(fgets(str, 1024, stdin) != NULL) {
        if(paras != NULL){
            myParse(paras, str, envp);
            list_print(paras, printString);
        }
        if(strcmp(str, "exit\n") == 0){
            exit(0);
        }
        printf("$ ");
    }
}