#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct {
    float x , y;
} vec2_t;

typedef struct {
    float x,y,z;
}vec3_t;

typedef struct {
    float x , y , z ,w;
}vec4_t;

//2D Vec functions
float vec2Length (vec2_t v);
vec2_t vec2Add(vec2_t a, vec2_t b);
vec2_t vec2Subtract(vec2_t a, vec2_t b);
vec2_t vec2Multiply(vec2_t v, float factor);
vec2_t vec2Divide(vec2_t v, float factor);
float vec2DotProduct(vec2_t a, vec2_t b);
void vec2Normalize (vec2_t* v);
vec2_t vec2Clone(vec2_t* v);




//3D vec functions
vec3_t vec3New(float x, float y, float z);
vec3_t vec3Clone(vec3_t* v);
float vec3Length(vec3_t v);
vec3_t vec3Add(vec3_t a, vec3_t b);
vec3_t vec3Subtract(vec3_t a, vec3_t b);
vec3_t vec3Multiply(vec3_t v, float factor);
vec3_t vec3Divide(vec3_t v, float factor);
vec3_t vec3Cross(vec3_t a , vec3_t b);
float vec3DotProduct(vec3_t a, vec3_t b);
void vec3Normalize(vec3_t* v);
vec3_t rotateAroundX(vec3_t v , float angle);
vec3_t rotateAroundY(vec3_t v , float angle);
vec3_t rotateAroundZ(vec3_t v , float angle);

//Conversion Functions
vec4_t vec3ToVec4(vec3_t v);
vec3_t vec4ToVec3 (vec4_t v);
vec2_t vec4ToVec2(vec4_t v);



#endif //VECTOR_H
