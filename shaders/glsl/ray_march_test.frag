#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/ray.glsl"

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
const int RAY_MARCH_MAX_ITERATION = 1000;
const float RAY_MARCH_HIT_DISTANCE = 0.01;

float sdSphere(in vec3 p, in vec3 c, float r) {
	return length(p - c) - r;
}

float sceneHit(in vec3 p, out vec3 normal) {
    const vec3 sphereCenter = vec3(0,0,0);
    const float sphereRadius = 0.2;

    vec3 q = p;
    q = fract(p) - .5;

    const float d = sdSphere(q, sphereCenter, sphereRadius);
    normal = normalize(q - sphereCenter);

    return d; 
}

float rayMarch(in vec3 origin, in vec3 dir, float near, float far, out vec3 position, out vec3 normal) { 
    normal = vec3(0,0,0);
    
    float depth = near;
    for (int i = 0; i < RAY_MARCH_MAX_ITERATION; i++) {
        vec3 p = origin + dir * depth;
        
        float dist = sceneHit(p, normal);
        if (dist < RAY_MARCH_HIT_DISTANCE) {
            position = p;

            return depth;
        }

        depth += dist;
        if ( depth >= far) {
            return far;
        }

    }

    return far;
}

void main()  {
    Ray ray = createCameraRay(uv*2-1, inverse(_projection), inverse(_view));
    
    // ray marching
    vec3 positionWorld;
    vec3 normalWorld;
    float depth = rayMarch(ray.origin, ray.direction, _projectionParams[0], _projectionParams[1], positionWorld, normalWorld);

    // hit check
    if(depth >= _projectionParams[1]) {
      gl_FragDepth = 1;
      return;
    } 

    // output
    gl_FragDepth = linearToZDepth(depthToEyeZ(depth, ray.direction));
    gbuffer0.xyz = vec3(0.7,0.2,0.2);
    gbuffer1.xyz = positionWorld;
    gbuffer2.xyz = normalWorld;
}
