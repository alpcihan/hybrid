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
const vec3 LIGHT_POS = vec3(1,1,1); 
const vec3 LIGHT_COLOR = vec3(1,1,1);

void main()  {
    // read g-buffer
    const vec3 diffuseColor   = texture(gbuffer0, uv).xyz;
    const vec3 positionWorld  = texture(gbuffer1, uv).xyz;
    const vec3 normalWorld    = texture(gbuffer2, uv).xyz;

    // shading
    vec3 finalColor = vec3(0);

    // diffuse
    vec3 norm = normalWorld;
    vec3 lightDir = normalize(LIGHT_POS - positionWorld);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LIGHT_COLOR;

    // ambient
    vec3 ambient = AMBIENT_LIGHT_STRENGTH * AMBIENT_LIGHT_COLOR;

    finalColor = (ambient + diffuse) * diffuseColor;

    // output
    fragOut = vec4(finalColor, 1);
}
