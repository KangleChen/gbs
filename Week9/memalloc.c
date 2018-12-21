#include "memalloc.h"
#include <stdlib.h>
#include <stdio.h>

double ceil(double number) {
    if (number < 1) {
        return 1.0;
    }
    return (double) ((unsigned int) (number + 0.5));
}

int mem_init(unsigned int size, unsigned int blocksize) {
    total_memory = size;
    int temp = blocksize;
    {
        extern unsigned int blocksize;
        blocksize = temp;
    }
    myMemory = list_init();
    memblock *firstBlock = calloc(1, sizeof(memblock));
    firstBlock->status = FREE;
    firstBlock->addr = 0;
    //size: ceiling(size/blocksize)
    double dSize = ceil(((double) size) / ((double) blocksize)) * (double) blocksize;
    firstBlock->size = (unsigned int) dSize;
    firstBlock->in_use = 0;

    list_append(myMemory, firstBlock);
    return 0;
}


void *mem_alloc(unsigned int bytesAllocated) { //3700
    double dBytesAllocated = ceil((double) bytesAllocated / (double) blocksize) * (double) blocksize;
    unsigned int numBytesAllocated = (unsigned int) dBytesAllocated; //4000
    list_elem *curr;
    memblock *data;

    //for WORST_FIT
    unsigned int maxVerschnitt = 0;
    list_elem *maxVerschnittElem = NULL;
    switch (myAlgo) {
        case FIRST_FIT:
            for (curr = myMemory->first; curr; curr = curr->next) {
                data = curr->data;  // 0x20 speichert 0x16
                if (data->status == USED)
                    continue;
                if (data->size == numBytesAllocated) {
                    data->status = USED;
                    data->in_use = bytesAllocated;
                    return data->addr;
                } //4000
                if (data->size > numBytesAllocated) { //5000 > 4000
                    memblock *left = calloc(1, sizeof(memblock));
                    left->status = USED;
                    left->addr = data->addr;
                    left->size = numBytesAllocated; //4000
                    left->in_use = bytesAllocated; //3700

                    memblock *right = calloc(1, sizeof(memblock));
                    right->status = FREE;
                    right->addr = left->addr + left->size; //4000
                    right->size = data->size - left->size; //1000
                    right->in_use = 0;

                    list_elem *new = calloc(1, sizeof(memblock));
                    new->next = curr->next;
                    new->data = right;

                    curr->data = left;
                    curr->next = new;

                    myMemory->count++;
                    return curr->data->addr;
                }

            }
            break;
        case WORST_FIT:
            for (curr = myMemory->first; curr; curr = curr->next) {
                if (curr->data->size < numBytesAllocated)
                    continue;
                if ((curr->data->size == bytesAllocated) && (curr->data->status == FREE)) {
                    curr->data->status = USED;
                    curr->data->in_use = bytesAllocated;
                    return curr->data->addr;
                }
                if ((curr->data->size > bytesAllocated) && (curr->data->status == FREE)) {
                    unsigned int myVerschnitt = curr->data->size - bytesAllocated;
                    if (myVerschnitt > maxVerschnitt) {
                        maxVerschnitt = myVerschnitt;
                        maxVerschnittElem = curr;
                    }
                }
            }
            if (maxVerschnittElem == NULL) {
                return NULL;
            }
            memblock *left = calloc(1, sizeof(memblock));
            left->status = USED;
            left->addr = maxVerschnittElem->data->addr;
            left->size = numBytesAllocated; //4000
            left->in_use = bytesAllocated; //3700

            memblock *right = calloc(1, sizeof(memblock));
            right->status = FREE;
            right->addr = left->addr + left->size; //4000
            right->size = maxVerschnittElem->data->size - left->size; //1000
            right->in_use = 0;

            list_elem *new = calloc(1, sizeof(memblock));
            new->next = maxVerschnittElem->next;
            new->data = right;

            maxVerschnittElem->data = left;
            maxVerschnittElem->next = new;

            myMemory->count++;
            return maxVerschnittElem->data->addr;
        default:
            return NULL;
    }
    return NULL;
}

void mem_free(void *addr) {
    memblock *myBlock = myMemory->first->data;
    //freiender Block steht am Anfang
    if (myBlock->addr == addr) {
        if (myMemory->count == 1) {
            myBlock->status = FREE;
            myBlock->in_use = 0;
            return;
        } else {
            if (myMemory->first->next->data->status == FREE) {
                memblock *myNachbar = myMemory->first->next->data;
                myBlock->status = FREE;
                myBlock->size += myNachbar->size;
                myBlock->in_use = 0;
                myMemory->first->next = myMemory->first->next->next;
                myMemory->count--;
                if (myMemory->count == 1)
                    myMemory->last = myMemory->first;
                return;
            } else if (myMemory->first->next->data->status == USED) {
                //B is the first but second is used
                myBlock->status = FREE;
                myBlock->in_use = 0;
                return;
            }
        }
    } else {
        //zu freiender Block steht nicht am Anfang
        for (list_elem *curr = myMemory->first; curr; curr = curr->next) {
            if (curr->next->data->addr != addr)
                continue;
            //now curr is the element direct before our address
            if (curr->data->status == USED) {
                if (curr->next->next == NULL) {
                    //Addr am Ende
                    curr = curr->next;
                    //now curr is our B
                    curr->data->status = FREE;
                    curr->data->in_use = 0;
                    myMemory->count--;
                    //B already am Ende, must not update last
                    return;
                } else {
                    //B in the middle
                    if (curr->next->next->data->status == FREE) {
                        curr = curr->next;
                        curr->data->status = FREE;
                        curr->data->in_use = 0;
                        curr->data->size += curr->next->data->size;
                        curr->next = curr->next->next;
                        myMemory->count--;
                        if (curr->next->next == NULL)
                            myMemory->last = curr;
                        return;
                    } else {
                        //If B in the middle and the two neighbors are occupied
                        curr = curr->next;
                        curr->data->status = FREE;
                        curr->data->in_use = 0;
                        return;
                    }
                }
            } else if (curr->data->status == FREE) {
                if (curr->next->next == NULL) {
                    //B ist am Ende
                    curr->data->size += curr->next->data->size;
                    curr->next = NULL;
                    myMemory->count--;
                    myMemory->last = curr;
                    return;
                } else {
                    //B in the middle
                    if (curr->next->next->data->status == USED) {
                        curr->data->size += curr->next->data->size;
                        curr->next = curr->next->next;
                        myMemory->count--;
                        return;
                    } else if (curr->next->next->data->status == FREE) {
                        //B in the middle of 2 free neighbors
                        curr->data->size += curr->next->data->size + curr->next->next->data->size;
                        curr->next = curr->next->next->next;
                        myMemory->count -= 2;
                        if (curr->next == NULL)
                            myMemory->last = curr;
                        return;
                    }
                }
            }
        }
    }
}

void print_memblock(memblock *block) {
    if (block->status == USED) {
        printf("[%p %c %d/%d] ", block->addr, 'P', block->in_use, block->size);
    } else if (block->status == FREE) {
        printf("[%p %c %d/%d] ", block->addr, 'F', block->in_use, block->size);
    }
}

void mem_dump() {
    list_print(myMemory, print_memblock);
}

