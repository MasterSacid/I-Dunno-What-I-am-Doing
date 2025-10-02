#include "swap.h"


void intSwap(int* a, int*b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


void floatSwap(float* a, float* b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

void vec2Swap(vec2_t* a, vec2_t* b) {
    vec2_t temp = *a;
    *a = *b;
    *b = temp;
}