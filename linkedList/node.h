#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>

typedef struct node {
    void *data;
    struct node *next;
    struct node *prev;
} node;

/**
 * Creates a node via the following steps:
 * - allocates memory for it with malloc().
 * - if malloc() fails, this function returns NULL
 * - malloc() succeeds, the data, next, and previous fields are initialized,
 *   and a pointer to the newly created node is returned.
 */
node* create_node(void *data);

/**
 * Takes in a pointer to the node to free as well as a pointer to the function
 * that frees the node's data.
 * The use of this function must have knowledge of the node's data, and pass
 * the appropriate free function on it.
 */
void free_node(node *n, void (*free_data)(void *));

#endif