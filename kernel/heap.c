//
// Created by Andrew Bowie on 3/24/23.
//

#include "mpx/heap.h"
#include "stddef.h"

///A structure that contains memory.
typedef struct mem_block {
    ///The previous memory block.
    struct mem_block *prev;
    ///The next memory block.
    struct mem_block *next;

    ///The start address of this block
    int start_address;
    ///The size of this block
    size_t size;
} mem_block_t;



void initialize_heap(size_t size)
{

}