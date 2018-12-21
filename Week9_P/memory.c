#include <stdlib.h>
#include "memory.h"

int mem_init(unsigned int size, unsigned int blocksize, enum mem_algo strategy){
    totalSize = size;
    blockSize = blocksize;
    mem_strategy = strategy;

    memoryList = list_init();

    memblock_t *initBlock = malloc(sizeof(memblock_t));
    initBlock->status = FREE;
    initBlock->addr = initBlock;
    initBlock->size = totalSize;
    initBlock->in_use = 0;

    list_append(memoryList, initBlock);
    return 0;
}

void *mem_alloc(unsigned int size){

}
