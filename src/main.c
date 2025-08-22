#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"


void setup(void);

void processInput(void);

void update(void);

void render(void);

vec2_t project(vec3_t point);


bool isRunning = false;
//ARRAY OF VECTORS
const int N_POINTS = 9*9*9;
vec3_t cubePoints[N_POINTS];
vec2_t projectedPoints [N_POINTS];

vec3_t cameraPosition = {.x=0,.y=0,.z=-5};
vec3_t cubeRotation = {.x = 0, .y = 0, .z = 0};

float fovFactor = 640;


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

    int pointCount = 0;

    //Load the array of vectors here
    for (float x = -1; x<= 1; x += 0.25) {
        for (float y = -1; y<= 1; y += 0.25) {
            for (float z = -1;z<= 1; z += 0.25) {
                vec3_t newPoint = {.x=x,.y=y,.z=z};
                cubePoints[pointCount++] = newPoint;
            }
        }
    }
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
    cubeRotation.x+= 0.001;
    cubeRotation.y+= 0.001;
    cubeRotation.z+= 0.001;

    //Original Cube Points
    for (int i = 0 ; i < N_POINTS ; i++) {
        vec3_t point = cubePoints[i];

        vec3_t transformedPoint = rotateAroundX(point,cubeRotation.x);
        transformedPoint = rotateAroundY(transformedPoint,cubeRotation.y);
        transformedPoint = rotateAroundZ(transformedPoint,cubeRotation.z);



        //Move the points away from the camera
        transformedPoint.z -= cameraPosition.z;
        //Project the current point (its single point)
        vec2_t projectedPoint = project(transformedPoint);
        //Then save them in the array of projected points
        projectedPoints[i] = projectedPoint;
    }
}


void render(void) {


    clearColorBuffer(0xFF000000);


    //drawGrid();

    //Loop all the projected points and trender them later on
    for (int i = 0 ; i <N_POINTS ; i ++) {
        vec2_t projectedPoint = projectedPoints[i];
        drawRect(
            projectedPoint.x + (windowWidth /2),
            projectedPoint.y + (windowHeight/2),
            4,
            4,
            0xFFFFFF00);
    }



    renderColorBuffer();

    SDL_RenderPresent(renderer);
}
