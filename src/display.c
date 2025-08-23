#include "display.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture* colorBufferTexture = NULL;
uint32_t* colorBuffer = NULL;

int windowWidth = 800;
int windowHeight = 600;

bool initializeWindow(void) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0,&displayMode);
    windowWidth=displayMode.w;
    windowHeight=displayMode.h;


    //Create a window
    window= SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
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
    if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
        colorBuffer[(windowWidth*y) + x] = color;
}


void destroyWindow(void) {
    free(colorBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void clearColorBuffer(uint32_t color) {
    for (int y =0 ;y < windowHeight ;y++) {
        for (int x =0 ;x < windowWidth ;x++) {
            colorBuffer[(windowWidth*y) + x] = color;
        }
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