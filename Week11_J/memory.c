#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "memory.h"

int iCeil(double val){
    int temp = (int)val;
    if(val == (double)temp){
        return temp;
    }
    return temp+1;
}

int mem_init(unsigned int size, unsigned int blocksize, enum mem_algo strategy) {
    totalSize = size;
    blockSize = blocksize;
    mem_strategy = strategy;

    memoryList = list_init();

    memblock_t *initBlock = malloc(sizeof(memblock_t));
    initBlock->status = FREE;
    initBlock->addr = (char *)initBlock;
    initBlock->size = totalSize;
    initBlock->in_use = 0;

    list_append(memoryList, initBlock);
    return 0;
}

int getFreeBlockElemFirstFit(unsigned int size, struct list_elem **freeBlockP) {
    struct list_elem *prev = NULL, *curr = memoryList->first;
    for (; curr; prev = curr, curr = curr->next) {
        memblock_t *currData = (memblock_t *)curr->data;
        if (currData->status == FREE && currData->size >= size) {
            break;
        }
    }
    if (curr == NULL) return 0;
    if (prev == NULL) {
        *freeBlockP = curr;
        return 2;
    }
    *freeBlockP = prev;
    return 1;
}

int getFreeBlockElemWorstFit(unsigned int size, struct list_elem **freeBlockP) {
    struct list_elem *candPrev = NULL, *candidate = NULL;
    for (struct list_elem *prev = NULL, *curr = memoryList->first; curr; prev = curr, curr = curr->next) {
        memblock_t *currData = (memblock_t *)curr->data;
        if (currData->status == FREE && (candidate == NULL || currData->size > ((memblock_t *)candidate->data)->size)) {
            candidate = curr;
            candPrev = prev;
        }
    }
    if (candidate == NULL) return 0;
    if (candPrev == NULL) {
        *freeBlockP = candidate;
        return 2;
    }
    *freeBlockP = candPrev;
    return 1;
}

/*
 * freeBlockP has to be a pointer to a pointer to a list_elem struct
 *
 * returns -1 if error, 0 if no free block found,
 * 1 if found and freeBlockP is previous block elem to free block (so the free block is actually freeBlockP->next)
 * 2 if found and freeBlockP is the free block elem (because the free block has no previous list elem)
 */
int getFreeBlockElem(unsigned int size, struct list_elem **freeBlockP) {
    if (freeBlockP == NULL) return -1;
    switch (mem_strategy) {
        case FIRST_FIT:
            return getFreeBlockElemFirstFit(size, freeBlockP);
            break;
        case WORST_FIT:
            return getFreeBlockElemWorstFit(size, freeBlockP);
            break;
        default:
            return -1;
    }
}

void *mem_alloc(unsigned int size) {
    struct list_elem **blockPP = malloc(sizeof(struct list_elem *));
    int ret = getFreeBlockElem(size, blockPP);
    struct list_elem *blockP = *blockPP;
    free(blockPP);

    if (ret <= 0) return NULL;

    struct list_elem *prevBlock;
    memblock_t *freeBlock;
    if (ret == 1) {
        prevBlock = blockP;
        freeBlock = (memblock_t *)blockP->next->data;
        list_remove(memoryList, blockP->next);
    } else if (ret == 2) {
        prevBlock = NULL;
        freeBlock = (memblock_t *)blockP->data;
        list_remove(memoryList, blockP);
    } else {
        return NULL;
    }

    int blockCount = iCeil((double) size / blockSize);

    memblock_t *newBlock = calloc(1, sizeof(memblock_t));
    newBlock->status = USED;
    newBlock->addr = freeBlock->addr;
    newBlock->size = blockCount * blockSize;
    newBlock->in_use = size;

    struct list_elem *newBlockElem = list_insert_after(memoryList, prevBlock, newBlock);

    int freeSizeRemain = freeBlock->size - newBlock->size;
    free(freeBlock);

    if (freeSizeRemain > 0) {
        memblock_t *newFreeBlock = calloc(1, sizeof(memblock_t));
        newFreeBlock->status = FREE;
        newFreeBlock->addr = newBlock->addr + newBlock->size;
        newFreeBlock->size = freeSizeRemain;
        newFreeBlock->in_use = 0;

        list_insert_after(memoryList, newBlockElem, newFreeBlock);
    }

    return newBlock->addr;

}

void mem_free(void *addr) {
    struct list_elem *prev = NULL, *curr;
    for (curr = memoryList->first; curr; prev = curr, curr = curr->next) {
        if (((memblock_t *)curr->data)->addr - (char *)addr == 0) {
            break;
        }
    }

    memblock_t *currData = (memblock_t *)curr->data;
    if (curr == NULL || currData->status == FREE) return;

    currData->status = FREE;
    currData->in_use = 0;

    if (prev != NULL && ((memblock_t *)prev->data)->status == FREE) {
        ((memblock_t *)prev->data)->size += currData->size;
        free(curr->data);
        list_remove(memoryList, curr);
        curr = prev;
    }

    currData = (memblock_t *)curr->data;

    if (curr->next != NULL && ((memblock_t *)curr->next->data)->status == FREE) {
        currData->size += ((memblock_t *)curr->next->data)->size;
        free(curr->next->data);
        list_remove(memoryList, curr->next);
    }

}

void dump_block(void *block){
    memblock_t *memBlock = (memblock_t *)block;
    printf("[%p %c %d/%d] ", memBlock->addr, memBlock->status, memBlock->in_use, memBlock->size);
}

void mem_dump(){
    list_print(memoryList, dump_block);
    printf("\n");
}