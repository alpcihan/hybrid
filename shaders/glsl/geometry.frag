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
layout(set = 0, binding = 3) uniform sampler2D metalicTex;
layout(set = 0, binding = 4) uniform sampler2D roughnessTex;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_TARGET

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    // material
    const vec3 albedo      = texture(colorTex, frag.uv).rgb;
    const float metallic   = texture(metalicTex, frag.uv).r;
    const float roughness  = texture(roughnessTex, frag.uv).r; 

    // output
    gbuffer0        = vec4(albedo, roughness);
    gbuffer1        = vec4(frag.positionWorld,metallic);
    gbuffer2        = vec4(frag.normalWorld, HYBRID_OBJECT_FLAG);
}
