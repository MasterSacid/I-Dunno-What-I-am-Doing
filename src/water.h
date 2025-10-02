#ifndef WATER_H
#define WATER_H

#include <stddef.h>

#ifndef WATER_MAX_WAVES
#define WATER_MAX_WAVES 8
#endif

    typedef struct {
        float kx;      // wave vector x component (rad/m)
        float kz;      // wave vector z component (rad/m)
        float omega;   // angular frequency (rad/s)
        float ampU;    // UV warp amplitude in U
        float ampV;    // UV warp amplitude in V
        float phaseU;  // extra phase (radians) for U
        float phaseV;  // extra phase (radians) for V
    } WaterWave;

    typedef struct {

        int wave_count;
        WaterWave waves[WATER_MAX_WAVES];


        float shimmer_u_scale; // try 0.002–0.008
        float shimmer_v_scale; // try 0.002–0.008


        int    lut_size;
        float* sin_lut;
    } WaterParams;


    void water_init_default(WaterParams* wp);


    void water_build_sin_lut(WaterParams* wp, int size, float* backing_buffer);

    /**
     * Compute a UV warp for world-space point (Xw, Zw) at time t.
     * Returns offsets (du, dv) you add to your base UV. Caller may wrap to [0,1).
     */
    void water_compute_uv_warp(const WaterParams* wp, float Xw, float Zw, float t,
                               float* out_du, float* out_dv);

    /** Wrap a float into [0,1). */
    static inline float water_wrap01(float u) {
        int i = (int)u;
        float f = u - (float)i;
        if (f < 0.0f) f += 1.0f;
        return f;
    }
#endif // WATER_H
