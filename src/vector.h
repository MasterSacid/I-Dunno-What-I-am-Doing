#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct {
    float x;
    float y;
} vec2_t;
typedef struct {
    float x;
    float y;
    float z;
}vec3_t;

vec3_t rotateAroundX(vec3_t v , float angle);
vec3_t rotateAroundY(vec3_t v , float angle);
vec3_t rotateAroundZ(vec3_t v , float angle);



#endif //VECTOR_H
