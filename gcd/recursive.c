#include <stdio.h>
#include <stdlib.h>
#include "recursive.h"

int gcd_recursive(int m, int n) {
    m = abs(m);
    n = abs(n);
    if(m == 0) {
        return n;
    } else if(n == 0) {
        return m;
    } else {
        int rem = m%n;
        m = n;
        n = rem;
        int GCD = m;
        if(n == 0) {
            return GCD;
        }
        return gcd_recursive(m, n);
    }
}


