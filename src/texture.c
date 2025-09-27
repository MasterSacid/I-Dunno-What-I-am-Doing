#include "texture.h"

#include <stdio.h>


int textureWidth = 64;
int textureHeight = 64;

uint32_t* meshTexture = NULL;
upng_t* pngTexture = NULL;

void loadPngTextureData(char* fileName) {
    pngTexture = upng_new_from_file(fileName);
    if (pngTexture != NULL) {
        upng_decode(pngTexture);
        if (upng_get_error(pngTexture)== UPNG_EOK) {
            meshTexture = (uint32_t*) upng_get_buffer(pngTexture);
            textureWidth = upng_get_width(pngTexture);
            textureHeight= upng_get_height(pngTexture);
        }

    }
}

tex2_t tex2Clone(tex2_t* t) {
    tex2_t result = {t ->u, t->v};
    return result;
}
