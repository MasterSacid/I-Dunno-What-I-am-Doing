#ifndef MESH_H
#define MESH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "vector.h"
#include "triangle.h"
#include "upng.h"
#include "display.h"






//Dynamic size meshes struct
typedef struct {
    //These two are arrays!
    vec3_t* vertices;
    face_t* faces;
    upng_t* texture; //texture per mesh array pointer
    //This is x y and z value and used for rotation scale and translation
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
    int effectMode;
} mesh_t;


void loadMesh(char* objFilePath, char* pngFilePath, vec3_t scale, vec3_t translation, vec3_t rotation, int effectMode);
void loadMeshObjData (mesh_t* mesh, char* objFileName);
void loadMeshPngData(mesh_t* mesh, char* pngFileName);

int getNumOfMeshes();
mesh_t* getMesh(int index);
void freeMeshes();









#endif