//
// Created by Andrew Bowie on 9/18/22.
//

#include "linked_list.h"
#include <stddef.h>
#include "memory.h"
#include "string.h"
#include "stdio.h"
#include "mpx/pcb.h"

///Contains the structure of the nodes in our linked list.
struct linked_list_node_
{
    ///The pointer to the item we're storing.
    void *_item; //8 bytes
    ///The next node in the list.
    ll_node *_next; //8 bytes
};

///Contains the definition of our linked list.
struct linked_list_
{
    ///The size of the linked list.
    int _size;
    ///The maximum size of the linked list, set to -1 for infinite.
    int _max_size;
    ///A pointer to the sorting function.
    int (*sort_func)(void*, void*);
    ///The first node in the linked list.
    ll_node *_first;
    ///The second node in the linked list.
    ll_node *_last;
};

/**
 * @brief Sets the item in the given list to the value given.
 * @param list the list to set the item in.
 * @param index the index to set at.
 * @param new_item the new item.
 * @return the old item, NULL if something failed.
 */
void
*set_item(linked_list *list, int index, void *new_item);

linked_list
*nl_unbounded(void)
{
    return nl_maxsize(-1);
}

linked_list
*nl_maxsize(int max_size)
{
    //Create and assign the max size.
    linked_list *created_ptr = sys_alloc_mem(sizeof(linked_list));

    //No size allowed.
    if (created_ptr == NULL)
        return NULL;

    memset(created_ptr, 0, sizeof(linked_list));
    created_ptr->_max_size = max_size;
    created_ptr->_first = created_ptr->_last = NULL;
    return created_ptr;
}

int
list_size(linked_list *list)
{
    return list->_size;
}

void
*get_item(linked_list *list, int index)
{
    if (list == NULL)
        return NULL;

    //Bounds check the index.
    if (index < 0 || index >= list->_size)
        return NULL;

    int walk_index = 0;
    ll_node *first = list->_first;
    while (first != NULL)
    {
        //If we've reached the index, return the item.
        if (index == walk_index)
            return first->_item;

        //Step forward.
        first = first->_next;
        walk_index++;
    }
    return NULL;
}

int
add_item_i(linked_list *list, int item)
{
    return add_item_index_i(list, list->_size, item);
}

int
add_item_index_i(linked_list *list, int index, const int item)
{
    //Create the new integer pointer.
    int *new_ptr = sys_alloc_mem(sizeof (item));

    //We weren't able to allocate the memory.
    if(new_ptr == NULL)
        return 0;

    //Copy the value.
    *new_ptr = item;

    return add_item_index(list, index, new_ptr);
}

int
add_item(linked_list *list, void *item)
{
    return add_item_index(list, list->_size, item);
}

int
add_item_index(linked_list *list, int index, void *item)
{
    if (list == NULL)
        return 0;

    //Bounds check the index.
    if (index < 0 || index > list->_size)
        return 0;

    //List is full.
    if(list->_max_size >= 0 && list->_max_size <= list->_size)
        return 0;

    //Create the node and assign the values.
    ll_node *created = sys_alloc_mem(sizeof(ll_node));

    //We were not able to allocate the memory required.
    if(created == NULL)
        return 0;

    created->_item = item;

    //We need to find the proper index to add the item in accordance to the sort function.
    if(list->sort_func != NULL)
    {
        ll_node *curr_next = list->_first;
        int walk = 0;
        while(curr_next != NULL)
        {
            if(list->sort_func(curr_next->_item, created->_item) < 0)
                break;

            curr_next = curr_next->_next;
            walk++;
        }

        index = walk;
    }

    int walk_index = 0;
    ll_node *first = list->_first;
    ll_node *last_iterated = NULL;
    //Check if we can simply jump to the end of the list.
    if(index == list->_size)
    {
        first = NULL;
        last_iterated = list->_last;
        walk_index = index;
    }
    else
    {
        while (first != NULL && walk_index != index)
        {
            //If we haven't reached the index, move the pointer forward.
            last_iterated = first;
            first = first->_next;
            walk_index++;
        }
    }

    created->_next = first;

    //Update the next node to have us as previous.
    if(last_iterated != NULL)
    {
        last_iterated->_next = created;
    }

    //Place it into the list, if necessary.
    if(walk_index == 0)
        list->_first = created;

    //Assign last if necessary.
    if(walk_index == list->_size)
        list->_last = created;

    list->_size++;

    //Success!
    return 1;
}

void
set_item_i(linked_list *list, int index, int new_item)
{
    //Create the new integer pointer.
    int *new_ptr = sys_alloc_mem(sizeof (new_item));

    //Copy the value.
    *new_ptr = new_item;

    //Free the old ptr.
    void *old = set_item(list, index, new_ptr);
    sys_free_mem(old);
}

void
*set_item_i_unsafe(linked_list *list, int index, int new_item)
{
    //Create the new integer pointer.
    int *new_ptr = sys_alloc_mem(sizeof (new_item));

    //Copy the value.
    *new_ptr = new_item;
    return set_item(list, index, new_ptr);
}

void
*set_item(linked_list *list, int index, void *new_item)
{
    if (list == NULL)
        return NULL;

    //Bounds check the index.
    if (index < 0 || index >= list->_size)
        return NULL;

    int walk_index = 0;
    ll_node *first = list->_first;
    if(index == list->_size)
    {
        first = list->_last;
        walk_index = index;
    }
    else
    {
        while (first != NULL && walk_index != index)
        {
            //If we haven't reached the index, move the pointer forward.
            first = first->_next;
            walk_index++;
        }
    }

    //We were nt able to find the index.
    if(first == NULL)
        return NULL;

    //Assign the item.
    void *old_item = first->_item;
    first->_item = new_item;

    //Success!
    return old_item;
}

void
remove_item(linked_list *list, int index)
{
    if(list == NULL)
        return;

    void *to_free = remove_item_unsafe(list, index);

    //Free the memory.
    if(to_free != NULL)
        sys_free_mem(to_free);
}

void
*remove_item_unsafe(linked_list *list, int index)
{
    if (list == NULL)
        return NULL;

    //Bounds check the index.
    if (index < 0 || index >= list->_size)
        return NULL;

    int walk_index = 0;
    ll_node *first = list->_first;
    ll_node *last_iterated = NULL;
    while (first != NULL && walk_index != index)
    {
        //If we haven't reached the index, move the pointer forward.
        last_iterated = first;
        first = first->_next;
        walk_index++;
    }

    //Update the list.
    if(first == NULL)
        return NULL;

    //Remove ourselves from the previous node.
    if(last_iterated != NULL)
        last_iterated->_next = first->_next;

    //Ensure the first pointer is correct.
    if(walk_index == 0)
        list->_first = first->_next;
    else if(walk_index == list->_size)
        list->_last = last_iterated;

    list->_size--;

    void *item = first->_item;

    //Free the pointer to the node.
    sys_free_mem(first);
    first = NULL;

    //Success!
    return item;
}

void
for_each_il(linked_list *list, void call(void *call))
{
    ll_node *first_node = list->_first;

    while(first_node != NULL)
    {
        call(first_node->_item);
        first_node = first_node->_next;
    }
}

void
set_sort_func(linked_list *list, int sort_func(void *, void *))
{
    list->sort_func = sort_func;
}

void
destroy_list(linked_list *to_destroy_ptr, int destroy_values)
{
    if (to_destroy_ptr == NULL)
        return;

    ll_node *nodes[to_destroy_ptr->_size];

    //First go through all nodes and free them.
    ll_node *first_ptr = to_destroy_ptr->_first;
    int index = 0;
    while (first_ptr != NULL)
    {
        nodes[index] = first_ptr;
        if (destroy_values)
        {
            sys_free_mem(first_ptr->_item);
        }

        //Step the pointer forward, then free the old one.
        ll_node *temporary = first_ptr;
        first_ptr = first_ptr->_next;
        sys_free_mem(temporary);
        index++;
    }

    sys_free_mem(to_destroy_ptr);
}