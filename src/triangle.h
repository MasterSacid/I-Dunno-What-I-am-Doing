#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "water.h"
#include "texture.h"
#include "vector.h"
#include "upng.h"

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
    upng_t* texture;
    float intensity;
    float intensities[3];
    vec2_t   worldXZ[3];
    int effect;
} triangle_t;



extern WaterParams gWater;
extern float gTimeSeconds;

void drawFilledTriangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color);


void drawTexel(
    int x,int y, upng_t* texture,
    vec4_t pA, vec4_t pB, vec4_t pC,
    tex2_t uvA, tex2_t uvB, tex2_t uvC,
    float i0, float i1, float i2,
    vec2_t aWZ, vec2_t bWZ, vec2_t cWZ,int effect
);

void drawTrianglePixel(
   int x, int y, uint32_t color,
   vec4_t pointA, vec4_t pointB, vec4_t pointC
   );

void drawTexturedTriangle(
    int x0,int y0,float z0,float w0,float u0,float v0,
    int x1,int y1,float z1,float w1,float u1,float v1,
    int x2,int y2,float z2,float w2,float u2,float v2,
    upng_t* texture,
    float i0, float i1, float i2,
    vec2_t aWZ,vec2_t bWZ, vec2_t cWZ,int effect
);

vec3_t barycentricWeights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

vec3_t getTriangleNormal(vec4_t vertices[3]);


void gouraudForClippedTri(
    const vec4_t viewTri[3],
    const float  Iorig[3],
    const triangle_t* clipped,
    float outI[3]
);



#endif //TRIANGLE_H