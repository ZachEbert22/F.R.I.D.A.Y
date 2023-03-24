//
// Created by Andrew Bowie on 3/24/23.
//

#ifndef F_R_I_D_A_Y_HEAP_H
#define F_R_I_D_A_Y_HEAP_H

#include "stddef.h"

/**
 * Initializes the heap with the given size.
 *
 * @param size the size of the new heap.
 */
void initialize_heap(size_t size);

/**
 * Frees the Memory Block at the given pointer.
 * @param pointer the address of the MB.
 * @return 0 on success 
 */
int free_memory(void* pointer);


#endif //F_R_I_D_A_Y_HEAP_H
