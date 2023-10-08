#include <stdio.h>
#include <string.h>
#include "quicksort.h"

/* Static (private to this file) function prototypes. */
static void swap(void *a, void *b, size_t size);
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*cmp) (const void*, const void*));
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*cmp) (const void*, const void*));

/**
 * Swaps the values in two pointers.
 *
 * Casts the void pointers to type (char *) and works with them as char pointers
 * for the remainder of the function. Swaps one byte at a time, until all 'size'
 * bytes have been swapped. For example, if ints are passed in, size will be 4
 * and this function will swap 4 bytes starting at a and b pointers.
 */
static void swap(void *a, void *b, size_t size) {
    char *l = (char *) a;
    char *r = (char *) b;
    char temp;
    for(int i = 0; i < size; i++) {
        temp = l[i];
        l[i] = r[i];
        r[i] = temp;
    }
}

/**
 * Partitions array around a pivot, utilizing the swap function. Each time the
 * function runs, the pivot is placed into the correct index of the array in
 * sorted order. All elements less than the pivot should be to its left, and all
 * elements greater than or equal to the pivot should be to its right.
 */
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*cmp) (const void*, const void*)) {
    char *arr = (char *) array;
    void *p = &arr[left * elem_sz];
    int s = left;
    for(int i = elem_sz * (left+1); i <= (elem_sz * right); i += elem_sz) {
        if(cmp(&arr[i], p) < 0) {
            s += 1;
            swap(&arr[s * elem_sz], &arr[i], elem_sz);
        }
    }
    swap(&arr[left * elem_sz], &arr[s * elem_sz], elem_sz);
    return s;
}

/**
 * Sorts with lomuto partitioning, with recursive calls on each side of the
 * pivot.
 * This is the function that does the work, since it takes in both left and
 * right index values.
 */
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*cmp) (const void*, const void*)) {
    if(left < right) {
        int s = lomuto(array, left, right, elem_sz, (int(*)(const void*, const void*))cmp);
        quicksort_helper(array, left, s-1, elem_sz, (int(*)(const void*, const void*))cmp);
        quicksort_helper(array, s+1, right, elem_sz, (int(*)(const void*, const void*))cmp);
    }
}

int int_cmp(const void *a, const void *b) {
    int *l = (int *) a;
    int *r = (int *) b;
    if(*l > *r) {
        return 1;
    } else if(*l == *r) {
        return 0;
    } else {
        return -1;
    }
}

int dbl_cmp(const void *a, const void *b) {
    double l = *(double *) a;
    double r = *(double *) b;
    if(l > r) {
        return 1;
    } else if(l == r) {
        return 0;
    } else {
        return -1;
    }
}

int str_cmp(const void *a, const void *b) {
    char **l = (char **) a;
    char **r = (char **) b;
    return strcmp(*l, *r);
}

void quicksort(void *array, size_t len, size_t elem_sz,
               int (*cmp) (const void*, const void*)) {
    quicksort_helper(array, (int) 0, (int) len-1, elem_sz, *cmp);
}
