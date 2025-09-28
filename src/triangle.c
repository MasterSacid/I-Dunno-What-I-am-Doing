#include "triangle.h"
#include "display.h"
#include "swap.h"



    void drawTrianglePixel(
    int x, int y, uint32_t color,
    vec4_t pointA, vec4_t pointB, vec4_t pointC
    ) {
        vec2_t p = {x,y};
        vec2_t a = vec4ToVec2(pointA);
        vec2_t b = vec4ToVec2(pointB);
        vec2_t c = vec4ToVec2(pointC);


        vec3_t weights = barycentricWeights(a,b,c,p);

        float alpha = weights.x;
        float beta = weights.y;
        float gamma = weights.z;

        //Variables to store the interpolated values of U, V and 1/W for texel

        float interpolatedReciprocalW;


        //Interpolate the value of 1/w for the current texel
        interpolatedReciprocalW =(1/pointA.w) * alpha + (1/pointB.w) * beta + (1/pointC.w) * gamma;


        //Adjust 1/w so the pixels that are closer to the camera have smaller values.
        interpolatedReciprocalW = 1.0 - interpolatedReciprocalW;


        //Only draw the pixel if the depth value is the less than the one previously stored in the z buffer
        if (interpolatedReciprocalW < getZBufferAt(x,y)) {
            drawPixel(x,y,color);

            //Update the z buffer value with 1/w of current pixel
            updateZBufferAt(x,y,interpolatedReciprocalW);
        }
}






void drawFilledTriangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color) {

    //Sort it by ascending order (y0 < y1 <y2)
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&z0,&z1);
        floatSwap(&w0,&w1);
    }
    if (y1 > y2) {
        intSwap(&y1,&y2);
        intSwap(&x1,&x2);
        floatSwap(&z1,&z2);
        floatSwap(&w1,&w2);
    }
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&z0,&z1);
        floatSwap(&w0,&w1);
    }
    //Create vector points and texture coords after we sort the vertices
    vec4_t pointA = {x0,y0,z0,w0};
    vec4_t pointB = {x1,y1,z1,w1};
    vec4_t pointC = {x2,y2,z2,w2};

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

                drawTrianglePixel(x, y, color, pointA, pointB, pointC);

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

                drawTrianglePixel(x, y, color, pointA, pointB, pointC);

            }
        }
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
    int x, int y, upng_t* texture,
    vec4_t pointA, vec4_t pointB, vec4_t pointC,
    tex2_t aUv,  tex2_t bUv, tex2_t cUv
    ) {
        vec2_t p = {x,y};
        vec2_t a = vec4ToVec2(pointA);
        vec2_t b = vec4ToVec2(pointB);
        vec2_t c = vec4ToVec2(pointC);


        vec3_t weights = barycentricWeights(a,b,c,p);

        float alpha = weights.x;
        float beta = weights.y;
        float gamma = weights.z;

        //Variables to store the interpolated values of U, V and 1/W for texel
        float interpolatedU;
        float interpolatedV;
        float interpolatedReciprocalW;


        //Perform the interpolation of all U/W and V/W values using barycentric weights and factor of 1/W
        interpolatedU = (aUv.u/pointA.w) * alpha + (bUv.u/pointB.w) * beta + (cUv.u/pointC.w) * gamma;
        interpolatedV = (aUv.v/pointA.w) * alpha + (bUv.v/pointB.w) * beta + (cUv.v/pointC.w) * gamma;

        //Interpolate the value of 1/w for the current texel
        interpolatedReciprocalW =(1/pointA.w) * alpha + (1/pointB.w) * beta + (1/pointC.w) * gamma;

        //We divide back both interpolations by 1/w
        interpolatedU /= interpolatedReciprocalW;
        interpolatedV /= interpolatedReciprocalW;


        unsigned int textureWidth = upng_get_width(texture);
        unsigned int textureHeight = upng_get_height(texture);

        //Map the UV coordinate to the full texture width and height
        int texX = abs((int)(interpolatedU * textureWidth)) % textureWidth; //Clamping
        int texY = abs((int)((1.0 - interpolatedV) * textureHeight)) % textureHeight;

        //Adjust 1/w so the pixels that are closer to the camera have smaller values.
        interpolatedReciprocalW = 1.0 - interpolatedReciprocalW;


        //Only draw the pixel if the depth value is the less than the one previously stored in the z buffer
        if (interpolatedReciprocalW < getZBufferAt(x,y)) {

            //Get the buffer of colors from the texture
            uint32_t* textureBuffer = (uint32_t*)upng_get_buffer(texture);

            drawPixel(x,y,textureBuffer[(textureWidth * texY) + texX]);

            //Update the z buffer value with 1/w of current pixel
            updateZBufferAt(x,y,interpolatedReciprocalW);
        }




}



void drawTexturedTriangle (
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    upng_t* texture
    ) {
    //Sort it by ascending order (y0 < y1 <y2)
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&z0,&z1);
        floatSwap(&w0,&w1);
        floatSwap(&u0,&u1);
        floatSwap(&v0,&v1);
    }
    if (y1 > y2) {
        intSwap(&y1,&y2);
        intSwap(&x1,&x2);
        floatSwap(&z1,&z2);
        floatSwap(&w1,&w2);
        floatSwap(&u1,&u2);
        floatSwap(&v1,&v2);
    }
    if (y0 > y1) {
        intSwap(&y0,&y1);
        intSwap(&x0,&x1);
        floatSwap(&z0,&z1);
        floatSwap(&w0,&w1);
        floatSwap(&u0,&u1);
        floatSwap(&v0,&v1);
    }
    //Create vector points and texture coords after we sort the vertices
    vec4_t pointA = {x0,y0,z0,w0};
    vec4_t pointB = {x1,y1,z1,w1};
    vec4_t pointC = {x2,y2,z2,w2};
    tex2_t aUv = {u0,v0};
    tex2_t bUv = {u1,v1};
    tex2_t cUv = {u2,v2};



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

                //drawPixel(x,y,0xFFFF00FF);
                drawTexel(x,y,texture,pointA,pointB,pointC,aUv,bUv,cUv);

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
                drawTexel(x,y,texture,pointA,pointB,pointC,aUv,bUv,cUv);
            }
        }
    }






}