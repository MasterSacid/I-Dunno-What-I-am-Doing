#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
typedef struct {
    float u;
    float v;
}tex2_t;

extern int textureWidth;
extern int textureHeight;

extern const uint8_t REDBRICK_TEXTURE[];

extern uint32_t* meshTexture;
#endif //TEXTURE_H