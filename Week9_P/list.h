#ifndef LIST_H
#define LIST_H

#include "memory.h"

struct list_elem {
    struct list_elem *next;  // Zeiger auf das naechste Element
    memblock_t *data;              // Zeiger auf ein Datenobject
};
typedef struct list_elem list_elem;

typedef struct list {
    unsigned int count;
    struct list_elem *first;  // erstes Element in der Liste
    struct list_elem *last;   // letztes Element in der Liste
} list_t;

/* function prototypes */
list_t *list_init();

struct list_elem *list_insert(list_t *list, memblock_t *data);

struct list_elem *list_append(list_t *list, memblock_t *data);

int list_remove(list_t *list, struct list_elem *elem);

void list_finit(list_t *list);

void list_removeAll(list_t *list);

void list_print(list_t *list, void (*print_elem)(char *));

struct list_elem *list_find(list_t *list, void *data,
                            int (*cmp_elem)(const memblock_t *, const memblock_t *));

char *list_toString(list_t *list);

int list_length(list_t *list);

void list_to_array(list_t* list, memblock_t *dataArray[]);

int list_updateCount(list_t *list);

#endif