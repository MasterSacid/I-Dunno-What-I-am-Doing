#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


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

#endif //DISPLAY_H
