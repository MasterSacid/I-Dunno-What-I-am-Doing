#ifndef MESH_H
#define MESH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "vector.h"
#include "triangle.h"




#define N_CUBE_VERTICES 8 //Cube has eight verticies
#define N_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face



extern face_t cubeFaces[N_CUBE_FACES];
extern vec3_t cubeVertices[N_CUBE_VERTICES];

//Dynamic size meshes struct
typedef struct {
    //These two are arrays!
    vec3_t* vertices;
    face_t* faces;
    //This is x y and z value and used for rotation scale and translation
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

extern mesh_t mesh;


void loadCubeMeshData(void);
void loadObjFileData (char* fileName);









#endif