#ifndef HYBRID_CORE_RAY_MARCH_TEST
#define HYBRID_CORE_RAY_MARCH_TEST

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core.glsl"
#include "math.glsl"
#include "ray.glsl"

//--------------------------------------------------------------------------------------
// custom sdf logic
//--------------------------------------------------------------------------------------
const int   RAY_MARCH_MAX_ITERATION     = 255;
const float RAY_MARCH_HIT_DISTANCE      = 0.001;

vec2 minx(vec2 v1, vec2 v2) {
	return (v1.x<v2.x) ? v1 : v2;
}

vec2 sdf_map(vec3 p) {
    float scale = 1.0;
    
    for(int i=0; i<8;i++) {
        p = -1.0 + 2.0*fract(0.5*p+0.5);
        float r2 = dot(p,p);
        float k = 1.25/r2;
        p     *= k;
        scale *= k;
    }

    return vec2(0.25*abs(p.y)/scale, 0);
}

vec3 sdf_n(in vec3 p) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*sdf_map(p+e.xyy).x + 
					  e.yyx*sdf_map(p+e.yyx).x + 
					  e.yxy*sdf_map(p+e.yxy).x + 
					  e.xxx*sdf_map(p+e.xxx).x );  
}

void sdf_material(in vec3 p, out hybrid_PBRMaterial mat){
    mat.albedo     = sdf_n(p)*0.5 + 0.5;
    if(p.y < 0.01) {
       mat.roughness  = 1;
       mat.metallic   = 0;  
    } else {
        mat.roughness  = 0;
        mat.metallic   = 1;  
    }
}

#endif