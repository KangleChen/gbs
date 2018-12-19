#ifndef LIST_H
#define LIST_H

struct memblock {
    enum {
        FREE, USED
    } status;
    char *addr;
    unsigned int size;
    unsigned int in_use;
};

struct list_elem {
    struct list_elem *next;  // Zeiger auf das naechste Element
    struct memblock  *data;              // Zeiger auf ein Datenobject
};
typedef struct list_elem list_elem;
typedef struct memblock memblock;

typedef struct list {
    unsigned int count;
    struct list_elem *first;  // erstes Element in der Liste
    struct list_elem *last;   // letztes Element in der Liste
} list_t;

/* function prototypes */
list_t *list_init();

struct list_elem *list_insert(list_t *list,  memblock *data);

struct list_elem *list_append(list_t *list, memblock *data);

int list_remove(list_t *list, list_elem *elem);

void list_finit(list_t *list);

void list_removeAll(list_t *list);

void list_print(list_t *list, void (*print_elem)(memblock *));

struct list_elem *list_find(list_t *list, memblock *data,
                            int (*cmp_elem)(const memblock *, const memblock *));

char *list_toString(list_t *list);

int list_length(list_t *list);

void list_to_array(list_t* list, memblock *dataArray[]);

int list_updateCount(list_t *list);

#endif