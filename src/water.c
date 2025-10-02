#include "water.h"
#include <math.h>
#include <stddef.h>


static inline float deg2rad(float d) { return d * (3.14159265358979323846f / 180.0f); }

static inline float fast_mod_2pi(float x) {
    const float TWO_PI = 6.28318530717958647692f;
    int n = (int)(x / TWO_PI);
    return x - n * TWO_PI;
}

static inline void make_wave(WaterWave* w,
                             float angle_deg, float lambda_m,
                             float omega,
                             float ampU, float ampV,
                             float phaseU, float phaseV)
{
    const float TWO_PI = 6.28318530717958647692f;
    float theta = deg2rad(angle_deg);
    float k     = TWO_PI / lambda_m;
    w->kx       = k * cosf(theta);
    w->kz       = k * sinf(theta);
    w->omega    = (omega > 0.0f) ? omega : sqrtf(9.81f * k);
    w->ampU     = ampU;
    w->ampV     = ampV;
    w->phaseU   = phaseU;
    w->phaseV   = phaseV;
}


static inline float lut_sin(const WaterParams* wp, float phase) {
    if (wp->lut_size <= 0 || !wp->sin_lut) return sinf(phase);
    const float TWO_PI = 6.28318530717958647692f;
    float p = phase / TWO_PI;  // cycles
    p -= floorf(p);            // fractional part in [0,1)
    int idx = (int)(p * wp->lut_size);
    if (idx >= wp->lut_size) idx = 0;
    return wp->sin_lut[idx];
}
static inline float lut_cos(const WaterParams* wp, float phase) {
    const float HALF_PI = 1.57079632679489661923f;
    return lut_sin(wp, phase + HALF_PI);
}


void water_init_default(WaterParams* wp) {
    if (!wp) return;
    *wp = (WaterParams){0};

    // Use 4 non-colinear directions so the effect stays visible at any camera yaw.
    wp->wave_count = 4;
    // Wavelengths ~5–10 m give nice on-screen variation with typical world scales.
    make_wave(&wp->waves[0],  15.0f,  9.0f,  0.0f, 0.020f, 0.018f, 0.0f,  0.0f);
    make_wave(&wp->waves[1],  75.0f,  6.5f,  0.0f, 0.014f, 0.016f, 1.1f,  2.0f);
    make_wave(&wp->waves[2], 135.0f,  5.0f,  0.0f, 0.012f, 0.013f, 0.6f,  1.7f);
    make_wave(&wp->waves[3], 195.0f, 10.0f,  0.0f, 0.010f, 0.010f, 2.2f,  0.8f);


    wp->shimmer_u_scale = 0.004f; // e.g., 0.004f
    wp->shimmer_v_scale = 0.004f; // e.g., 0.004f


    wp->lut_size = 0;
    wp->sin_lut  = NULL;
}

void water_build_sin_lut(WaterParams* wp, int size, float* backing_buffer) {
    if (!wp || size <= 0 || !backing_buffer) return;
    wp->lut_size = size;
    wp->sin_lut  = backing_buffer;

    const float TWO_PI = 6.28318530717958647692f;
    for (int i = 0; i < size; ++i) {
        float phase = (TWO_PI * i) / (float)size;
        backing_buffer[i] = sinf(phase);
    }
}

void water_compute_uv_warp(const WaterParams* wp, float Xw, float Zw, float t,
                           float* out_du, float* out_dv)
{
    float du = 0.0f, dv = 0.0f;

    // Sum directional waves
    for (int i = 0; i < wp->wave_count; ++i) {
        const WaterWave* w = &wp->waves[i];
        // phase φ = k · p − ω t, with k=(kx,kz), p=(Xw,Zw)
        float phase = w->kx * Xw + w->kz * Zw - w->omega * t;
        phase = fast_mod_2pi(phase);
        du += w->ampU * lut_sin(wp, phase + w->phaseU);
        dv += w->ampV * lut_cos(wp, phase + w->phaseV);
    }

    // Optional gradient-driven shimmer (approx refraction via ∇h)
    if (wp->shimmer_u_scale != 0.0f || wp->shimmer_v_scale != 0.0f) {
        float dHx = 0.0f, dHz = 0.0f;
        for (int i = 0; i < wp->wave_count; ++i) {
            const WaterWave* w = &wp->waves[i];
            float phase = w->kx * Xw + w->kz * Zw - w->omega * t;
            phase = fast_mod_2pi(phase);
            float c = lut_cos(wp, phase);
            float a = 0.5f * (w->ampU + w->ampV);
            dHx += a * w->kx * c;
            dHz += a * w->kz * c;
        }
        du += wp->shimmer_u_scale * dHx;
        dv += wp->shimmer_v_scale * dHz;
    }

    if (out_du) *out_du = du;
    if (out_dv) *out_dv = dv;
}
