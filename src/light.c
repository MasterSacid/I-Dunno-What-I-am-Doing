

#include "light.h"


uint32_t lightApplyIntensity(uint32_t originalColor, float percentage) {
    uint32_t a = (originalColor & 0xFF000000);
    uint32_t r = ((originalColor & 0x00FF0000) >> 16) * percentage;  // Need to shift!
    uint32_t g = ((originalColor & 0x0000FF00) >> 8) * percentage;   // Need to shift!
    uint32_t b = (originalColor & 0x000000FF) * percentage;

    // Reconstruct the color properly
    uint32_t newColor = a |
                        ((uint32_t)(r) << 16) |
                        ((uint32_t)(g) << 8) |
                        (uint32_t)(b);
    return newColor;
}

void lightNormalizedDirection(vec3_t lightDirection, light_t *lightPos) {
    vec3_t vector3LightDestinationToLightPos = vec3Subtract(lightDirection,lightPos->position);
    vec3Normalize(&vector3LightDestinationToLightPos);
    lightPos->direction = vector3LightDestinationToLightPos;

}