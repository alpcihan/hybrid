#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/ray_march_test.glsl"

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
void main()  {
    Ray ray = createCameraRay(uv*2-1, inverse(_projection), inverse(_view));
    
    // ray marching
    vec3 positionWorld;
    float depth = rayCast(ray.origin, ray.direction, _projectionParams[0], _projectionParams[1], positionWorld);

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
    gbuffer2.xyz    = sdfNormal(positionWorld);
}
