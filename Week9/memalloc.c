#include "memalloc.h"

int mem_init (unsigned int size, unsigned int blocksize);
void *mem_alloc (unsigned int size);
void mem_free (void *addr);
void mem_dump ();

