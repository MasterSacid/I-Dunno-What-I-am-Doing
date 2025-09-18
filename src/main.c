#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"


void setup(void);

void processInput(void);

void update(void);

void render(void);


void freeResources(void);

int compareFaceDepth(const void *a, const void *b);


bool isRunning = false;

vec3_t cameraPosition = {0, 0, 0};

mat4_t projMatrix;
uint32_t previousFrameTime = 0;

triangle_t *trianglesToRender = NULL;
light_t light = {.position={0,0,5}};


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

    //Allocate the required memory to hold the color buffer
    colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * windowWidth * windowHeight);

    //Creating a SDL texture that is used to display the color buffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        windowWidth,
        windowHeight
    );

    //initialize the perspective projection matrix
    float fov = M_PI/3.0; //60 degrees to radians
    float aspect = (float)windowHeight / (float)windowWidth;
    float zNear= 0.1;
    float zFar= 100.0;
    projMatrix= mat4MakePerspective(fov,aspect,zNear,zFar);

    vec3_t sunRaysDir = { 0.0f,0.0f,1.0f};
    vec3Normalize(&sunRaysDir);
    light.direction = vec3Multiply(sunRaysDir,-1.0f);

    //loadCubeMeshData();
    loadObjFileData("../assets/Car 01/Car.obj");
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
            if (event.key.keysym.sym == SDLK_c)
                cullMode = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_d)
                cullMode = CULL_NONE;
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
    previousFrameTime = SDL_GetTicks();

    //Initialize the array of triangles
    trianglesToRender = NULL;

    //Change the mesh scale, rotation, translation....  every frame

    //mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    //mesh.rotation.z += 0.01;


    //mesh.scale.x += 0.002;
    //mesh.scale.y += 0.001;


    //mesh.translation.x += 0.01;
    mesh.translation.z= 6.00;



    //Create a scale matrix that will be used to multiply the mesh vertices
    mat4_t scaleMatrix = mat4MakeScale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    //Create a translation matrix that will be used to multiply the mesh vertices
    mat4_t translationMatrix = mat4MakeTranslation(mesh.translation.x,mesh.translation.y,mesh.translation.z);
    //Create  rotation matrixws that will be used to multiply the mesh vertices
    mat4_t rotationMatrixX = mat4MakeRotationX(mesh.rotation.x);
    mat4_t rotationMatrixY = mat4MakeRotationY(mesh.rotation.y);
    mat4_t rotationMatrixZ = mat4MakeRotationZ(mesh.rotation.z);



    int numFaces = array_length(mesh.faces);
    //Goes through triangular faces
    for (int i = 0; i < numFaces; i++) {
        face_t meshFace = mesh.faces[i];
        vec3_t faceVertices[3];
        faceVertices[0] = mesh.vertices[meshFace.a - 1];
        faceVertices[1] = mesh.vertices[meshFace.b - 1];
        faceVertices[2] = mesh.vertices[meshFace.c - 1];


        vec4_t transformedVertices[3];

        //loop all three vertices of the face and aplly rotations
        for (int j = 0; j < 3; j++) {
            vec4_t transformedVertex = vec3ToVec4(faceVertices[j]);

            //Create  a world matrix combining scale, rotation and translation (The order matters a lott!!!!)
            mat4_t worldMatrix = mat4Identity();
            worldMatrix =  mat4MultipMat4(scaleMatrix,worldMatrix);
            worldMatrix =  mat4MultipMat4(rotationMatrixZ,worldMatrix);
            worldMatrix =  mat4MultipMat4(rotationMatrixY,worldMatrix);
            worldMatrix =  mat4MultipMat4(rotationMatrixX,worldMatrix);
            worldMatrix =  mat4MultipMat4(translationMatrix,worldMatrix);

            //Multiply the world matrix by the original vector
            transformedVertex = mat4MultipVec4(worldMatrix,transformedVertex);

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

            //Find the vector between point A in the triangle (can be any point) and the camera origin (For Camera ray)
            vec3_t cameraRay = vec3Subtract(cameraPosition, vectorA);

            //Calculate the alignment between the face and the camera
            float faceNormalAndCameraRayDotProduct = vec3DotProduct(cameraRay, normal);

            //-------------- Make the backface culling ---------
            if (faceNormalAndCameraRayDotProduct < 0 && cullMode == CULL_BACKFACE)
                continue; // we bypass everything


        vec4_t projectedPoints[3];
        //Now we do projection and loop all the faces if it is not at the back
        for (int j = 0; j < 3; j++) {
            projectedPoints[j] = mat4MultipVec4Project(projMatrix,transformedVertices[j]);

            //Scale them
            projectedPoints[j].x *=(windowWidth/2.0);
            projectedPoints[j].y *=(windowHeight/2.0);

            //After projecting them (And scaling them) move  them to the middle of the screen
            projectedPoints[j].x += (windowWidth / 2.0);
            projectedPoints[j].y += (windowHeight / 2.0);


        }

        //Calculate the average depth for each face based on the vertices after transformation
        float avgDepth = (transformedVertices[0].z + transformedVertices[1].z + transformedVertices[2].z) / 3.0;


        //Light       Shading
        float faceNormalAndLightRayDotProduct = vec3DotProduct(normal,light.direction);
        float ambient = 0.2f;  // Base ambient light
        float diffuse = fmax(0.0f, faceNormalAndLightRayDotProduct);
        float intensity = ambient + (1.0f - ambient) * diffuse;
        uint32_t shadedColor = lightApplyIntensity(meshFace.color,intensity);

        triangle_t projectedTriangle = {
            .points = {
                {projectedPoints[0].x, projectedPoints[0].y},
                {projectedPoints[1].x, projectedPoints[1].y},
                {projectedPoints[2].x, projectedPoints[2].y},
            },
            .color = shadedColor,
            .avgDepth = avgDepth
        };

        // Lastly save the projected triangle in the array of triangles to render
        array_push(trianglesToRender, projectedTriangle);
    }

    //Sorting the faces and using painters algorithm to determine the order
    int numTrianglesToSort = array_length(trianglesToRender);
    qsort(trianglesToRender, numTrianglesToSort, sizeof(triangle_t), compareFaceDepth);
}


void render(void) {
    clearColorBuffer(0xFF000000);


    //drawGrid();


    int numOfTriangles = array_length(trianglesToRender);
    for (int i = 0; i < numOfTriangles; i++) {
        triangle_t triangle = trianglesToRender[i];

        //Filled Triangle
        if (renderMode == RENDER_FILL_TRIANGLE || renderMode == RENDER_FILL_TRIANGLE_WIRE) {
            drawFilledTriangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color
            );
        }
        if (renderMode == RENDER_WIRE || renderMode == RENDER_WIRE_VERTEX || renderMode == RENDER_FILL_TRIANGLE_WIRE) {
            //Unfilled Triangles for wireframe view

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


    //FREE THE ARRAY FIRST!!!!!!!!!!!!!!!!
    array_free(trianglesToRender);
    //Its ready to be rendered now
    renderColorBuffer();

    SDL_RenderPresent(renderer);
}

void freeResources(void) {
    free(colorBuffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int compareFaceDepth(const void *a, const void *b) {
    const triangle_t *triangleA = (const triangle_t *) a;
    const triangle_t *triangleB = (const triangle_t *) b;

    if (triangleA->avgDepth > triangleB->avgDepth) {
        return -1; // triangleA comes before triangleB
    } else if (triangleA->avgDepth < triangleB->avgDepth) {
        return 1; // triangleB comes before triangleA
    }
    return 0; // Equal depth
}
