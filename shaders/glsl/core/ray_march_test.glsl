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

const vec3 RAY_MARCH_ALBEDOS[6] = {
   vec3(0.9), vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(1,0,1), vec3(1,1,0)
};

vec2 minx(vec2 v1, vec2 v2) {
	return (v1.x<v2.x) ? v1 : v2;
}

vec2 map(in vec3 p) {
    vec2 res = vec2(MAX_FLOAT, 0); // x: distance -> y: material id | TODO: consider int material id?

    float d = MAX_FLOAT;

    // spheres
    float b = 3;
    float s0 = 0.5;
    vec3 q0 = p + vec3(0, -0.75, -4); 

    vec3 rou = round(q0/s0);
    q0.xy = q0.xy - s0*clamp(rou.xy,-b,b);

    rou += ceil(b/2) + 1;
    res = minx(res, vec2(sdSphere(q0, 0.2), rou.x * (b*2+1) + rou.y));

    vec3 q1 = p + vec3(-1.5, -0.5, -1);
    res = minx(res, vec2(sdSphere(q1, 0.25), -6));

    vec3 q2 = p + vec3(1.5, -0.5, -1);
    res = minx(res, vec2(sdSphere(q2, 0.25), -3));

    vec3 q3 = p + vec3(-1.5, -0.5, -2);
    res = minx(res, vec2(sdSphere(q3, 0.25), -4));

    vec3 q4 = p + vec3(1.5, -0.5, -2);
    res = minx(res, vec2(sdSphere(q4, 0.25), -5));

    vec3 q5 = p + vec3(0, 0, -1.5);
    res = minx(res, vec2(sdSphere(q5, 0.5), -1));

    // ground
    res = minx(res, vec2(sdZPlane(p, -0.5), -1));

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
            if(res.y > -1) {
                mat.albedo     = vec3(1,0,0);
                mat.roughness  = (mod(res.y, 7)) / 7;
                mat.metallic   = (res.y / 7) / 7;
            } else {
                mat.albedo     = RAY_MARCH_ALBEDOS[abs(int(res.y)+1)];
                mat.roughness  = 0.7;
                mat.metallic   = 0.7;
            }

            return depth;
        }

        depth += res.x;
    }

    return far;
}

#endif