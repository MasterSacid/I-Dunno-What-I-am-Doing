#ifndef CLIPPING_H
#define CLIPPING_H

#include "triangle.h"
#include "vector.h"

#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct {
    vec3_t vertices [MAX_NUM_POLY_VERTICES];
    tex2_t texCoords[MAX_NUM_POLY_VERTICES];
    int numVertices;
    float intensities[MAX_NUM_POLY_VERTICES];
}polygon_t;

void initFrustumPlanes(float fovX, float fovY, float zNear, float zFar);
polygon_t createPolygonFromTriangle(vec3_t v0, vec3_t v1, vec3_t v2,tex2_t t0, tex2_t t1, tex2_t t2, float iA, float iB, float iC);
void trianglesFromPolygon(polygon_t* polygon, triangle_t triangles[], int* numTriangles);
void clipPolygonAgainstPlane(polygon_t *polygon,int plane);
void clipPolygon(polygon_t* polygon);
#endif //CLIPPING_H