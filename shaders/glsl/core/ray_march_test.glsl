#ifndef HYBRID_CORE_RAY_MARCH_TEST
#define HYBRID_CORE_RAY_MARCH_TEST

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "math.glsl"
#include "sdf.glsl"
#include "ray.glsl"

//--------------------------------------------------------------------------------------
// test ray marcher
//--------------------------------------------------------------------------------------
const int RAY_MARCH_MAX_ITERATION = 255;
const float RAY_MARCH_HIT_DISTANCE = 0.001;

float map(in vec3 p) {
    float d = MAX_FLOAT;

    // spheres
    const float md = 1; 
    vec3 q0 = p; q0 = mod(p, md) - md * 0.5;
    d = min(d, sdSphere(q0, 0.15));

    // box
    vec3 q1 = p;
    q1 += vec3(sin(_time),0,0);
    q1.xz *= rot2D(_time);
    q1.yz *= rot2D(_time);
    d = smin(d, sdRoundBox(q1, vec3(0.5), 0.01), 0.3);

    // ground
    d = smin(d, sdZPlane(p, -0.4), 0.1);

    return d;
}

// https://iquilezles.org/articles/normalsSDF
vec3 sdfNormal(in vec3 p) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map(p+e.xyy) + 
					  e.yyx*map(p+e.yyx) + 
					  e.yxy*map(p+e.yxy) + 
					  e.xxx*map(p+e.xxx) );  
}

float rayCast(in vec3 origin, in vec3 dir, float near, float far, out vec3 position) { 
    float depth = near;
    for (int i = 0; i < RAY_MARCH_MAX_ITERATION && depth < far; i++) {
        vec3 p = origin + dir * depth;
        
        float d = map(p);
        if (d < RAY_MARCH_HIT_DISTANCE) {
            position = p;
            return depth;
        }

        depth += d;
    }

    return far;
}

#endif