#include "list.h"
#include <stdbool.h>


list_t *parse(char *str, char *envp[]){

    list_t *res = list_init();
    list_t *buf = list_init();
    list_t *varBuf = list_init();

    char encChar = 0;
    bool isVar = false;
    bool isEscaped = false;

    int i;
    char c;
    for (i = 0, c = str[i]; c != 0; i++, c = str[i]){
        if(encChar!=0){
            if(c == encChar){
                encChar = 0;
            } else {
                list_append(buf, &c);
            }
        } else {
            switch (c){
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
                    list_append(res, list_toString(buf));
                    list_removeAll(buf);
                default:
                    list_append(buf, &c);
            }

        }

    }

    return res;
}