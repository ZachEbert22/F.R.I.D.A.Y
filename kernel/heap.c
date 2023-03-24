//
// Created by Andrew Bowie on 3/24/23.
//

#include "mpx/heap.h"
#include "linked_list.h"
#include "stddef.h"
#include "mpx/vm.h"

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

linked_list *free_list;
linked_list *alloc_list;

void initialize_lists()
{
    //Set the lists.
    static linked_list free_list_stc = {0};
    static linked_list alloc_list_stc = {0};
    free_list = &free_list_stc;
    alloc_list = &alloc_list_stc;
}

void initialize_heap(size_t size)
{
    initialize_lists();

    //Malloc the full free block.
    mem_block_t *block = kmalloc(size, 0, NULL);
    add_item(free_list, block);

    //Initialize the values of the block.
    block->size = size - sizeof (struct mem_block);
    block->start_address = (int) (((int) block) + sizeof (struct mem_block));
}