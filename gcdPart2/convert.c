#include <stdio.h>
#include <stdlib.h>

int main() {
    int d;
    printf("Enter an integer : ");
    scanf("%d", &d);
    printf("\nsigned decimal   : %d", d);
    printf("\nunsigned decimal : %u", d);
    printf("\nhexademical      : %x", d);
    printf("\nbinary           : ");
    unsigned int r = d;
    for(int i = 0; i<32; i++) {
        int p = (r & (1<<31)) >> 31;
        r = r << 1;
        char c = p + '0';
        putc(c, stdout);
        if(i % 4 == 3 && i < 31) {
            printf(" ");
        }
    }
    printf("\n");
    return EXIT_SUCCESS;
}
