#include "mesh.h"




#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0},
    .scale = {1.0,1.0,1.0},
    .translation = {0,0,0}
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
    { .a = 1, .b = 2, .c = 3, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .aUv = { 0, 0 }, .bUv = { 0, 1 }, .cUv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .aUv = { 0, 0 }, .bUv = { 1, 1 }, .cUv = { 1, 0 }, .color = 0xFFFFFFFF }
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

void loadObjFileData(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if (!fp) {
        perror("fopen");
        return;
    }

    char buffer[1024];
    tex2_t *texCoords = NULL; //Thats an array !

    while (fgets(buffer, sizeof(buffer), fp)) {
        // Skip leading whitespace
        char *p = buffer;
        while (*p && isspace((unsigned char)*p)) p++;

        // Skip empty lines and comments
        if (*p == '\0' || *p == '#') continue;


        if (strncmp(p, "v ", 2) == 0) {
            float x, y, z;
            if (sscanf(p + 2, "%f %f %f", &x, &y, &z) == 3) {
                vec3_t meshVertex = { .x = x, .y = y, .z = z };
                array_push(mesh.vertices, meshVertex);
            }
            continue;
        }

        // Texture Coordinate Info: "vt u v"

        if (strncmp(p, "vt ", 3) == 0) {
            tex2_t texCoord;
            sscanf(buffer,"vt %f %f", &texCoord.u,&texCoord.v);
            array_push(texCoords,texCoord);

            continue;
        }

        // Face Info: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3"
        if (strncmp(p, "f ", 2) == 0) {
            int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
            if (sscanf(p + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                       &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9) {
                face_t meshFace = {
                    .a = v1 - 1,
                    .b = v2 - 1,
                    .c = v3 - 1,
                    .aUv = texCoords[vt1 - 1],
                    .bUv = texCoords[vt2 - 1],
                    .cUv = texCoords[vt3 - 1],
                    .color = 0xFFFFFFFF
                };
                array_push(mesh.faces, meshFace);
                       }
            continue;
        }

    }
    array_free(texCoords);

    fclose(fp);
}
