#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "list.h"

list_t *list_init() {
    list_t *list = malloc(sizeof(list_t));

    return list;
}

struct list_elem *list_insert(list_t *list, char *data) {
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

struct list_elem *list_append(list_t *list, char *data) {
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

int list_remove(list_t *list, struct list_elem *elem){
    struct list_elem *prev_list_elem = NULL;
    struct list_elem *current_list_elem = list->first;
    while(current_list_elem != elem){
        prev_list_elem = current_list_elem;
        current_list_elem = current_list_elem->next;
    }

    if(current_list_elem == NULL){
        return -1;
    }

    if(prev_list_elem != NULL){
        prev_list_elem->next = current_list_elem->next;
    } else {
        list->first = current_list_elem->next;
    }

    if(current_list_elem->next == NULL){
        list->last = prev_list_elem;
    }

    free(current_list_elem);

    return 0;

}

void list_finit(list_t *list){
    while(list->first != NULL){
        list_remove(list, list->last);
    }
    free(list);
}

struct list_elem *list_find(list_t *list, char *data, int (*cmp_elem)(const char *, const char *)){
    struct list_elem *current_list_elem = list->first;
    while(current_list_elem != NULL && (*cmp_elem)(current_list_elem->data, data)!=0){
        current_list_elem = current_list_elem->next;
    }
    return current_list_elem;
}

int numPlaces (int n) {
    if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

void list_print(list_t *list, void (*print_elem)(char *)){
    int i = 1;
    struct list_elem *current_list_elem = list->first;
    while(current_list_elem != NULL){
        char *out_str = malloc(strlen(current_list_elem->data)+numPlaces(i)+2);
        sprintf(out_str, "%d:%s", i, current_list_elem->data);
        (*print_elem)(out_str);
        current_list_elem = current_list_elem->next;
        i++;
    }
}

