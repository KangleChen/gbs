#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "list.h"

list_t *list_init() {

    list_t *list = calloc(1, sizeof(list_t));

    return list;
}

struct list_elem *list_insert(list_t *list, void *data) {
    struct list_elem *list_elem1 = malloc(sizeof(struct list_elem));
    if (list_elem1 == NULL) {
        return NULL;
    }

    list_elem1->next = list->first;
    list_elem1->data = data;
    list->first = list_elem1;
    if (list->last == NULL) {
        list->last = list_elem1;
    }

    return list_elem1;
}

struct list_elem *list_append(list_t *list, void *data) {
    struct list_elem *list_elem1 = malloc(sizeof(struct list_elem));
    if (list_elem1 == NULL) {
        return NULL;
    }

    list_elem1->data = data;
    list_elem1->next = NULL;

    if (list->last != NULL) {
        list->last->next = list_elem1;
    } else {
        list->first = list_elem1;
    }

    list->last = list_elem1;

    return list_elem1;
}

int list_remove(list_t *list, struct list_elem *elem) {
    for (struct list_elem *prev = NULL, *curr = list->first; curr; prev = curr, curr = curr->next) {
        if (curr == elem) {
            if (curr == list->first) {
                list->first = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == list->last) {
                list->last = prev;
            }
            free(curr);
            return 0;
        }
    }
    return -1;
}

void list_finit(list_t *list) {
    while (list->first != NULL) {
        list_remove(list, list->last);
    }
    free(list);
}

struct list_elem *list_find(list_t *list, void *data, int (*cmp_elem)(const void *, const void *)) {
    struct list_elem *current_list_elem = list->first;
    while (current_list_elem != NULL && (*cmp_elem)(current_list_elem->data, data) != 0) {
        current_list_elem = current_list_elem->next;
    }
    return current_list_elem;
}

void list_print(list_t *list, void (*print_elem)(void *)) {
    struct list_elem *curr = list->first;
    for (int i = 1; curr != NULL; i++) {
        printf("%d:", i);
        (*print_elem)(curr->data);
        curr = curr->next;
    }
}
