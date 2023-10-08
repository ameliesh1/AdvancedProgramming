#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

linked_list* create_linked_list() {
    linked_list *list = malloc(sizeof(linked_list));
    if(list == NULL) {
        return NULL;
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

bool insert_in_order(
    linked_list *list, void *data, int (*cmp)(const void*, const void*)) {
    node *dataNode = create_node(data);
    if(dataNode == NULL) {
        return false;
    }
    if(list->head == NULL) {
        list->head = dataNode;
        list->tail = dataNode;
        list->size++;
        return true;
    }
    node *ptr = list->head;
    while(ptr != NULL && (cmp(dataNode->data, ptr->data) >= 0)) {
        ptr = ptr->next;
    }
    if(ptr == NULL) {
        node *end = list->tail;
        end->next = dataNode;
        dataNode->prev = end;
        dataNode->next = NULL;
        list->tail = dataNode;
        list->size++;
        return true;
    }
    if(ptr->prev == NULL) {
        list->head = dataNode;
        dataNode->next = ptr;
        ptr->prev = dataNode;
        list->size++;
        return true;
    }
    node *after = ptr;
    ptr->prev->next = dataNode;
    dataNode->prev = ptr->prev;
    dataNode->next = after;
    if(after == NULL) {
        list->tail = dataNode;
    }
    list->size++;
    return true;
}

void print_list(linked_list *list, void (*print_function)(void*)) {
    putchar('[');
    node *cur = list->head;
    if (cur != NULL) {
        print_function(cur->data);
        cur = cur->next;
    }
    for ( ; cur != NULL; cur = cur->next) {
        printf(", ");
        print_function(cur->data);
    }
    printf("]\n{length: %lu, head->data: ", list->size);
    list->head != NULL ? print_function(list->head->data) :
                         (void)printf("NULL");
    printf(", tail->data: ");
    list->tail != NULL ? print_function(list->tail->data) :
                         (void)printf("NULL");
    printf("}\n\n");
}

void free_list(linked_list *list, void (*free_data)(void *)) {
    node *ptr = list->tail;
    node *previous = ptr;
    while(ptr != NULL) {
        previous = ptr->prev;
        free_node(ptr, free_data);
        ptr = previous;
    }
    free(list);
}
