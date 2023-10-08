#include <stdio.h>
#include <stdlib.h>
#include "mystring.h"

/**
 * Take an array of char* pointers and print each of the strings to standard
 * out. This function must use only pointer arithmetic and no array
 * subscripting. The output start with "[", ends with "]\n", and prints the
 * strings inside, each separated by a comma and space.
 * Example: [Hi, BYE, AP, COMSW 3157, FunTimes]
 */
void display_strings(char **strings) {
    printf("[");
    if (strings != NULL) {
        char **str_ptr = strings;
        if (*str_ptr != NULL) {
            printf("%s", *str_ptr++);
            while (*str_ptr != NULL) {
                printf(", %s", *str_ptr++);
            }
        }
    }
    printf("]\n");
}

static char **copy_args_lowercase(int argc, char **argv) {
    char **arr = malloc(sizeof(char *) * (1 + argc));
    char **ptr = arr;
    int i;
    for(i=1; i<argc; i++) {
        char *dest = malloc(sizeof(char) * (1 + my_strlen(argv[i])));
        my_strcpy(dest, argv[i]);
        my_strlower(dest);
        *arr = dest;
        arr++;
    }
    *arr = NULL;
    return ptr;
}

static void free_copy(char **copy) {
    char **ptr = copy;
    for(; *copy != NULL; copy++) {
        free(*copy);
    }
    free(ptr);
}


/**
 * DO NOT MODIFY main()
 */
int main(int argc, char **argv) {
    char **copy;
    copy = copy_args_lowercase(argc, argv);

    printf("Raw arguments       : ");
    display_strings(argv + 1);

    printf("Lowercase arguments : ");
    display_strings(copy);

    free_copy(copy);

    return EXIT_SUCCESS;
}
