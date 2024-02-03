#ifndef HYBRID_CORE_RAY_MARCH_TEST
#define HYBRID_CORE_RAY_MARCH_TEST

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core.glsl"
#include "math.glsl"
#include "ray.glsl"

//--------------------------------------------------------------------------------------
// signed distance functions
//--------------------------------------------------------------------------------------
// https://iquilezles.org/articles/distfunctions/
float sdSphere(in vec3 p, float r) {
    return length(p) - r;
}

// https://iquilezles.org/articles/distfunctions/
float sdBox(in vec3 p, in vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// https://iquilezles.org/articles/distfunctions/
float sdRoundBox(in vec3 p, in vec3 b, float r) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float sdZPlane(in vec3 p, in float y) {
    return p.y - y;
}


//--------------------------------------------------------------------------------------
// test ray marcher
//--------------------------------------------------------------------------------------
const int   RAY_MARCH_MAX_ITERATION     = 255;
const float RAY_MARCH_HIT_DISTANCE      = 0.001;

vec2 minx(vec2 v1, vec2 v2) {
	return (v1.x<v2.x) ? v1 : v2;
}

// https://iquilezles.org/articles/menger/
float sdMengerSponge( in vec3 p ) {
    float d = sdBox(p,vec3(1.0));
    
    float s = 1.0;
    for( int m=0; m<4; m++ ){
        vec3 a = mod( (p - vec3(m+1, m+1, m+1)) *s, 2.0 )-1.0;
        s *= 3.0;
        vec3 r = abs(2.0 - 3.0*abs(a));

        float da = max(r.x,r.y);
        float db = max(r.y,r.z);
        float dc = max(r.z,r.x);
        float c = (min(da,min(db,dc))-1.0)/s;

        d = max(d,c);
    }

    return d;
}

//vec2 sdf_map(in vec3 p) {
//    vec2 res = vec2(MAX_FLOAT, 0); // x: distance -> y: material id | TODO: consider int material id?
//
//    res = minx(res, vec2(sdMengerSponge((p) / 4) * 4, 0));
//    
//    return res;
//}

 
vec2 sdf_map( vec3 p)
{
    float scale = 1.0;
    
    for( int i=0; i<8;i++ )
    {
        p = -1.0 + 2.0*fract(0.5*p+0.5);

        float r2 = dot(p,p);
        
        float k = 1.25/r2;
        p     *= k;
        scale *= k;
    }
    
    return vec2(0.25*abs(p.y)/scale,0);
}

// https://iquilezles.org/articles/normalsSDF
vec3 sdf_n(in vec3 p) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*sdf_map(p+e.xyy).x + 
					  e.yyx*sdf_map(p+e.yyx).x + 
					  e.yxy*sdf_map(p+e.yxy).x + 
					  e.xxx*sdf_map(p+e.xxx).x );  
}

void sdf_material(in vec3 p, out hybrid_PBRMaterial mat){
    mat.albedo     = sdf_n(p)*0.5 + 0.5;
    mat.roughness  = 0.1;
    mat.metallic   = 1.0;
}

#endif