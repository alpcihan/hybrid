#ifndef HYBRID_CORE_RAY_MARCH_TEST
#define HYBRID_CORE_RAY_MARCH_TEST

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core.glsl"
#include "math.glsl"
#include "sdf.glsl"
#include "ray.glsl"

//--------------------------------------------------------------------------------------
// test ray marcher
//--------------------------------------------------------------------------------------
const int   RAY_MARCH_MAX_ITERATION     = 255;
const float RAY_MARCH_HIT_DISTANCE      = 0.001;

vec2 minx(vec2 v1, vec2 v2) {
	return (v1.x<v2.x) ? v1 : v2;
}

vec2 map(in vec3 p) {
    vec2 res = vec2(MAX_FLOAT, 0); // x: distance -> y: material id | TODO: consider int material id?

    float d = MAX_FLOAT;

    // spheres
    const float md = 1; 
    vec3 q0 = p; //q0 = mod(p, md) - md * 0.5;
    q0 += vec3(0,0,-1);
    res = minx(res, vec2(sdSphere(q0, 0.15), 0));

    // box
    vec3 q1 = p;
    q1 += vec3(1,0,0);
    q1.xz *= rot2D(_time);
    //q1.yz *= rot2D(0.2);
    res = minx(res, vec2(sdRoundBox(q1, vec3(0.5), 0.01), 1));
    //d = smin(d, sdRoundBox(q1, vec3(0.5), 0.01), 0.3);

    // ground
    //d = smin(d, sdZPlane(p, -0.4), 0.1);
    res = minx(res, vec2(sdZPlane(p, -0.4), 2));

    return res;
}

// https://iquilezles.org/articles/normalsSDF
vec3 sdfNormal(in vec3 p) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map(p+e.xyy).x + 
					  e.yyx*map(p+e.yyx).x + 
					  e.yxy*map(p+e.yxy).x + 
					  e.xxx*map(p+e.xxx).x );  
}

float rayCast(in vec3 origin, in vec3 dir, float near, float far, out vec3 position, out hybrid_PBRMaterial mat) { 
    float depth = near;
    for (int i = 0; i < RAY_MARCH_MAX_ITERATION && depth < far; i++) {
        vec3 p = origin + dir * depth;
        
        vec2 res = map(p);
        if (res.x < RAY_MARCH_HIT_DISTANCE) {
            position       = p;
            mat.albedo     = vec3(1,0,0);
            mat.roughness  = 0.7;
            mat.metallic   = 0.2;

            return depth;
        }

        depth += res.x;
    }

    return far;
}

#endif