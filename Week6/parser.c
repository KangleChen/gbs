#include "list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addParam(list_t *params, list_t *buffer) {
    if (list_length(buffer) > 0) {
        list_append(params, list_toString(buffer));
        list_removeAll(buffer);
    }
}

void addVar(list_t *buffer, list_t *varBuffer) {
    if (list_length(varBuffer) <= 0) {
        return;
    }
    char *envVar = getenv(list_toString(varBuffer));
    list_removeAll(varBuffer);
    if (envVar == NULL) {
        return;
    }

    char *cP;
    for (int i = 0; i < strlen(envVar); ++i) {
        cP = malloc(sizeof(char));
        *cP = envVar[i];
        list_append(buffer, cP);
    }

}

list_t *myParse(list_t *res, char *str, char *envp[]) {

    //list_t *res = list_init();
    list_removeAll(res);
    //printf("myParse res: %p", res);
    list_t *buf = list_init();
    list_t *varBuf = list_init();

    char encChar = 0;
    bool isVar = false;
    bool isEscaped = false;

    int i;
    char c;
    char *cP;
    for (i = 0, c = str[i]; c != 0; i++, c = str[i]) {
        if (c == '\n') {
            break;
        }

        cP = malloc(sizeof(char));
        *cP = c;
        //printf("c: %c\n", *cP);
        if (isVar) {
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '_') {
                list_append(varBuf, cP);
                continue;
            } else {
                addVar(buf, varBuf);
                isVar = false;
            }
        }

        if (isEscaped) {
            list_append(buf, cP);
            isEscaped = false;
        } else if (encChar != 0) {
            if (c == encChar) {
                encChar = 0;
            } else {
                if (c == '$') {
                    isVar = true;
                } else if (c == '\\') {
                    isEscaped = true;
                } else {
                    list_append(buf, cP);
                }
            }
        } else {
            switch (c) {
                case '"':
                case '\'':
                    encChar = c;
                    break;
                case '$':
                    isVar = true;
                    break;
                case '\\':
                    isEscaped = true;
                    break;
                case ' ':
                    addParam(res, buf);
                    break;
                default:
                    list_append(buf, cP);
            }

        }
    }

    if (isVar) {
        addVar(buf, varBuf);
    }

    /*
    if (encChar != 0) {

    } else if (isEscaped) {

    } else {

    }
    */
    addParam(res, buf);

    list_finit(buf);
    list_finit(varBuf);

    return res;
}