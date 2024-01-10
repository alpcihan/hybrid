#version 450
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout(location = 0) in FragData {
    vec3 positionWorld;
    vec3 normalWorld;
} frag;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_TARGET

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    // material
    const vec3  albedo      = vec3(0.8);
    const float roughness   = 0.2;
    const float metallic    = 0.8;

    // output
    gbuffer0        = vec4(albedo,roughness);
    gbuffer1        = vec4(frag.positionWorld,metallic);
    gbuffer2        = vec4(frag.normalWorld, HYBRID_OBJECT_FLAG);
}
