
#ifndef MEM_HEADER
#define MEM_HEADER

#include "list.h"

unsigned int total_memory;
unsigned int blocksize;
list_t *myMemory;
enum algorithm {
    FIRST_FIT, WORST_FIT
};
enum algorithm myAlgo;

typedef struct memblock memblock;
int mem_init (unsigned int size, unsigned int blocksize);

void *mem_alloc(unsigned int bytesAllocated);
void mem_free (void *addr);
void mem_dump ();

double ceil(double number);

#endif
