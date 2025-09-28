#include "texture.h"

#include <stdio.h>




tex2_t tex2Clone(tex2_t* t) {
    tex2_t result = {t ->u, t->v};
    return result;
}
