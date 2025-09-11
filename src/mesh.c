#include "mesh.h"




#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0}
};

vec3_t cubeVertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {.x = -1, .y = 1, .z = -1},
    {.x = 1, .y = 1, .z = -1},
    {.x = 1, .y = -1, .z = -1},
    {.x = 1, .y = 1, .z = 1},
    {.x = 1, .y = -1, .z = 1},
    {.x = -1, .y = 1, .z = 1},
    {.x = -1, .y = -1, .z = 1}
};

face_t cubeFaces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3,.color = 0xFFE6B0FF},
    {.a = 1, .b = 3, .c = 4,.color = 0xFFE6B0FF},

    // right
    {.a = 4, .b = 3, .c = 5,.color = 0xFFFF7F11},
    {.a = 4, .b = 5, .c = 6,.color = 0xFFFF7F11},

    // back
    {.a = 6, .b = 5, .c = 7,.color = 0xFF11C5FF},
    {.a = 6, .b = 7, .c = 8,.color = 0xFF11C5FF},

    // left
    {.a = 8, .b = 7, .c = 2,.color = 0xFF32CD32},
    {.a = 8, .b = 2, .c = 1,.color = 0xFF32CD32},

    // top
    {.a = 2, .b = 7, .c = 5,.color = 0xFF3A3A3A},
    {.a = 2, .b = 5, .c = 3,.color = 0xFF3A3A3A},

    // bottom
    {.a = 6, .b = 8, .c = 1,.color = 0xFF8B0000},
    {.a = 6, .b = 1, .c = 4,.color = 0xFF8B0000}
};

void loadCubeMeshData(void) {
    for (int i = 0; i< N_CUBE_VERTICES; i ++) {
        vec3_t cubeVertex = cubeVertices[i];
        array_push(mesh.vertices,cubeVertex);
    }
    for (int i = 0; i <N_CUBE_FACES; i++) {
        face_t cubeFace = cubeFaces[i];
        array_push(mesh.faces,cubeFace);
    }
}

void loadObjFileData (char* fileName) {

    FILE *fp = fopen(fileName, "r");
    char buffer[1000];



    while (fgets(buffer, sizeof(buffer), fp)) {
        if (buffer[0] == 'v') {
            float x,y,z;
            sscanf(buffer,"v %f %f %f",&x,&y,&z);
            vec3_t meshVertex = {.x=x,.y=y,.z=z};
            array_push(mesh.vertices,meshVertex);
        }
        if (buffer[0] == 'f') {
            int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
            sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
            face_t meshFace = {.a = v1, .b= v2, .c=v3};
            array_push(mesh.faces,meshFace);

        }
    }

    fclose(fp);


}
