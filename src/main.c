#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"


void setup(void);

void processInput(void);

void update(void);

void render(void);

vec2_t project(vec3_t point);


bool isRunning = false;

vec3_t cameraPosition = {.x=0,.y=0,.z=-5};
vec3_t cubeRotation = {.x = 0, .y = 0, .z = 0};

float fovFactor = 640;

uint32_t previousFrameTime = 0;
triangle_t trianglesToRender[N_MESH_FACES];


int main(void) {
    isRunning = initializeWindow();

    setup();



    while (isRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();


    return 0;
}


void setup(void) {
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
            break;
    }
}
//This function receives a 3D vector and returns a projected 2D point
vec2_t project(vec3_t point) {
    vec2_t projectedPoint = {
        .x = (fovFactor * point.x) / point.z,
        .y = (fovFactor * point.y) / point.z
    };
    return projectedPoint;
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

    cubeRotation.x+= 0.01;
    cubeRotation.y+= 0.01;
    cubeRotation.z+= 0.01;


    //Goes through triangular faces
    for (int i = 0; i < N_MESH_FACES; i ++) {
        face_t meshFace = meshFaces[i];
        vec3_t faceVertices[3];
        faceVertices[0] = meshVertices[meshFace.a - 1];
        faceVertices[1] = meshVertices[meshFace.b - 1];
        faceVertices[2] = meshVertices[meshFace.c - 1];

        triangle_t projectedTriangle;

        //loop all three vertices of the face and aplly rotations
        for (int j = 0 ;j < 3; j ++) {
            vec3_t transformedVertex = faceVertices[j];
            transformedVertex = rotateAroundX(transformedVertex,cubeRotation.x);
            transformedVertex = rotateAroundY(transformedVertex,cubeRotation.y);
            transformedVertex = rotateAroundZ(transformedVertex,cubeRotation.z);

            //Translate the vertex avay from the camera
            transformedVertex.z-= cameraPosition.z;

            vec2_t projectedPoint = project(transformedVertex);

            //After projecting them (And before saving them) move and scale them to the middle of the screen
            projectedPoint.x += (windowWidth/2);
            projectedPoint.y += (windowHeight/2);

            //Now we can pass it to this arrray
            projectedTriangle.points[j] = projectedPoint;

        }

        // Lastly save the projected triangle in the array of triangles to render
        trianglesToRender[i] =projectedTriangle;

    }


}


void render(void) {


    clearColorBuffer(0xFF000000);


    //drawGrid();


    //Loop all the projected triangles and connect them with lines
    for (int i = 0 ; i <N_MESH_FACES ; i ++) {
        triangle_t triangle = trianglesToRender[i];
        drawLine(triangle.points[0].x,triangle.points[0].y,triangle.points[1].x,triangle.points[1].y,0xFFFFFF00);
        drawLine(triangle.points[1].x,triangle.points[1].y,triangle.points[2].x,triangle.points[2].y,0xFFFFFF00);
        drawLine(triangle.points[2].x,triangle.points[2].y,triangle.points[0].x,triangle.points[0].y,0xFFFFFF00);
    }
    //Its ready to be rendered now
    renderColorBuffer();

    SDL_RenderPresent(renderer);
}
