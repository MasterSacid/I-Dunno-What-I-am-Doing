#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

enum cullMode {
    CULL_NONE,
    CULL_BACKFACE
}cullMode;

enum renderMode {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
} renderMode;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern SDL_Texture* colorBufferTexture;
extern uint32_t* colorBuffer;

extern int windowWidth;
extern int windowHeight;


bool initializeWindow(void);
void drawGrid(void);
void drawRect(int x , int y, int width , int height , uint32_t color);
void drawPixel(int x, int y, uint32_t color);
void destroyWindow(void);
void clearColorBuffer(uint32_t color);
void renderColorBuffer(void);
void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif //DISPLAY_H
