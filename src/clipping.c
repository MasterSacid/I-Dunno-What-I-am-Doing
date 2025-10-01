#include "clipping.h"


plane_t frustumPlanes[6]; //We have six planes
void initFrustumPlanes(float fovX, float fovY, float zNear, float zFar) {
    float cosHalfFovX = cos(fovX / 2);
    float sinHalfFovX = sin(fovX / 2);
    float cosHalfFovY = cos(fovY / 2);
    float sinHalfFovY = sin(fovY / 2);

    frustumPlanes[LEFT_FRUSTUM_PLANE].point = vec3New(0, 0, 0);
    frustumPlanes[LEFT_FRUSTUM_PLANE].normal.x = cosHalfFovX;
    frustumPlanes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustumPlanes[LEFT_FRUSTUM_PLANE].normal.z = sinHalfFovX;

    frustumPlanes[RIGHT_FRUSTUM_PLANE].point = vec3New(0, 0, 0);
    frustumPlanes[RIGHT_FRUSTUM_PLANE].normal.x = -cosHalfFovX;
    frustumPlanes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustumPlanes[RIGHT_FRUSTUM_PLANE].normal.z = sinHalfFovX;

    frustumPlanes[TOP_FRUSTUM_PLANE].point = vec3New(0, 0, 0);
    frustumPlanes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustumPlanes[TOP_FRUSTUM_PLANE].normal.y = -cosHalfFovY;
    frustumPlanes[TOP_FRUSTUM_PLANE].normal.z = sinHalfFovY;

    frustumPlanes[BOTTOM_FRUSTUM_PLANE].point = vec3New(0, 0, 0);
    frustumPlanes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustumPlanes[BOTTOM_FRUSTUM_PLANE].normal.y = cosHalfFovY;
    frustumPlanes[BOTTOM_FRUSTUM_PLANE].normal.z = sinHalfFovY;

    frustumPlanes[NEAR_FRUSTUM_PLANE].point = vec3New(0, 0, zNear);
    frustumPlanes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustumPlanes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustumPlanes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustumPlanes[FAR_FRUSTUM_PLANE].point = vec3New(0, 0, zFar);
    frustumPlanes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustumPlanes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustumPlanes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t createPolygonFromTriangle(vec3_t v0, vec3_t v1, vec3_t v2,tex2_t t0, tex2_t t1, tex2_t t2, float iA, float iB, float iC){
    polygon_t polygon = {
        .vertices = {v0,v1,v2},
        .texCoords = {t0,t1,t2},
        .numVertices = 3,
        .intensities = {iA,iB,iC}
    };
    return polygon;
}

void trianglesFromPolygon(polygon_t* polygon, triangle_t triangles[], int* numTriangles) {

    for (int i = 0; i < polygon ->numVertices - 2; i ++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = vec3ToVec4(polygon -> vertices[index0]);
        triangles[i].points[1] = vec3ToVec4(polygon -> vertices[index1]);
        triangles[i].points[2] = vec3ToVec4(polygon -> vertices[index2]);

        triangles[i].texCoords[0] = polygon ->texCoords[index0];
        triangles[i].texCoords[1] = polygon ->texCoords[index1];
        triangles[i].texCoords[2] = polygon ->texCoords[index2];

        triangles[i].intensities[0] = polygon->intensities[index0];
        triangles[i].intensities[1] = polygon->intensities[index1];
        triangles[i].intensities[2] = polygon->intensities[index2];
    }
    *numTriangles = polygon ->numVertices - 2;
}

float floatLerp(float a, float b, float t) {
    return a + t * (b-a);
}

void clipPolygonAgainstPlane(polygon_t* polygon,int plane) {
    vec3_t planePoint  = frustumPlanes[plane].point;
    vec3_t planeNormal = frustumPlanes[plane].normal;

    //Array of inside vertices that will be part of the final polygon
    vec3_t insideVertices[MAX_NUM_POLY_VERTICES];
    tex2_t insideTexCoords[MAX_NUM_POLY_VERTICES];
    float insideIntensities[MAX_NUM_POLY_VERTICES];

    int numInsideVertices = 0;

    vec3_t* currentVertex  = &polygon ->vertices[0];
    tex2_t* currentTexCoord = &polygon ->texCoords[0];

    vec3_t* previousVertex = &polygon -> vertices[polygon -> numVertices -1];
    tex2_t* previousTexCoord = &polygon ->texCoords[polygon ->numVertices -1];

    float* currentIntensity = &polygon -> intensities [0];
    float* previousIntensity = &polygon -> intensities [polygon -> numVertices -1];

    float currentDot = 0;
    float prevDot = vec3DotProduct(vec3Subtract(*previousVertex,planePoint),planeNormal);

    //Loop all the poly vertices
    while (currentVertex != &polygon -> vertices[polygon -> numVertices]) {
        currentDot = vec3DotProduct(vec3Subtract(*currentVertex,planePoint),planeNormal);

        //if there is switch between inside and outside
        if (currentDot * prevDot < 0) {
            //find the interpolation factor t
            float t = prevDot / (prevDot - currentDot);


            //----------------------calculate  the intersection point I = Q1 + t(Q2-Q1)-----------------------
            //For vertexes
            vec3_t intersectionPoint = {
                .x =floatLerp(previousVertex -> x, currentVertex -> x,t),
                .y =floatLerp(previousVertex -> y, currentVertex -> y,t),
                .z =floatLerp(previousVertex -> z, currentVertex -> z,t)
            };

            //For texels
            tex2_t interpolatedTexCoord = {
                .u = floatLerp(previousTexCoord -> u, currentTexCoord -> u, t),
                .v = floatLerp(previousTexCoord -> v, currentTexCoord -> v, t)
            };

            //For Intensities
            float interpolatedIntensity = floatLerp(*previousIntensity,*currentIntensity,t);

            insideVertices[numInsideVertices] = vec3Clone(&intersectionPoint);
            insideTexCoords[numInsideVertices] = tex2Clone(&interpolatedTexCoord);
            insideIntensities[numInsideVertices] = interpolatedIntensity;
            numInsideVertices++;

        }

        //Current vertex inside the plane
        if (currentDot >= 0) {
            insideVertices[numInsideVertices] = vec3Clone(currentVertex);
            insideTexCoords[numInsideVertices] = tex2Clone(currentTexCoord);
            insideIntensities[numInsideVertices]= *currentIntensity;
            numInsideVertices++;
        }

        prevDot = currentDot;

        previousVertex    = currentVertex;
        currentVertex++;

        previousTexCoord  = currentTexCoord;
        currentTexCoord++;

        previousIntensity = currentIntensity;
        currentIntensity++;

    }

    //copy the list of in vertices to the polygon
    for (int i = 0; i < numInsideVertices ; i++) {
        polygon -> vertices[i] = vec3Clone(&insideVertices[i]);
        polygon -> texCoords[i] = tex2Clone(&insideTexCoords[i]);
        polygon->intensities[i]  = insideIntensities[i];
    }
    polygon -> numVertices = numInsideVertices;
}


void clipPolygon(polygon_t* polygon) {
    clipPolygonAgainstPlane(polygon,LEFT_FRUSTUM_PLANE);
    clipPolygonAgainstPlane(polygon,RIGHT_FRUSTUM_PLANE);
    clipPolygonAgainstPlane(polygon,TOP_FRUSTUM_PLANE);
    clipPolygonAgainstPlane(polygon,BOTTOM_FRUSTUM_PLANE);
    clipPolygonAgainstPlane(polygon,NEAR_FRUSTUM_PLANE);
    clipPolygonAgainstPlane(polygon,FAR_FRUSTUM_PLANE);
}


