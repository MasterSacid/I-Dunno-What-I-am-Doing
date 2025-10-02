#include "display.h"


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static SDL_Texture* colorBufferTexture = NULL;
static uint32_t* colorBuffer = NULL;
static float* zBuffer = NULL;

static int windowWidth = 320;
static int windowHeight = 200;


static int renderMode = 0;
static int cullMode = 0;
static int effectMode = 0;


//Getters and Setters
int getWindowWidth(void) {
    return windowWidth;
}
int getWindowHeight(void) {
    return windowHeight;
}

void setRenderMode(int mode) {
    renderMode = mode;

}
void setCullMode(int mode) {
    cullMode = mode;
}

void setEffectMode(int mode) {
    effectMode = mode;
}
int getEffect(void) {
    return effectMode;
}


float getZBufferAt(int x, int y) {
    if (x<0 || x> windowWidth || y < 0 || y> windowHeight)
        return 1.0;
    return zBuffer[(windowWidth*y) + x];
}
void updateZBufferAt(int x, int y, float value) {
    if (x<0 || x> windowWidth || y < 0 || y> windowHeight)
        return;
    zBuffer[(windowWidth*y)+x] = value;
}






bool isCullBackface(void) {
    return cullMode == CULL_BACKFACE;
}
bool shouldRenderFilledTriangles(void) {
    return (renderMode == RENDER_FILL_TRIANGLE || renderMode == RENDER_FILL_TRIANGLE_WIRE);
}
bool shouldRenderTexturedTriangles(void) {
    return (renderMode == RENDER_TEXTURED || renderMode == RENDER_TEXTURED_WIRE);
}

bool shouldRenderWireframeTriangles(void) {
    return (renderMode == RENDER_WIRE || renderMode == RENDER_WIRE_VERTEX || renderMode == RENDER_FILL_TRIANGLE_WIRE ||
            renderMode == RENDER_TEXTURED_WIRE);
}
bool shouldRenderVertex(void) {
    return renderMode == RENDER_WIRE_VERTEX;
}


bool initializeWindow(void) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0,&displayMode);
    int fullscreenWidth = displayMode.w;
    int fullscreenHeight = displayMode.h;

    windowWidth = fullscreenWidth / 2;
    windowHeight = fullscreenHeight / 2;


    //Create a window
    window= SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        fullscreenWidth,
        fullscreenHeight,
        SDL_WINDOW_BORDERLESS
        );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    //Create SDK renderer
    renderer = SDL_CreateRenderer(window,-1,0);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);

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

    return true;
}

void drawGrid(void) {
    for (int y = 0; y < windowHeight; y++) {
        for (int x = 0; x < windowWidth; x++) {

            if (x % 10 == 0 || y % 10 == 0) {
                colorBuffer[(windowWidth * y) + x] = 0xFFFFFFFF;
            }
        }
    }
}

void drawPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= windowWidth || y < 0 || y >= windowHeight) {
        return;
    }
        colorBuffer[(windowWidth*y) + x] = color;
}


void destroyWindow(void) {
    free(colorBuffer);
    free(zBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void clearColorBuffer(uint32_t color) {
    for (int i =0 ;i < windowHeight * windowWidth ;i++) {
            colorBuffer[i] = color;

    }

}

void clearZBuffer(void) {
    for (int i =0 ;i < windowHeight * windowWidth ;i++) {
        zBuffer[i] = 1.0;
    }
}




void renderColorBuffer(void) {
    SDL_UpdateTexture(
        colorBufferTexture,
        NULL,
        colorBuffer,
        (int)(windowWidth * sizeof(uint32_t))
        );
    SDL_RenderCopy(
        renderer,
        colorBufferTexture,
        NULL,
        NULL
    );
    SDL_RenderPresent(renderer);
}

void drawRect(int x , int y, int width , int height , uint32_t color) {
    for (int i = 0 ; i<width ; i ++) {
        for (int j= 0 ; j<height ; j++) {
            int currentX= x+ i;
            int currentY= y+ j;
            //colorBuffer[(windowWidth * currentY) + currentX] = color;
            drawPixel(currentX,currentY,color);
        }
    }

}


// Bresenham's Line Drawing Algorithm (DDA is slower so I choose to do it like this)
void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
    // Calculate absolute differences
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    // Determine direction of line
    int sx = (x0 < x1) ? 1 : -1;  // Step in x direction
    int sy = (y0 < y1) ? 1 : -1;  // Step in y direction

    // Calculate initial error
    int err = dx - dy;

    // Draw the line
    while (1) {
        // Draw pixel at current position
        drawPixel(x0, y0, color);

        // Check if we've reached the end point
        if (x0 == x1 && y0 == y1) break;

        // Calculate error for next step
        int e2 = 2 * err;

        // Step in x direction if needed
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        // Step in y direction if needed
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Draw the three edges of the triangle
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}
