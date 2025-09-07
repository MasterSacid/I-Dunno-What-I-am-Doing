#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct {
    float x , y;
} vec2_t;
typedef struct {
    float x,y,z;
}vec3_t;

//2D Vec functions
float vec2Length (vec2_t v);

//3D vec functions
float vec3Length(vec3_t v);

vec3_t rotateAroundX(vec3_t v , float angle);
vec3_t rotateAroundY(vec3_t v , float angle);
vec3_t rotateAroundZ(vec3_t v , float angle);



#endif //VECTOR_H
