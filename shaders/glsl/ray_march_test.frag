#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/math.glsl"
#include "core/ray.glsl"
#include "core/sdf.glsl"

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_TARGET

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
const int RAY_MARCH_MAX_ITERATION = 255;
const float RAY_MARCH_HIT_DISTANCE = 0.001;

float map(in vec3 p) {
    float d = MAX_FLOAT;

    // spheres
    const float md = 0.75; 
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

float rayCast(in vec3 origin, in vec3 dir, float near, float far, out vec3 position, out vec3 normal) { 
    float depth = near;
    for (int i = 0; i < RAY_MARCH_MAX_ITERATION && depth < far; i++) {
        vec3 p = origin + dir * depth;
        
        float d = map(p);
        if (d < RAY_MARCH_HIT_DISTANCE) {
            position = p;
            normal = sdfNormal(p);
            return depth;
        }

        depth += d;
    }

    return far;
}

void main()  {
    Ray ray = createCameraRay(uv*2-1, inverse(_projection), inverse(_view));
    
    // ray marching
    vec3 positionWorld;
    vec3 normalWorld;
    float depth = rayCast(ray.origin, ray.direction, _projectionParams[0], _projectionParams[1], positionWorld, normalWorld);

    // hit check
    if(depth >= _projectionParams[1]) {
      gl_FragDepth = 1;
      return;
    } 

    // material
    const vec3  albedo      = vec3(0.8);
    const float roughness   = 0.2;
    const float metallic    = 0.8;

    // output
    gl_FragDepth    = linearToZDepth(depthToEyeZ(depth, ray.direction));
    gbuffer0        = vec4(albedo, roughness);
    gbuffer1        = vec4(positionWorld, metallic);
    gbuffer2.xyz    = normalWorld;
}
