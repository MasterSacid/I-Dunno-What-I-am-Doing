#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>

#include "texture.h"
#include "vector.h"

typedef struct {
    int a;
    int b;
    int c;
    tex2_t aUv;
    tex2_t bUv;
    tex2_t cUv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t texCoords[3];
    uint32_t color;
} triangle_t;

void drawFilledTriangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color);


void drawTexel(
    int x, int y, uint32_t* texture,
    vec4_t pointA, vec4_t pointB, vec4_t pointC,
    tex2_t aUv,  tex2_t bUv, tex2_t cUv
    );

void drawTrianglePixel(
   int x, int y, uint32_t color,
   vec4_t pointA, vec4_t pointB, vec4_t pointC
   );

void drawTexturedTriangle (
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2,float u2, float v2,
    uint32_t* texture
    );

vec3_t barycentricWeights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);




#endif //TRIANGLE_H