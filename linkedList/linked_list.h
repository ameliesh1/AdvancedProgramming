#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include "node.h"

typedef struct linked_list {
    node *head;
    node *tail;
    size_t size;
} linked_list;

/**
 * Creates a linked_list via the following steps:
 * - allocates memory for it with malloc().
 * - if malloc() fails, this function returns NULL
 * - malloc() succeeds, the head, tail, and size fields are initialized,
 *   and a pointer to the newly created linked_list is returned.
 */
linked_list* create_linked_list();

/**
 * Inserts a non-NULL node into a (possibly NULL) linked list.
 * The node is created with the specified data and then inserted so that the
 * data in the linked list remains in non-decreasing order.
 * A node with a value already in the list is inserted AFTER the node(s)
 * already in the list.
 *
 * If list is NULL, a new one should not be created. 
 * 
 * Returns turn if the operation was successful and false otherwise.
 */
bool insert_in_order(
    linked_list *list, void *data, int (*cmp)(const void*, const void*));

/**
 * Prints the linked list to standard out with the function passed in to print
 * the contents of each node in the list.
 */
void print_list(linked_list *list, void (*print_function)(void*));

/**
 * Frees a list starting from the tail.
 * This will check if your previous pointers have been set up correctly.
 */
void free_list(linked_list *list, void (*free_data)(void *));

#endif