#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"

//--------------------------------------------------------------------------------------
// g-buffer (read)
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_SAMPLER

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout (location = 0) out vec4 fragOut;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
const vec3 AMBIENT_LIGHT_COLOR = vec3(1,1,1);
const float AMBIENT_LIGHT_STRENGTH = 0.1;

void main()  {
    // read g-buffer
    vec3 diffuse        = texture(gbuffer0, uv).xyz;
    vec3 positionWorld  = texture(gbuffer1, uv).xyz;
    vec3 normalWorld    = texture(gbuffer2, uv).xyz;

    // shading
    vec3 finalColor = vec3(0);

    vec3 ambient = AMBIENT_LIGHT_STRENGTH * AMBIENT_LIGHT_COLOR;

    finalColor = ambient * diffuse;

    // output
    fragOut = vec4(finalColor, 1);
}
