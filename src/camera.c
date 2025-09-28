#include "camera.h"
static camera_t camera;

void initCamera(vec3_t pos, vec3_t dir) {
    camera.position = pos;
    camera.direction = dir;
    camera.forwardVelocity = vec3New(0,0,0);
    camera.yaw = 0.0;
    camera.pitch = 0.0;

}

vec3_t getCameraPosition(void) {
    return camera.position;
}
vec3_t getCameraDirection(void) {
    return camera.direction;
}
vec3_t getForwardVelocity(void) {
    return camera.forwardVelocity;
}
float getCameraYaw(void) {
    return camera.yaw;
}
float getCameraPitch(void) {
    return camera.pitch;
}

void rotateCameraYaw(float angle) {
    camera.yaw += angle;
}

void rotateCameraPitch(float angle) {
    camera.pitch += angle;
}
void updateCameraPosition(vec3_t position) {
    camera.position = position;
}
void updateCameraDirection(vec3_t direction) {
    camera.direction = direction;
}
void updateCameraForwardVelocity(vec3_t forwardVelocity) {
    camera.forwardVelocity = forwardVelocity;
}

vec3_t getCameraLookatTarget(void) {
    // Initialize the target looking at the +Z axis
    vec3_t target = (vec3_t){0, 0, 1};

    // Get yaw & pitch from camera
    float yaw   = getCameraYaw();
    float pitch = getCameraPitch();

    // Build rotation matrices
    mat4_t yawRotation   = mat4MakeRotationY(yaw);
    mat4_t pitchRotation = mat4MakeRotationX(pitch);

    // Combine into one rotation matrix (pitch first, then yaw)
    mat4_t rotation = mat4Identity();
    rotation = mat4MultipMat4(pitchRotation, rotation);
    rotation = mat4MultipMat4(yawRotation, rotation);

    // Rotate the target direction
    vec4_t rotatedTarget = mat4MultipVec4(rotation, vec3ToVec4(target));
    vec3_t direction     = vec4ToVec3(rotatedTarget);

    // Update camera direction
    updateCameraDirection(direction);

    // Offset the camera position in the direction it's pointing
    vec3_t pos = getCameraPosition();
    target = vec3Add(pos, direction);

    return target;
}
