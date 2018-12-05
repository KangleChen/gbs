#include "list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BIT_MAN
#define BIT_MAN
#define _BS(x, n) (x | (1UL << n));
#define _BC(x, n) (x & (~(1UL << n)));
#define _BT(x, n) (x ^ (1UL << n));
#define _CB(x, n) ((x >> n) & 1U);
#endif

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

int myParseStg2(list_t *args, char **outFile, char *inFile) {
    int retVal = 0;
    int delCurrCount = 0;
    for (struct list_elem *curr = list->first; curr; curr = curr->next) {
        if (strcmp((char *) curr->data, ">") == 0) {
            if (curr->next != NULL) {
                char *outFileT = (char *) curr->next->data;
                *outFile = calloc(strlen(outFileT) + 1, sizeof(char));
                strcpy(*outFile, outFileT);
                if (strcmp(*outFile, outFileT) == 0) {
                    retVal = _BS(retVal, 0);
                    delCurrCount = 2;
                } else {
                    retVal = -1;
                    break;
                }
            } else {
                retVal = -1;
                break;
            }
        } else if (strcmp((char *) curr->data, "<") == 0) {
            if (curr->next != NULL) {
                char *inFileT = (char *) curr->next->data;
                *inFile = calloc(strlen(inFileT) + 1, sizeof(char));
                strcpy(*inFile, inFileT);
                if (strcmp(*inFile, inFileT) == 0) {
                    retVal = _BS(retVal, 1);
                    delCurrCount = 2;
                } else {
                    retVal = -1;
                    break;
                }
            } else {
                retVal = -1;
                break;
            }
        }

        while (delCurrCount > 0) {
            struct list_elem *currT = curr;
            curr = curr->next;
            list_remove(args, currT);
            delCurrCount--;
        }
    }
    return retVal;
}