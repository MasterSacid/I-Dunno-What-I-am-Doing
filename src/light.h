#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
}light_t;


uint32_t lightApplyIntensity(uint32_t originalColor, float percentage);
void lightNormalizedDirection(vec3_t lightDirection, light_t *lightPos);


#endif //LIGHT_H