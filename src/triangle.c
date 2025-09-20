#include "triangle.h"
#include "display.h"
#include "swap.h"





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
        fillFlatBottomTriangle(x0,y0,x1,y1,Mx,My,color);

        //Draw flat -top triangle
        fillFlatTopTriangle(x1,y1,Mx,My,x2,y2,color);
    }


}
//======================TEXTURE STUFF========================

vec3_t barycentricWeights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    // Find the vectors between the vertices ABC and point p
    vec2_t ac = vec2Subtract(c, a);
    vec2_t ab = vec2Subtract(b, a);
    vec2_t pc = vec2Subtract(c, p);
    vec2_t pb = vec2Subtract(b, p);
    vec2_t ap = vec2Subtract(p, a);

    // Area of the full parallelogram (triangle ABC) using cross product
    float areaParallelogramAbc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

    // Alpha = area of parallelogram-PBC over the area of the full parallelogram-ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) / areaParallelogramAbc;

    // Beta = area of parallelogram-APC over the area of the full parallelogram-ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / areaParallelogramAbc;

    // Gamma can be easily found since barycentric coordinates always add up to 1.0
    float gamma = 1.0 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}



//Function to draw textured pixel at x and y position
void drawTexel(
    int x, int y, uint32_t* texture,
    vec2_t pointA, vec2_t pointB,vec2_t pointC,
    float u0, float v0, float u1, float v1, float u2, float v2
    ) {
        vec2_t pointP = {x,y};

        vec3_t weights = barycentricWeights(pointA,pointB,pointC,pointP);

        float alpha = weights.x;
        float beta = weights.y;
        float gamma = weights.z;

        //Perform the interpolation of all U and V values using barycentric weights
        float interpolatedU = u0 * alpha + u1 * beta + u2 * gamma;
        float interpolatedV = v0 * alpha + v2 * beta + v2 * gamma;

        //Map the UV coordinate to the full texture width and height
        int texX = abs((int)(interpolatedU * textureWidth));
        int texY = abs((int)(interpolatedV * textureHeight));

        drawPixel(x,y,texture[(textureWidth * texY) + texX]);


}



void drawTexturedTriangle (
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t* texture
    ) {
    //Sort it by ascending order (y0 < y1 <y2)
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&u0,&u1);
        floatSwap(&v0,&v1);
    }
    if (y1 > y2) {
        intSwap(&y1,&y2);
        intSwap(&x1,&x2);
        floatSwap(&u1,&u2);
        floatSwap(&v1,&v2);
    }
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&u0,&u1);
        floatSwap(&v0,&v1);
    }
    //Create vector points after we sort the vertices
    vec2_t pointA = {x0,y0};
    vec2_t pointB = {x1,y1};
    vec2_t pointC = {x2,y2};


    //++++++++++==================++++++++++
    //Render the upper part of the triangle
    //++++++++++==================++++++++++
    float invSlope1 = 0;
    float invSlope2 = 0;

    if (y1-y0 != 0)
        invSlope1 = (float)(x1-x0) /(float) abs(y1-y0);

    if(y2-y0!= 0)
        invSlope2 = (float)(x2-x0) /(float) abs(y2-y0);

    if (y1-y0 != 0) {
        for (int y = y0; y<= y1; y++) {
            int xStart = x1 + (y - y1) * invSlope1; //Since these are pixels it is better for them to be int
            int xEnd = x0 + (y - y0) * invSlope2;

            if (xEnd < xStart) {
                intSwap(&xStart,&xEnd); // It should be always from left to right so we swap them
            }

            for (int x = xStart; x<xEnd;x++) {
                /
                //drawPixel(x,y,0xFFFF00FF);
                drawTexel(x,y,texture,pointA,pointB,pointC,u0,v0,u1,v1,u2,v2);

            }
        }
    }


    //++++++++++==================++++++++++
    //Render the bottom part of the triangle
    //++++++++++==================++++++++++
     invSlope1 = 0;
     invSlope2 = 0;

    if (y2-y1 != 0)
        invSlope1 = (float)(x2-x1) /(float) abs(y2-y1);

    if(y2-y0!= 0)
        invSlope2 = (float)(x2-x0) /(float) abs(y2-y0);

    if (y2-y1 != 0) {
        for (int y = y1; y<= y2; y++) {
            int xStart = x1 + (y - y1) * invSlope1; //Since these are pixels it is better for them to be int
            int xEnd = x0 + (y - y0) * invSlope2;

            if (xEnd < xStart) {
                intSwap(&xStart,&xEnd); // It should be always from left to right so we swap them
            }

            for (int x = xStart; x<xEnd;x++) {

                //drawPixel(x,y,0xFFFF00FF);
                drawTexel(x,y,texture,pointA,pointB,pointC,u0,v0,u1,v1,u2,v2);

            }
        }
    }






}