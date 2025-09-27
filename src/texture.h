#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "upng.h"
typedef struct {
    float u;
    float v;
}tex2_t;

extern int textureWidth;
extern int textureHeight;

extern upng_t* pngTexture;
extern uint32_t* meshTexture;

void loadPngTextureData(char* fileName);
tex2_t tex2Clone(tex2_t* t);

#endif //TEXTURE_H