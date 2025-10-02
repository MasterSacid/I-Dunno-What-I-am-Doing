#include "mesh.h"
#include "array.h"

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int meshCount = 0;


void loadMesh(char* objFilePath, char* pngFilePath, vec3_t scale, vec3_t translation, vec3_t rotation, int effectMode) {
    loadMeshObjData(&meshes[meshCount],objFilePath);
    loadMeshPngData(&meshes[meshCount],pngFilePath);

    meshes[meshCount].scale = scale;
    meshes[meshCount].translation = translation;
    meshes[meshCount].rotation = rotation;
    meshes[meshCount].effectMode = effectMode;

    meshCount++;
}



void loadMeshObjData(mesh_t* mesh, char *objFileName) {
    FILE *fp = fopen(objFileName, "r");
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
                array_push(mesh -> vertices, meshVertex);
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
                array_push(mesh -> faces, meshFace);
                       }
            continue;
        }

    }
    array_free(texCoords);

    fclose(fp);
}


void loadMeshPngData(mesh_t* mesh, char* pngFileName) {
    upng_t* pngImage = upng_new_from_file(pngFileName);
    if (pngImage != NULL) {
        upng_decode(pngImage);
        if (upng_get_error(pngImage)== UPNG_EOK) {
            mesh -> texture = pngImage;
        }

    }
}
int getNumOfMeshes() {
    return meshCount;
}

mesh_t* getMesh(int index) {
    return &meshes[index];
}

void freeMeshes() {
    for (int i = 0 ; i < meshCount; i++) {
        upng_free(meshes[i].texture);
        array_free(meshes[i].faces);
        array_free(meshes[i].vertices);
    }

}


