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