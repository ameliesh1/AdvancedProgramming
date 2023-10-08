#include <stdio.h>
#include <stdlib.h>
#include "iterative.h"
#include "recursive.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: ./gcd <integer m> <integer n>\n");
        return EXIT_FAILURE;
    }
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    if(m == 0 && n == 0) {
        fprintf(stdout, "gcd(0, 0) = undefined\n");
        return EXIT_SUCCESS;
    }
    int iter = gcd_iterative(m, n);
    int recur = gcd_recursive(m, n);
    printf("Iterative: gcd(%d, %d) = %d\n", m, n, iter);
    printf("Recursive: gcd(%d, %d) = %d\n", m, n, recur);
    return EXIT_SUCCESS;
}
