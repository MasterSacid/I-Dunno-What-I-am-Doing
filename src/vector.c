#include "vector.h"


//-------------------- Vec2D Functions are here -----------------------
float vec2Length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2Add(vec2_t a, vec2_t b) {
    vec2_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

vec2_t vec2Subtract(vec2_t a, vec2_t b) {
    vec2_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

vec2_t vec2Multiply(vec2_t v, float factor) {
    vec2_t result = {
        .x = v.x * factor,
        .y = v.y * factor
    };
    return result;
}

vec2_t vec2Divide(vec2_t v, float factor) {
    vec2_t result = {
        .x = v.x / factor,
        .y = v.y / factor
    };
    return result;
}

float vec2DotProduct(vec2_t a, vec2_t b) {
    return (a.x * b.x) + (a.y * b.y);
}

void vec2Normalize (vec2_t* v) {
    float length = sqrt(v->x * v->x + v->y * v->y);
    v -> x /= length;
    v -> y /= length;
}




//--------------------- Vec3D functions are here ------------------------
float vec3Length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3Add(vec3_t a, vec3_t b) {
    vec3_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z

    };
    return result;
}

vec3_t vec3Subtract(vec3_t a, vec3_t b) {
    vec3_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return result;
}

vec3_t vec3Multiply(vec3_t v, float factor) {
    vec3_t result = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor
    };
    return result;
}

vec3_t vec3Divide(vec3_t v, float factor) {
    vec3_t result = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor
    };
    return result;
}


vec3_t vec3Cross(vec3_t a, vec3_t b) {
    vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}
float vec3DotProduct(vec3_t a, vec3_t b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


vec3_t rotateAroundX(vec3_t v, float angle) {
    vec3_t rotatedVector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)

    };
    return rotatedVector;
}

vec3_t rotateAroundY(vec3_t v, float angle) {
    vec3_t rotatedVector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)

    };
    return rotatedVector;
}

vec3_t rotateAroundZ(vec3_t v, float angle) {
    vec3_t rotatedVector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z

    };
    return rotatedVector;
}

void vec3Normalize(vec3_t* v) {
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v -> x /= length;
    v -> y /= length;
    v -> z /= length;
}


//--------------------- Conversion functions are here ------------------------
vec4_t vec3ToVec4(vec3_t v) {
    vec4_t result = {v.x,v.y,v.z,1.0};
    return result;
}

vec3_t vec4ToVec3 (vec4_t v) {
    vec3_t result = {v.x,v.y,v.z};
    return result;
}



