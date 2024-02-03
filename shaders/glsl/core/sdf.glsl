#ifndef HYBRID_CORE_SDF
#define HYBRID_CORE_SDF

#include "ray_march_test.glsl"

// https://iquilezles.org/articles/normalsSDF
vec3 sdf_normal(in vec3 p) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*sdf_map(p+e.xyy).x + 
					  e.yyx*sdf_map(p+e.yyx).x + 
					  e.yxy*sdf_map(p+e.yxy).x + 
					  e.xxx*sdf_map(p+e.xxx).x );  
}

// https://iquilezles.org/articles/rmshadows/
float sdf_softShadow( in vec3 ro, in vec3 rd, float mint, float maxt, float w )
{
    float res = 1.0;
    float t = mint;
    for( int i=0; i<RAY_MARCH_MAX_ITERATION && t<maxt; i++ )
    {
        float h = sdf_map(ro + t*rd).x;
        res = min( res, h/(w*t) );
        t += clamp(h, 0.005, 0.50);
        if( res<-1.0 || t>maxt ) break;
    }
    res = max(res,-1.0);
    return smoothstep( -1.0 , 1.0, res );
}

float sdf_rayCast(in vec3 origin, in vec3 dir, float near, float far, out vec3 position, out hybrid_PBRMaterial mat) { 
    float depth = near;
    for (int i = 0; i < RAY_MARCH_MAX_ITERATION && depth < far; i++) {
        vec3 p = origin + dir * depth;
        
        vec2 res = sdf_map(p);
        if (res.x < RAY_MARCH_HIT_DISTANCE) {
            position       = p; 
            sdf_material(p, mat);
            return depth;
        }

        depth += res.x;
    }

    return far;
}

#endif