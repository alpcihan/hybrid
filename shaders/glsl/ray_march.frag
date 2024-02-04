#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
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
void main()  {
    
    Ray ray = createCameraRay(uv*2-1 + 0.5*calculateUvSize(), inverse(_projection), inverse(_view));
    
    // ray marching
    vec3 positionWorld;
    hybrid_PBRMaterial mat;
    float depth = sdf_rayCast(
      ray.origin,
      ray.direction,
      _projectionParams[0],
      _projectionParams[1],
      positionWorld,
      mat
    );

    // hit check
    if(depth >= _projectionParams[1]) {
      gl_FragDepth = 1;
      return;
    } 

    // output
    gl_FragDepth    = hybrid_linearToZDepth(hybrid_depthToEyeZ(depth, ray.direction));
    gbuffer0        = vec4(mat.albedo, mat.roughness);
    gbuffer1        = vec4(positionWorld, mat.metallic);
    gbuffer2        = vec4(sdf_normal(positionWorld), HYBRID_OBJECT_FLAG);
}
