#include "matrix.h"

mat4_t mat4Identity(void) {
    mat4_t m = {{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    }};
    return m;
}
mat4_t mat4MakeScale(float sx, float sy, float sz) {
    /*
 * Scale Matrix:
 * [sx  0   0   0]
 * [0   sy  0   0]
 * [0   0   sz  0]
 * [0   0   0   1]
 *
 * sx, sy, sz = scaling factors for x, y, z axes
 */

    mat4_t m = mat4Identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

mat4_t mat4MakeTranslation(float tx, float ty, float tz) {
    /*
 * Translation Matrix:
 * [1   0   0   tx]
 * [0   1   0   ty]
 * [0   0   1   tz]
 * [0   0   0   1 ]
 *
 * tx, ty, tz = translation distances for x, y, z axes
 */
    mat4_t m = mat4Identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

mat4_t mat4MakeRotationX(float angle) {
    /*
     * Rotation Matrix around X-axis:
     * [1    0    0    0]
     * [0    c   -s    0]
     * [0    s    c    0]
     * [0    0    0    1]
     *
     * c = cos(angle), s = sin(angle)
     */
    mat4_t m = mat4Identity();
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;

    return m;
}

mat4_t mat4MakeRotationY(float angle) {
    /*
     * Rotation Matrix around Y-axis:
     * [ c    0    s    0]
     * [ 0    1    0    0]
     * [-s    0    c    0]
     * [ 0    0    0    1]
     *
     * c = cos(angle), s = sin(angle)
     */
    mat4_t m = mat4Identity();
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;

    return m;
}

mat4_t mat4MakeRotationZ(float angle) {
    /*
     * Rotation Matrix around Z-axis:
     * [ c   -s    0    0]
     * [ s    c    0    0]
     * [ 0    0    1    0]
     * [ 0    0    0    1]
     *
     * c = cos(angle), s = sin(angle)
     */
    mat4_t m = mat4Identity();
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;

    return m;
}


mat4_t mat4MakePerspective(float fov, float aspect, float zNear, float zFar) {

    // Perspective projection matrix (row-major)
    //
    // | (h/w)*1/tan(fov/2)      0                  0                     0 |
    // | 0                        1/tan(fov/2)      0                     0 |
    // | 0                        0                  zf/(zf-zn)      (-zf*zn)/(zf-zn) |
    // | 0                        0                  1                     0 |

    mat4_t m = { { { 0 } } };

    m.m[0][0] = aspect * (1 / tan(fov / 2));
    m.m[1][1] = 1 / tan(fov / 2);
    m.m[2][2] = zFar / (zFar - zNear);
    m.m[2][3] = (-zFar * zNear) / (zFar - zNear);
    m.m[3][2] = 1.0f;

    return m;
}


vec4_t mat4MultipVec4Project(mat4_t matProj, vec4_t v) {
    // multiply the projection matrix by our original vector
    vec4_t result = mat4MultipVec4(matProj, v);

    // perform perspective divide with original z-value that is now stored in w
    if (result.w != 0.0f) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}




mat4_t mat4MultipMat4(mat4_t a, mat4_t b) {
    mat4_t m;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0] * b.m[0][j] +
                        a.m[i][1] * b.m[1][j] +
                        a.m[i][2] * b.m[2][j] +
                        a.m[i][3] * b.m[3][j];
        }
    }
    return m;
}






//Vector return
vec4_t mat4MultipVec4(mat4_t m , vec4_t v){
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4_t mat4LookAt(vec3_t eye, vec3_t target, vec3_t up) {
    // Compute the forward (z), right (x), and up (y) vectors
    vec3_t z = vec3Subtract(target, eye);
    vec3Normalize(&z);
    vec3_t x = vec3Cross(up, z);
    vec3Normalize(&x);
    vec3_t y = vec3Cross(z, x);

    mat4_t viewMatrix = {{
        { x.x, x.y, x.z, -vec3DotProduct(x, eye) },
        { y.x, y.y, y.z, -vec3DotProduct(y, eye) },
        { z.x, z.y, z.z, -vec3DotProduct(z, eye) },
        { 0,   0,   0,    1 }
    }};

    return viewMatrix;
}









