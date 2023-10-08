#include <stdio.h>
#include <string.h>
#include "linked_list.h"

void print_function(void *data) {
    char *word = data;
    printf("%s", word);
}

void free_data(void *a) {
    
}

int str_cmp(const void *a, const void *b) {
    return strcmp(a, b);
}

int main(int argc, char **argv) {
    linked_list *list = create_linked_list();
    print_list(list, print_function);
    int i = 1;
    while(i < argc) {
        insert_in_order(list, argv[i], str_cmp);
        print_list(list, print_function);
        i++;
    }
    free_list(list, free_data);
    return EXIT_SUCCESS;
}

