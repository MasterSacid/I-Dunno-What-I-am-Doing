#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "upng.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "camera.h"
#include "clipping.h"
#include "water.h"


void setup(void);

void processInput(void);

void update(void);

void render(void);


void freeResources(void);
void processGraphicsPipelineStages(mesh_t* mesh);




#define MAX_TRIANGLES_PER_MESH 10000


bool isRunning = false;
uint32_t previousFrameTime = 0;
float deltaTime = 0;



mat4_t projMatrix;
mat4_t viewMatrix;
mat4_t worldMatrix;

WaterParams gWater;
float gTimeSeconds;


triangle_t trianglesToRender[MAX_TRIANGLES_PER_MESH];

int numTrianglesToRender = 0;

light_t light = {.position = {0, 0, 5}};


int main(void) {
    isRunning = initializeWindow();


    setup();


    while (isRunning) {
        processInput();
        update();
        render();
    }


    freeResources();


    return 0;
}


void setup(void) {
    setRenderMode(RENDER_TEXTURED);
    setCullMode(CULL_BACKFACE);

    float aspectY = (float) getWindowHeight() / (float) getWindowWidth();
    float aspectX = (float) getWindowWidth() / (float) getWindowHeight();
    float fovY = M_PI / 3.0; //60 degrees to radians
    float fovX = 2.0 * atan(tan(fovY / 2) * aspectX);
    float zNear = 0.1;
    float zFar = 100.0;
    projMatrix = mat4MakePerspective(fovY, aspectY, zNear, zFar);

    initFrustumPlanes(fovX, fovY, zNear, zFar);

    vec3_t sunRaysDir = {0.0f, 3.0f, 0.0f};
    vec3Normalize(&sunRaysDir);
    light.direction = vec3Multiply(sunRaysDir, 1.0f);


    //Load the OBJ FIle
    loadMesh("../assets/Car 01/Car.obj", "../assets/Car 01/car.png", vec3New(1, 1, 1), vec3New(-4, 0, 8),vec3New(M_PI/3.0, M_PI/3.0, 0),NONE);
    loadMesh("../assets/Car 01/Car.obj", "../assets/Car 01/car_red.png", vec3New(1, 1, 1), vec3New(4, 0, 8),vec3New(M_PI/2.0, 0, 0),NONE);
    loadMesh("../assets/Car 01/Car.obj", "../assets/Car 01/car_blue.png", vec3New(1, 1, 1), vec3New(0, 0, 8),vec3New(0, 0, 0),NONE);
    loadMesh("../assets/surface.obj", "../assets/pool2.png", vec3New(1, 1, 1), vec3New(0, 0, 0), vec3New(0, 0, 0),WATER_EFFECT);
    //loadMesh("../assets/cube.obj", "../assets/MamaHong.png", vec3New(1, 1, 1), vec3New(0, 0, -5),vec3New(0, 0, 0),NONE);






    water_init_default(&gWater);
    gWater.shimmer_u_scale = 0.005f;
    gWater.shimmer_v_scale = 0.005f;


    // --- Camera init ---
    initCamera((vec3_t){0.0f, 1.5f, -3.0f}, (vec3_t){0.0f, 0.0f, 1.0f}); // pos, dir (+Z)
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void processInput(void) {
    const float moveYSpeed = 8.0f * deltaTime; // world Y
    const float movePlanarSpd = 10.0f * deltaTime;
    const float maxPitch = 1.55334306f; // 89°

    // Mouse-look config
    static float mouseSensitivity = 0.0025f;
    static bool invertMouseY = true;
    static bool invertMouseX = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN: {
                SDL_Keycode k = event.key.keysym.sym;
                switch (k) {
                    case SDLK_ESCAPE: isRunning = false;
                        break;

                    // Render modes
                    case SDLK_1: setRenderMode(RENDER_WIRE_VERTEX);
                        break;
                    case SDLK_2: setRenderMode(RENDER_WIRE);
                        break;
                    case SDLK_3: setRenderMode(RENDER_FILL_TRIANGLE);
                        break;
                    case SDLK_4: setRenderMode(RENDER_FILL_TRIANGLE_WIRE);
                        break;
                    case SDLK_5: setRenderMode(RENDER_TEXTURED);
                        break;
                    case SDLK_6: setRenderMode(RENDER_TEXTURED_WIRE);
                        break;

                    // Culling
                    case SDLK_c: setCullMode(CULL_BACKFACE);
                        break;
                    case SDLK_x: setCullMode(CULL_NONE);
                        break;

                    default: break;
                }
                break;
            }

            case SDL_MOUSEMOTION: {
                float yawDelta = mouseSensitivity * (float) event.motion.xrel;
                float pitchDelta = mouseSensitivity * (float) event.motion.yrel;

                if (invertMouseX) yawDelta = -yawDelta;

                if (!invertMouseY) pitchDelta = -pitchDelta;

                rotateCameraYaw(yawDelta);

                float p = getCameraPitch();
                float np = p + pitchDelta;
                if (np < -maxPitch) np = -maxPitch;
                if (np > maxPitch) np = maxPitch;
                rotateCameraPitch(np - p);
                break;
            }
        }
    }

    // --------- Keyboard movement---------
    const Uint8 *ks = SDL_GetKeyboardState(NULL);

    // Build planar basis from current facing
    vec3_t forward = getCameraDirection();
    forward.y = 0.0f;
    if (vec3Length(forward) > 0.0f)
        vec3Normalize(&forward);

    vec3_t up = (vec3_t){0, 1, 0};
    vec3_t right = vec3Cross(forward, up);
    if (vec3Length(right) > 0.0f)
        vec3Normalize(&right);

    vec3_t move = (vec3_t){0, 0, 0};

    // FPS movement: W/S forward/back, A/D strafe left/right
    if (ks[SDL_SCANCODE_W]) move = vec3Add(move, vec3Multiply(forward, movePlanarSpd));
    if (ks[SDL_SCANCODE_S]) move = vec3Subtract(move, vec3Multiply(forward, movePlanarSpd));
    if (ks[SDL_SCANCODE_A]) move = vec3Add(move, vec3Multiply(right, movePlanarSpd)); // strafe right
    if (ks[SDL_SCANCODE_D]) move = vec3Subtract(move, vec3Multiply(right, movePlanarSpd)); // strafe left

    // Space up, Ctrl down
    if (ks[SDL_SCANCODE_SPACE]) move.y += moveYSpeed;
    if (ks[SDL_SCANCODE_LCTRL] || ks[SDL_SCANCODE_RCTRL]) move.y -= moveYSpeed;

    if (move.x || move.y || move.z) {
        vec3_t pos = getCameraPosition();
        pos = vec3Add(pos, move);
        updateCameraPosition(pos);
        updateCameraForwardVelocity(move);
    }
}

void update(void) {
    //This snippet limits the FPS
    //This was the only solution that works on MacOS and SDLdelay (since MacOS calculates ticks differently than other OS)
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - previousFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        if (timeToWait > 5) {
            SDL_Delay(timeToWait - 5);
        }

        while (!SDL_GetTicks() - previousFrameTime < FRAME_TARGET_TIME);
    }
    //Get a delta time factor converted to seconds to be used to update our game obj
    deltaTime = (SDL_GetTicks() - previousFrameTime) / 1000.0;
    previousFrameTime = SDL_GetTicks();

    gTimeSeconds += deltaTime;



    //Initialize the counter of triangles for the current frame
    numTrianglesToRender = 0;


    //Loop all the meshes
    for (int meshIndex = 0; meshIndex < getNumOfMeshes(); meshIndex++) {
        mesh_t* allOffTheMeshes = getMesh(meshIndex);
        //mesh_t* m1 = getMesh(0);
        //mesh_t* m2 = getMesh(1);
        //mesh_t* m3 = getMesh(2);



        //Change the mesh scale, rotation, translation....  every frame

        /*
        m1->rotation.y += 1.0 * deltaTime;
        m2->rotation.y += 1.0 * deltaTime;
        m3->rotation.y += 1.0 * deltaTime;
        */


        //mesh.rotation.y += 1.0 * deltaTime;
        //mesh.rotation.z += 0.01 * deltaTime;
        //mesh.scale.x += 0.002;
        //mesh.scale.y += 0.001;
        //mesh.translation.x += 1.0 * deltaTime;
        //mesh.translation.z = 6.00;

        processGraphicsPipelineStages(allOffTheMeshes);

    }
}


void render(void) {
    //Fresh Start
    clearColorBuffer(0xFF000000);
    clearZBuffer();

    for (int i = 0; i < numTrianglesToRender; i++) {
        triangle_t triangle = trianglesToRender[i];

        //Filled Triangle
        if (shouldRenderFilledTriangles()) {
            drawFilledTriangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }

        //Textured Triangle
        if (shouldRenderTexturedTriangles()) {
            drawTexturedTriangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.texCoords[0].u, triangle.texCoords[0].v, //Vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.texCoords[1].u, triangle.texCoords[1].v, //Vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.texCoords[2].u, triangle.texCoords[2].v, //Vertex C
                triangle.texture,
                triangle.intensities[0], triangle.intensities[1], triangle.intensities[2],
                triangle.worldXZ[0],triangle.worldXZ[1],triangle.worldXZ[2],triangle.effect

             );
        }

        //Unfilled Triangles for wireframe view
        if (shouldRenderWireframeTriangles()) {
            triangle_t triangle = trianglesToRender[i];
            drawTriangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF6A0DAD
            );
        }

        if (shouldRenderVertex()) {
            drawRect(triangle.points[0].x - 5, triangle.points[0].y - 5, 10, 10, 0xFFFF0000);
            drawRect(triangle.points[1].x - 5, triangle.points[1].y - 5, 10, 10, 0xFFFF0000);
            drawRect(triangle.points[2].x - 5, triangle.points[2].y - 5, 10, 10, 0xFFFF0000);
        }
    }
    //Its ready to be rendered now
    renderColorBuffer();
}


void processGraphicsPipelineStages(mesh_t* mesh) {

        //Create a scale matrix that will be used to multiply the mesh vertices
        mat4_t scaleMatrix = mat4MakeScale(mesh->scale.x, mesh->scale.y, mesh->scale.z);

        //Create a translation matrix that will be used to multiply the mesh vertices
        mat4_t translationMatrix = mat4MakeTranslation(mesh->translation.x, mesh -> translation.y, mesh -> translation.z);
        //Create  rotation matrixws that will be used to multiply the mesh vertices
        mat4_t rotationMatrixX = mat4MakeRotationX(mesh -> rotation.x);
        mat4_t rotationMatrixY = mat4MakeRotationY(mesh -> rotation.y);
        mat4_t rotationMatrixZ = mat4MakeRotationZ(mesh -> rotation.z);

        viewMatrix = mat4LookAt(getCameraPosition(), getCameraLookatTarget(), (vec3_t){0, 1, 0});

        // Build world matrix once for this mesh (same order you use later)
        worldMatrix = mat4Identity();
        worldMatrix = mat4MultipMat4(scaleMatrix, worldMatrix);
        worldMatrix = mat4MultipMat4(rotationMatrixZ, worldMatrix);
        worldMatrix = mat4MultipMat4(rotationMatrixY, worldMatrix);
        worldMatrix = mat4MultipMat4(rotationMatrixX, worldMatrix);
        worldMatrix = mat4MultipMat4(translationMatrix, worldMatrix);

        int numVerts = array_length(mesh->vertices);
        int numFaces = array_length(mesh -> faces);

        vec3_t* accNormals = (vec3_t*)calloc(numVerts, sizeof(vec3_t));
        int*    counts     = (int*)   calloc(numVerts, sizeof(int));

        for (int i = 0; i < numFaces; i++) {
            face_t f = mesh->faces[i];
            vec3_t A = mesh->vertices[f.a];
            vec3_t B = mesh->vertices[f.b];
            vec3_t C = mesh->vertices[f.c];
            vec3_t e1 = vec3Subtract(B, A);
            vec3_t e2 = vec3Subtract(C, A);
            vec3_t fn = vec3Cross(e1, e2); // area-weighted
            accNormals[f.a] = vec3Add(accNormals[f.a], fn);
            accNormals[f.b] = vec3Add(accNormals[f.b], fn);
            accNormals[f.c] = vec3Add(accNormals[f.c], fn);
            counts[f.a]++; counts[f.b]++; counts[f.c]++;
        }
        for (int v = 0; v < numVerts; v++) {
            if (counts[v] > 0) vec3Normalize(&accNormals[v]);
        }

        vec3_t* worldNormals = (vec3_t*)malloc(numVerts * sizeof(vec3_t));
        for (int v = 0; v < numVerts; v++) {
            vec4_t n4 = (vec4_t){accNormals[v].x, accNormals[v].y, accNormals[v].z, 0.0f};
            vec4_t nw = mat4MultipVec4(worldMatrix, n4);
            worldNormals[v] = vec4ToVec3(nw);
            vec3Normalize(&worldNormals[v]);
        }

        float ambient = 0.05f;
        vec3_t L = light.direction;
        float* vertI = (float*)malloc(numVerts * sizeof(float));
        for (int v = 0; v < numVerts; v++) {
            float ndotl = vec3DotProduct(worldNormals[v], L);
            if (ndotl < 0.0f) ndotl = 0.0f;
            vertI[v] = ambient + (1.0f - ambient) * ndotl;
        }

        free(accNormals);
        free(worldNormals);

        //Goes through triangular faces
        for (int i = 0; i < numFaces; i++) {
            face_t meshFace = mesh -> faces[i];
            vec3_t faceVertices[3];
            faceVertices[0] = mesh -> vertices[meshFace.a];
            faceVertices[1] = mesh -> vertices[meshFace.b];
            faceVertices[2] = mesh -> vertices[meshFace.c];

            vec4_t worldVertices[3];
            vec4_t viewVertices[3];
            vec4_t transformedVertices[3];

            vec2_t worldXZ[3];

            //loop all three vertices of the face and aplly rotations
            for (int j = 0; j < 3; j++) {
                vec4_t v = vec3ToVec4(faceVertices[j]);   // w=1

                // Use the prebuilt worldMatrix
                vec4_t vWorld = mat4MultipVec4(worldMatrix, v);
                worldVertices[j] = vWorld;

                //For water rendering only
                worldXZ[j] = (vec2_t){vWorld.x, vWorld.z};
                vec2Normalize(&worldXZ[j]);

                // Then view transform once
                vec4_t vView  = mat4MultipVec4(viewMatrix, vWorld);
                viewVertices[j] = vView;

                //clipper works in view space
                transformedVertices[j] = vView;
            }



            //Calculate the triangle face normal

            vec3_t nView = getTriangleNormal(viewVertices); // For culling (view space)


            //-------------- Make the backface culling ---------
            if (isCullBackface()) {
                //Find the vector between point A in the triangle (can be any point) and the origin (For Camera ray)
                vec3_t origin = {0, 0, 0};
                vec3_t cameraRay = vec3Subtract(origin, vec4ToVec3(viewVertices[0]));

                //Calculate the alignment between the face and the camera
                float viewMatrixAndCameraRayDotProduct = vec3DotProduct(cameraRay, nView);
                if (viewMatrixAndCameraRayDotProduct < 0)
                    continue;
            }

            // Per-vertex intensities for this face (from precomputed array)
            float iA = vertI[meshFace.a];
            float iB = vertI[meshFace.b];
            float iC = vertI[meshFace.c];



            //Create a polygon from the original transform
            polygon_t polygon = createPolygonFromTriangle(
                vec4ToVec3(transformedVertices[0]),
                vec4ToVec3(transformedVertices[1]),
                vec4ToVec3(transformedVertices[2]),
                meshFace.aUv,
                meshFace.bUv,
                meshFace.cUv,
                iA,iB,iC,
                worldXZ[0],worldXZ[1],worldXZ[2]
            );
            //Clip the polygon
            clipPolygon(&polygon);

            //break the polygon into triangles
            triangle_t trianglesAfterClipping[MAX_NUM_POLY_TRIANGLES];
            int numTrianglesAfterClipping = 0;

            trianglesFromPolygon(&polygon, trianglesAfterClipping, &numTrianglesAfterClipping);

            //Loops all the assembled triangles
            for (int t = 0; t < numTrianglesAfterClipping; t++) {
                triangle_t triangleAfterClipping = trianglesAfterClipping[t];

                // Gouraud: use intensities carried through the clipper
                float Itri[3] = {
                    triangleAfterClipping.intensities[0],
                    triangleAfterClipping.intensities[1],
                    triangleAfterClipping.intensities[2]
                };


                vec4_t projectedPoints[3];
                //Now we do projection and loop all the faces if it is not at the back
                for (int j = 0; j < 3; j++) {
                    projectedPoints[j] = mat4MultipVec4Project(projMatrix, triangleAfterClipping.points[j]);

                    //Scale them
                    projectedPoints[j].x *= (getWindowWidth() / 2.0);
                    projectedPoints[j].y *= (getWindowHeight() / 2.0);

                    projectedPoints[j].y *= -1;

                    //After projecting them (And scaling them) move  them to the middle of the screen
                    projectedPoints[j].x += (getWindowWidth() / 2.0);
                    projectedPoints[j].y += (getWindowHeight() / 2.0);
                }

                //Light and Shading
                float triIntensityAvg = (Itri[0] + Itri[1] + Itri[2]) / 3.0f;
                uint32_t shadedColor = lightApplyIntensity(meshFace.color, triIntensityAvg);
                //Push it
                triangle_t triangleToRender = {
                    .points = {
                        {projectedPoints[0].x, projectedPoints[0].y, projectedPoints[0].z, projectedPoints[0].w},
                        {projectedPoints[1].x, projectedPoints[1].y, projectedPoints[1].z, projectedPoints[1].w},
                        {projectedPoints[2].x, projectedPoints[2].y, projectedPoints[2].z, projectedPoints[2].w},
                    },
                    .texCoords = {
                        {triangleAfterClipping.texCoords[0].u, triangleAfterClipping.texCoords[0].v},
                        {triangleAfterClipping.texCoords[1].u, triangleAfterClipping.texCoords[1].v},
                        {triangleAfterClipping.texCoords[2].u, triangleAfterClipping.texCoords[2].v}
                    },
                    .color = shadedColor,
                    .texture = mesh ->texture,
                    .intensity = triIntensityAvg,
                    .intensities = { Itri[0], Itri[1], Itri[2] },
                    .worldXZ = {
                        {triangleAfterClipping.worldXZ[0].x,triangleAfterClipping.worldXZ[0].y},
                        {triangleAfterClipping.worldXZ[1].x,triangleAfterClipping.worldXZ[1].y},
                        {triangleAfterClipping.worldXZ[2].x,triangleAfterClipping.worldXZ[2].y},
                    },
                    .effect = mesh ->effectMode
                };

                if (numTrianglesToRender < MAX_TRIANGLES_PER_MESH) {
                    trianglesToRender[numTrianglesToRender] = triangleToRender;
                    numTrianglesToRender++;
                }
            }
        }

        free(vertI);
}

void freeResources(void) {
    freeMeshes();
    destroyWindow();
    SDL_SetRelativeMouseMode(SDL_FALSE);

}
