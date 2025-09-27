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


void setup(void);

void processInput(void);

void update(void);

void render(void);


void freeResources(void);

#define MAX_TRIANGLES_PER_MESH 10000


bool isRunning = false;
uint32_t previousFrameTime = 0;
float deltaTime = 0;


mat4_t projMatrix;
mat4_t viewMatrix;
mat4_t worldMatrix;


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

    destroyWindow();
    freeResources();


    return 0;
}


void setup(void) {
    renderMode = RENDER_WIRE;
    cullMode = CULL_BACKFACE;

    //Allocate the required memory to hold the color  and zbuffers
    colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * windowWidth * windowHeight);
    zBuffer = (float *) malloc(sizeof(float) * windowWidth * windowHeight);

    //Creating a SDL texture that is used to display the color buffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        windowWidth,
        windowHeight
    );

    float aspectY = (float) windowHeight / (float) windowWidth;
    float aspectX = (float) windowWidth / (float) windowHeight;
    float fovY = M_PI / 3.0; //60 degrees to radians
    float fovX = 2.0 * atan(tan(fovY/2) * aspectX);
    float zNear = 0.1;
    float zFar = 100.0;

    projMatrix = mat4MakePerspective(fovY, aspectY, zNear, zFar);
    initFrustumPlanes(fovX, fovY, zNear, zFar);


    vec3_t sunRaysDir = {0.0f, 0.0f, 1.0f};
    vec3Normalize(&sunRaysDir);
    light.direction = vec3Multiply(sunRaysDir, -1.0f);


    //Load the OBJ FIle
    //loadCubeMeshData();

    //loadObjFileData("../assets/Car 01/Car.obj");
    //loadPngTextureData("../assets/Car 01/car.png");

    loadObjFileData("../assets/cube.obj");
    //loadPngTextureData("../assets/MamaHong.png");
    loadPngTextureData("../assets/upscaled.png");
}

void processInput(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isRunning = false;
            if (event.key.keysym.sym == SDLK_1)
                renderMode = RENDER_WIRE_VERTEX;
            if (event.key.keysym.sym == SDLK_2)
                renderMode = RENDER_WIRE;
            if (event.key.keysym.sym == SDLK_3)
                renderMode = RENDER_FILL_TRIANGLE;
            if (event.key.keysym.sym == SDLK_4)
                renderMode = RENDER_FILL_TRIANGLE_WIRE;
            if (event.key.keysym.sym == SDLK_5)
                renderMode = RENDER_TEXTURED;
            if (event.key.keysym.sym == SDLK_6)
                renderMode = RENDER_TEXTURED_WIRE;
            if (event.key.keysym.sym == SDLK_c)
                cullMode = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_x)
                cullMode = CULL_NONE;
            if (event.key.keysym.sym == SDLK_UP)
                camera.position.y += 8.0 * deltaTime;
            if (event.key.keysym.sym == SDLK_DOWN)
                camera.position.y -= 8.0 * deltaTime;
            if (event.key.keysym.sym == SDLK_a)
                camera.yaw -= 8.0 * deltaTime;
            if (event.key.keysym.sym == SDLK_d)
                camera.yaw += 8.0 * deltaTime;
            if (event.key.keysym.sym == SDLK_w) {
                camera.forwardVelocity = vec3Multiply(camera.direction, 20.0 * deltaTime);
                camera.position = vec3Add(camera.position, camera.forwardVelocity);
            }
            if (event.key.keysym.sym == SDLK_s) {
                camera.forwardVelocity = vec3Multiply(camera.direction, 20.0 * deltaTime);
                camera.position = vec3Subtract(camera.position, camera.forwardVelocity);
            }

            break;
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

    //Initialize the counter of triangles for the current frame
    numTrianglesToRender = 0;

    //Change the mesh scale, rotation, translation....  every frame

    //mesh.rotation.x += 0.01 * deltaTime;
    //mesh.rotation.y += 1.0 * deltaTime;
    //mesh.rotation.z += 0.01 * deltaTime;


    //mesh.scale.x += 0.002;
    //mesh.scale.y += 0.001;


    //mesh.translation.x += 1.0 * deltaTime;
    mesh.translation.z = 6.00;


    //Initialize the target looking at the positive z axis
    vec3_t target = {0, 0, 1};
    mat4_t cameraYawRotation = mat4MakeRotationY(camera.yaw);
    camera.direction = vec4ToVec3(mat4MultipVec4(cameraYawRotation, vec3ToVec4(target)));

    //Offset the camera position
    target = vec3Add(camera.position, camera.direction);
    vec3_t upDirection = {0, 1, 0};

    //Create a wiev matrix
    viewMatrix = mat4LookAt(camera.position, target, upDirection);

    //Create a scale matrix that will be used to multiply the mesh vertices
    mat4_t scaleMatrix = mat4MakeScale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    //Create a translation matrix that will be used to multiply the mesh vertices
    mat4_t translationMatrix = mat4MakeTranslation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    //Create  rotation matrixws that will be used to multiply the mesh vertices
    mat4_t rotationMatrixX = mat4MakeRotationX(mesh.rotation.x);
    mat4_t rotationMatrixY = mat4MakeRotationY(mesh.rotation.y);
    mat4_t rotationMatrixZ = mat4MakeRotationZ(mesh.rotation.z);


    int numFaces = array_length(mesh.faces);
    //Goes through triangular faces
    for (int i = 0; i < numFaces; i++) {


        face_t meshFace = mesh.faces[i];
        vec3_t faceVertices[3];
        faceVertices[0] = mesh.vertices[meshFace.a];
        faceVertices[1] = mesh.vertices[meshFace.b];
        faceVertices[2] = mesh.vertices[meshFace.c];


        vec4_t transformedVertices[3];

        //loop all three vertices of the face and aplly rotations
        for (int j = 0; j < 3; j++) {
            vec4_t transformedVertex = vec3ToVec4(faceVertices[j]);


            //Create  a world matrix combining scale, rotation and translation (The order matters a lott!!!!)
            worldMatrix = mat4Identity();
            worldMatrix = mat4MultipMat4(scaleMatrix, worldMatrix);
            worldMatrix = mat4MultipMat4(rotationMatrixZ, worldMatrix);
            worldMatrix = mat4MultipMat4(rotationMatrixY, worldMatrix);
            worldMatrix = mat4MultipMat4(rotationMatrixX, worldMatrix);
            worldMatrix = mat4MultipMat4(translationMatrix, worldMatrix);

            //Multiply the world matrix by the original vector
            transformedVertex = mat4MultipVec4(worldMatrix, transformedVertex);

            //Multiply the view matrix by the vector to transform the scene to camera space
            transformedVertex = mat4MultipVec4(viewMatrix, transformedVertex);

            //Save it into the global loop
            transformedVertices[j] = transformedVertex;
        }


        //Dont forget this is in CLOCKWISE ORDER
        vec3_t vectorA = vec4ToVec3(transformedVertices[0]);
        vec3_t vectorB = vec4ToVec3(transformedVertices[1]);
        vec3_t vectorC = vec4ToVec3(transformedVertices[2]);

        //Find the vector between points
        vec3_t vectorAB = vec3Subtract(vectorB, vectorA);
        vec3_t vectorAC = vec3Subtract(vectorC, vectorA);
        vec3Normalize(&vectorAB);
        vec3Normalize(&vectorAC);

        //Compute the face normal using the cross product
        vec3_t normal = vec3Cross(vectorAB, vectorAC);

        //Normalize the face normal
        vec3Normalize(&normal);

        //Find the vector between point A in the triangle (can be any point) and the origin (For Camera ray)

        vec3_t origin = {0, 0, 0};
        vec3_t cameraRay = vec3Subtract(origin, vectorA);

        //Calculate the alignment between the face and the camera
        float faceNormalAndCameraRayDotProduct = vec3DotProduct(cameraRay, normal);

        //-------------- Make the backface culling ---------
        if (faceNormalAndCameraRayDotProduct < 0 && cullMode == CULL_BACKFACE)
            continue; // we bypass everything

        //Create a polygon from the original transform
        polygon_t polygon = createPolygonFromTriangle(
            vec4ToVec3(transformedVertices[0]),
            vec4ToVec3(transformedVertices[1]),
            vec4ToVec3(transformedVertices[2]),
            meshFace.aUv,
            meshFace.bUv,
            meshFace.cUv
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

            vec4_t projectedPoints[3];
            //Now we do projection and loop all the faces if it is not at the back
            for (int j = 0; j < 3; j++) {
                projectedPoints[j] = mat4MultipVec4Project(projMatrix, triangleAfterClipping.points[j]);

                //Scale them
                projectedPoints[j].x *= (windowWidth / 2.0);
                projectedPoints[j].y *= (windowHeight / 2.0);

                projectedPoints[j].y *= -1;


                //After projecting them (And scaling them) move  them to the middle of the screen
                projectedPoints[j].x += (windowWidth / 2.0);
                projectedPoints[j].y += (windowHeight / 2.0);
            }

            //Light and Shading
            float faceNormalAndLightRayDotProduct = vec3DotProduct(normal, light.direction);
            float ambient = 0.2f; // Base ambient light
            float diffuse = fmax(0.0f, faceNormalAndLightRayDotProduct);
            float intensity = ambient + (1.0f - ambient) * diffuse;
            uint32_t shadedColor = lightApplyIntensity(meshFace.color, intensity);

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

            };

            if (numTrianglesToRender < MAX_TRIANGLES_PER_MESH) {
                trianglesToRender[numTrianglesToRender] = triangleToRender;
                numTrianglesToRender++;
            }
        }
    }
}


void render(void) {
    //drawGrid();


    for (int i = 0; i < numTrianglesToRender; i++) {
        triangle_t triangle = trianglesToRender[i];

        //Filled Triangle
        if (renderMode == RENDER_FILL_TRIANGLE || renderMode == RENDER_FILL_TRIANGLE_WIRE) {
            drawFilledTriangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }

        //Textured Triangle
        if (renderMode == RENDER_TEXTURED || renderMode == RENDER_TEXTURED_WIRE) {
            drawTexturedTriangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.texCoords[0].u, triangle.texCoords[0].v, //Vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.texCoords[1].u, triangle.texCoords[1].v, //Vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.texCoords[2].u, triangle.texCoords[2].v, //Vertex C
                meshTexture

            );
        }

        //Unfilled Triangles for wireframe view
        if (renderMode == RENDER_WIRE || renderMode == RENDER_WIRE_VERTEX || renderMode == RENDER_FILL_TRIANGLE_WIRE ||
            renderMode == RENDER_TEXTURED_WIRE) {
            triangle_t triangle = trianglesToRender[i];
            drawTriangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF6A0DAD
            );
        }

        if (renderMode == RENDER_WIRE_VERTEX) {
            drawRect(triangle.points[0].x - 5, triangle.points[0].y - 5, 10, 10, 0xFFFF0000);
            drawRect(triangle.points[1].x - 5, triangle.points[1].y - 5, 10, 10, 0xFFFF0000);
            drawRect(triangle.points[2].x - 5, triangle.points[2].y - 5, 10, 10, 0xFFFF0000);
        }
    }


    //Its ready to be rendered now
    renderColorBuffer();
    clearColorBuffer(0xFF000000);
    clearZBuffer();
    SDL_RenderPresent(renderer);
}

void freeResources(void) {
    free(colorBuffer);
    free(zBuffer);
    upng_free(pngTexture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}
