#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "../external/SDL/include/SDL.h"


#define FPS 120
#define FRAME_TARGET_TIME (1000/FPS)

enum cullMode {
    CULL_NONE,
    CULL_BACKFACE
};

enum renderMode {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
};

enum effect {
    WATER_EFFECT,
    NONE
};








int getWindowWidth(void);
int getWindowHeight(void);
void setRenderMode(int mode);
void setCullMode(int mode);


float getZBufferAt(int x, int y);
void updateZBufferAt(int x, int y, float value);


bool isCullBackface(void);
bool shouldRenderFilledTriangles(void);
bool shouldRenderTexturedTriangles(void);
bool shouldRenderWireframeTriangles(void);
bool shouldRenderVertex(void);



bool initializeWindow(void);
void drawGrid(void);
void drawRect(int x , int y, int width , int height , uint32_t color);
void drawPixel(int x, int y, uint32_t color);
void destroyWindow(void);
void clearColorBuffer(uint32_t color);
void clearZBuffer(void);
void renderColorBuffer(void);
void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif //DISPLAY_H
