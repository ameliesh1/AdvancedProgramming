#include <stddef.h>
#include "mystring.h"

size_t my_strlen(char *src) {
    if(src == NULL) {
        return 0;
    }
    size_t length = 0;
    for(int i=0; *src != '\0'; i++) {
        src++;
        length++;
    }
    return length;
}

char *my_strcpy(char *dst, char *src) {
    if(src == NULL) {
        return NULL;
    }
    char *ptr = dst;
    for(int i=0; *src != '\0'; i++) {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = '\0';
    return ptr;
}

void my_strlower(char *src) {
    if(src == NULL) {
       return;
    }
    for(int i=0; *src != '\0'; i++) {
        *src = tolower(*src);
        src++;
    }
}
