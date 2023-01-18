//
// Created by Andrew Bowie on 9/18/22.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct linked_list_node_ ll_node;
typedef struct linked_list_ linked_list;

/**
 * @brief Creates a new unbounded linked list. You should call {@code destroy(list, 1)} when finished with it.
 * @return a pointer to the new linked list.
 */
linked_list
*nl_unbounded(void);

/**
 * @brief Creates a new bounded linked list. You should call {@code destroy(list, 1)} when finished with it.
 * @return a pointer to the new linked list.
 */
linked_list
*nl_maxsize(int max_size);

/**
 * @brief Gets the size of the linked list.
 * @return the size of the linked list.
 */
int
list_size(linked_list *list);

/**
 * @brief Gets the item at the given index in the list, or returns NULL
 * if the index was invalid.
 * @param list the list.
 * @param index the index.
 * @return the item, or NULL.
 */
void *
get_item(linked_list *list, int index);

/**
 * @brief Destroys the linked list by freeing any memory associated with it.
 * @param list the list to destroy.
 * @param destroy_values 1 if the values should be freed as well.
 */
void
destroy_list(linked_list *list, int destroy_values);

/**
 * @brief Adds an item to the end of the linked list.
 * @param list the list to add to.
 * @param item the item to add.
 * @return 1 if successful, 0 if the item was not added.
 */
int
add_item_i(linked_list *list, int item);

/**
 * @brief Adds an item to the end of the linked list.
 * @param list the list to add to.
 * @param index the index to add the item to.
 * @param item the item to add.
 * @return 1 if successful, 0 if the item was not added.
 */
int
add_item_index_i(linked_list *list, int index, int item);

/**
 * @brief An internal method for adding an unknown type to the list.
 * @param list the list to add to.
 * @param index the index to add to.
 * @param item the item to add.
 * @return 1 if successful, 0 if not.
 */
int
add_item_index(linked_list *list, int index, void *item);

/**
 * @brief Sets the value of the item at an index within the list.
 * @param list the list to set in.
 * @param index the index to set at.
 * @param new_item the item to set to.
 * @return the previous value, or NULL if the operation was unsuccessful.
 */
void
set_item_i(linked_list *list, int index, int new_item);

/**
 * @brief Sets the value of the item at an index within the list.
 * YOU are responsible for freeing this pointer if used!
 * @param list the list to set in.
 * @param index the index to set at.
 * @param new_item the item to set to.
 * @return the previous value, or NULL if the operation was unsuccessful.
 */
void
*set_item_i_unsafe(linked_list *list, int index, int new_item);

/**
 * @brief Removes an item from the linked list at the given index.
 * @param list the list to remove from.
 * @param index the index to remove at.
 * @return the item removed, or NULL if unsuccessful.
 */
void
remove_item(linked_list *list, int index);

/**
 * @brief Removes an item from the linked list at the given index.
 * YOU are responsible for freeing this pointer if used!
 * @param list the list to remove from.
 * @param index the index to remove at.
 * @return the item removed, or NULL if unsuccessful.
 */
void
*remove_item_unsafe(linked_list *list, int index);

/**
 * Applies the given function to each item within the linked list.
 * @param list the list to apply to.
 * @param call the function to apply.
 */
void
for_each_il(linked_list *list, void call(void *node));

#endif //LINKEDLIST_H