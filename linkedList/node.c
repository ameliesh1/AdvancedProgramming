#include <stdlib.h>
#include "node.h"

node* create_node(void *data) {
    node *n = (node *)malloc(sizeof(node));
    if (n != NULL) {
        n->data = data;
        n->next = NULL;
        n->prev = NULL;
    }
    return n;
}

void free_node(node *n, void (*free_data)(void *)) {
    free_data(n->data);
    free(n);
}