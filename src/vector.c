#include "vector.h"




//-------------------- Vec2D Functions are here -----------------------
float vec2Length (vec2_t v) {
    return sqrt(v.x *v.x + v.y*v.y);
}


//--------------------- Vec3D functions are here ------------------------
float vec3Length (vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}




vec3_t rotateAroundX(vec3_t v , float angle) {
    vec3_t rotatedVector = {
        .x= v.x,
        .y= v.y * cos(angle) - v.z * sin(angle),
        .z= v.y * sin(angle) + v.z * cos(angle)

    };
    return rotatedVector;

}

vec3_t rotateAroundY(vec3_t v , float angle) {
    vec3_t rotatedVector = {
        .x= v.x * cos(angle) - v.z * sin(angle),
        .y= v.y,
        .z= v.x * sin(angle) + v.z * cos(angle)

    };
    return rotatedVector;

}

vec3_t rotateAroundZ(vec3_t v , float angle) {
    vec3_t rotatedVector = {
        .x= v.x * cos(angle) - v.y * sin(angle),
        .y= v.x * sin(angle) + v.y * cos(angle),
        .z= v.z

    };
    return rotatedVector;

}


