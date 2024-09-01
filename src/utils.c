#include "utils.h"
#include "stddef.h"

// Swap function to swap two elements
void swap(void* a, void* b, size_t size) {
    char temp;
    char* p1 = (char*)a;
    char* p2 = (char*)b;
    
    for (size_t i = 0; i < size; i++) {
        temp = p1[i];
        p1[i] = p2[i];
        p2[i] = temp;
    }
}

// Partition function for quicksort
int partition(void* base, int low, int high, size_t size, int (*cmp)(const void*, const void*)) {
    char* arr = (char*)base;
    void* pivot = arr + high * size;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (cmp(arr + j * size, pivot) < 0) {
            i++;
            swap(arr + i * size, arr + j * size, size);
        }
    }
    swap(arr + (i + 1) * size, arr + high * size, size);
    return i + 1;
}

// Quicksort function
void quickSort(void* base, int low, int high, size_t size, int (*cmp)(const void*, const void*)) {
    if (low < high) {
        int pi = partition(base, low, high, size, cmp);
        quickSort(base, low, pi - 1, size, cmp);
        quickSort(base, pi + 1, high, size, cmp);
    }
}

