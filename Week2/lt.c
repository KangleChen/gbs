#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void print_string(char *data) {
    printf("%s\n", data);
}

int mainTest(int argc, char *argv[], char *envp[]) {
    list_t *li;
    if ((li = list_init()) == NULL) {
        perror(" Cannot allocate memory ");
        exit(-1);
    }

    if (list_append(li, "erstes") == NULL ||
        list_append(li, "zweites") == NULL ||
        list_append(li, "drittes") == NULL) {
        perror(" Cannot allocate memory ");
        exit(-1);
    }
    list_print(li, print_string);
    exit(0);
}

int main(int argc, char *argv[], char *envp[]) {
    list_t *list1;
    if ((list1 = list_init()) == NULL) {
        perror("Cannot allocate memory");
        exit(-1);
    }

    for (int i = 1; i < argc-1; ++i) {
        if (0 == strcasecmp(argv[i], "-a")) {
            list_append(list1, argv[i+1]);
        }
        if (0 == strcasecmp(argv[i], "-i")) {
            list_insert(list1, argv[i+1]);
        }
        if (0 == strcasecmp(argv[i], "-r")) {
            list_remove(list1, list_find(list1, argv[i+1], strcmp));
        }
    }

    list_print(list1, print_string);
    exit(0);
}