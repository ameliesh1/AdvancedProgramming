#include <stdio.h>
#include <stdlib.h>
#include "iterative.h"

int gcd_iterative(int m, int n) {
    m = abs(m);
    n = abs(n);
    if(m == 0) {
        return n;
    } else if(n == 0) {
        return m;
    } else {
        int GCD = n;
        while(n != 0) {
            int rem = m%n;
            m = n;
            n = rem;
            GCD = m;
        }
    return GCD;
    }
}
