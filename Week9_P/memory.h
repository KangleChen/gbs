#ifndef MEMORY_H
#define MEMORY_H

#include "list.h"

unsigned int totalSize;
unsigned int blockSize;

enum mem_algo {
    NONE, FIRST_FIT, WORST_FIT
};
enum mem_algo mem_strategy;

typedef struct memblock {
    enum {
        FREE, USED
    } status;
    char *addr;
    unsigned int size;
    unsigned int in_use;
} memblock_t;

list_t *memoryList;


int mem_init(unsigned int size, unsigned int blocksize, enum mem_algo strategy);

void *mem_alloc(unsigned int size);

void mem_free(void *addr);

void mem_dump();


#endif
