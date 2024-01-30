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
    vec2 uv;
} frag;

layout(set = 0, binding = 2) uniform sampler2D colorTex;
//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_TARGET

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    // material
    const float roughness   = 0.9; //TODO: Remove these
    const float metallic    = 0.2; //TODO: Remove these

    // output
    gbuffer0        = vec4(texture(colorTex, frag.uv).rgb, roughness);
    gbuffer1        = vec4(frag.positionWorld,metallic);
    gbuffer2        = vec4(frag.normalWorld, HYBRID_OBJECT_FLAG);
}
