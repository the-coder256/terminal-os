#ifndef ARRAYS_H_
#define ARRAYS_H_ 0

#include "../include/strings.h"

void append(char** arr, int* ptr, char* src) {
    for (int i = 0; i < len(src) + 1; i++) {
        arr[*ptr][i] = src[i];
    }
    (*ptr)++;
}

#endif