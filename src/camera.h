#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"
#include "matrix.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t forwardVelocity;
    float yaw;
    float pitch;
}camera_t;

void initCamera(vec3_t pos, vec3_t dir);

vec3_t getCameraPosition(void);
vec3_t getCameraDirection(void);
vec3_t getCameraForwardVelocity(vec3_t velocity);

float getCameraYaw(void);
float getCameraPitch(void);

void rotateCameraYaw(float angle);
void rotateCameraPitch(float angle);

void updateCameraPosition(vec3_t position);
void updateCameraDirection(vec3_t direction);
void updateCameraForwardVelocity(vec3_t forwardVelocity);

vec3_t getCameraLookatTarget(void);



#endif //CAMERA_H