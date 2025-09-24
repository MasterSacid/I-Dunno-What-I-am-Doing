#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"
#include <math.h>


typedef struct {
    float m [4][4];

}mat4_t;

mat4_t mat4Identity(void);
mat4_t mat4MakeScale(float sx, float sy, float sz);
mat4_t mat4MakeTranslation(float tx, float ty, float tz);
mat4_t mat4MakeRotationX(float angle);
mat4_t mat4MakeRotationY(float angle);
mat4_t mat4MakeRotationZ(float angle);
mat4_t mat4MakePerspective(float fov, float aspect, float zNear, float zFar);
vec4_t mat4MultipVec4Project(mat4_t matProj, vec4_t v);
mat4_t mat4MultipMat4(mat4_t a, mat4_t b);

vec4_t mat4MultipVec4(mat4_t m , vec4_t v);
mat4_t mat4LookAt(vec3_t eye, vec3_t target, vec3_t up);

#endif //MATRIX_H