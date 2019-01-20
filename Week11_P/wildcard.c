#include <fnmatch.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "list.h"

#include "wildcard.h"

char *wildcardStr;

int scanFilter(const struct dirent *dirEnt) {
    if (strcmp(dirEnt->d_name, ".") == 0 || strcmp(dirEnt->d_name, "..") == 0) {
        return 0;
    }
    if (fnmatch(wildcardStr, dirEnt->d_name, 0) == 0) {
        return 1;
    }
    return 0;

}

void expandWildcards(list_t *paras) {
    struct list_elem *curr = paras->first;
    for (int i = 0; curr != NULL; i++) {
        char *string = (char *) curr->data;
        if (strchr(string, '*') != NULL) {

            int lastSlash = -1;
            int wildcard = -1;
            int nextSlash = -1;

            for (int j = 0; j < strlen(string); ++j) {
                if (string[j] == '/') {
                    if (wildcard < 0) {
                        lastSlash = j;
                    } else {
                        if (nextSlash < 0) {
                            nextSlash = j;
                            break;
                        }
                    }
                } else if (string[j] == '*') {
                    wildcard = j;
                }
            }
            // bla/bl*aet/tr.ml
            char *prefix;
            if (lastSlash < 0) {
                prefix = calloc(1, sizeof(char));
            } else {
                prefix = calloc(lastSlash + 1, sizeof(char));
                strncpy(prefix, string, lastSlash);
            }

            char *infix;
            int inL;
            if (nextSlash < 0) {
                inL = strlen(string) - (lastSlash + 1);
                infix = calloc(inL + 1, sizeof(char));
            } else {
                inL = nextSlash - (lastSlash + 1);
                infix = calloc(inL + 1, sizeof(char));
            }
            strncpy(infix, string + lastSlash + 1, inL);

            char *suffix;
            if (nextSlash < 0) {
                suffix = calloc(1, sizeof(char));
            } else {
                int sufL = strlen(string) - (nextSlash + 1);
                suffix = calloc(sufL + 1, sizeof(char));
                strncpy(suffix, string + nextSlash + 1, sufL);
            }

            wildcardStr = infix;

            struct dirent **namelist;
            int n;

            n = scandir(strlen(prefix)==0?".":prefix, &namelist, scanFilter, alphasort);
            if (n < 0) {
                // ERROR
            } else {
                while (n--) {
                    char *tempStr = calloc(strlen(namelist[n]->d_name) + strlen(prefix) + strlen(suffix) + 1,
                                           sizeof(char));
                    sprintf(tempStr, "%s%s%s%s%s", prefix, strlen(prefix) == 0?"":"/" , namelist[n]->d_name, strlen(suffix) == 0?"":"/" , suffix);

                    /*
                    char *realPath = calloc(4096, sizeof(char));
                    realpath(tempStr, realPath);
                    free(tempStr);
                    char *finalStr = calloc(strlen(realPath) + 1, sizeof(char));
                    strcpy(finalStr, realPath);
                    free(realPath);
                    */

                    struct stat s;
                    int err = stat(tempStr, &s);
                    if (err != -1) {
                        list_insert_after(paras, curr, tempStr);
                    } else {
                        free(tempStr);
                    }

                    free(namelist[n]);
                }
                free(namelist);
            }
            struct list_elem *next = curr->next;
            free(string);
            list_remove(paras, curr);
            curr = next;
        } else {
            curr = curr->next;
        }
    }
}