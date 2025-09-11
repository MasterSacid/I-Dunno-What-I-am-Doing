#include "triangle.h"
#include "display.h"

void intSwap(int* a, int*b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}



void fillFlatBottomTriangle(int x0,int y0,int x1,int y1,int x2,int y2, uint32_t color) {
    //Find the inverse slopes of the legs
    float inverseSlope1 = (float)(x1 - x0) / (y1 - y0);
    float inverseSlope2 = (float)(x2 - x0) / (y2 - y0);

    //Start xStart and xEnd from the top vertex (Which is vertes (x0,y0))
    float xStart = x0;
    float xEnd = x0;

    //Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; y++) {
        drawLine(xStart,y,xEnd,y,color);
        xStart += inverseSlope1;
        xEnd += inverseSlope2;
    }
}

void fillFlatTopTriangle(int x0,int y0,int x1,int y1,int x2,int y2, uint32_t color) {
    //Find the inverse slopes of the legs
    float inverseSlope1 = (float)(x2 - x0) / (y2 - y0);
    float inverseSlope2 = (float)(x2 - x1) / (y2 - y1);

    //Start xStart and xEnd from the bottom vertex (Which is vertes (x2,y2))
    float xStart = x2;
    float xEnd = x2;

    //Loop all the scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        drawLine(xStart,y,xEnd,y,color);
        xStart -= inverseSlope1;
        xEnd -= inverseSlope2;
    }

}


void drawFilledTriangle(int x0, int y0, int x1 , int y1, int x2, int y2,uint32_t color) {
    //Sort it by ascending order (y0 < y1 <y2)
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
    }
    if (y1 > y2) {
        intSwap(&y1,&y2);
        intSwap(&x1,&x2);
    }
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
    }

    if (y1==y2) {  //This two conditionals are added to aviod division by zero
        //This will directly draw the bottom manually
        fillFlatBottomTriangle(x0,y0,x1,y1,x2,y2,color);
    }
    else if (y0 == y1) {
        //Same excuse
        fillFlatTopTriangle(x0,y0,x1,y1,x2,y2,color);
    }

    //No division by zero then calculate ...
    else {
        //Calculate the vertex Mx and My (In my notes)
        int My = y1;
        int Mx = ((float)((x2- x0) * (y1-y0)) / (float) (y2 - y0)) + x0;

        //Draw flat - bottom triangle
        fillFlatBottomTriangle(x0,y0,x1,y1,Mx,My,0xFFFFFF00);

        //Draw flat -top triangle
        fillFlatTopTriangle(x1,y1,Mx,My,x2,y2,0xFFFFFF00);
    }


}
