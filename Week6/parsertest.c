#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "parser.c"

int main (int argc, char *argv [], char *envp []){

    char str[1024];

    while(fgets(str, 1024, stdin) != NULL) {
      if(strcmp(str, "exit\n") !=0){
          abort();
      }
      list_t *paras = parse(str, envp);
      if(paras != NULL){
          list_print(paras, printString);
      }
      printf("$ ");
    }
}