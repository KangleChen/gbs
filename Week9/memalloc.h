
#ifndef MEM_HEADER
#define MEM_HEADER

int mem_init (unsigned int size, unsigned int blocksize);
void *mem_alloc (unsigned int size);
void mem_free (void *addr);
void mem_dump ();

struct memblock{
    enum {free, used} status;
    char *addr;
    unsigned int size;
    unsigned int in_use;
};

#endif
