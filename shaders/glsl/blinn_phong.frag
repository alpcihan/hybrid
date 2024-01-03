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
struct Material {
    float specularStrength;
    float shininess;
};

struct AmbientLight {
    vec3 emissiveColor;
};

struct PointLight {
    vec3 emissiveColor;
    vec3 position;

    float c;
    float l;
    float q;
};

const Material material = {1, 128};
const PointLight pointLight = {vec3(1,1,1), vec3(0,0,1), 0.5, 0.09, 6.4};
const AmbientLight ambientLight = {vec3(0,0,0)};

void main() {
    // read g-buffer
    const vec3 albedo           = texture(gbuffer0, uv).xyz;
    const vec3 positionWorld    = texture(gbuffer1, uv).xyz;
    const vec3 normalWorld      = texture(gbuffer2, uv).xyz;

    //--------------------------------------------------------------------------------------
    // shading
    vec3 finalColor = vec3(0);
    const vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;
    const vec3 viewDir = normalize(viewPos - positionWorld);

    // diffuse (point light)
    vec3 pl_pos = pointLight.position;
    const vec3 pl_lightToSurface = positionWorld - pl_pos;
    const vec3 pl_lightDir = normalize(pl_lightToSurface);  
    const float pl_distance = length(pl_lightToSurface);
    const float pl_attenuation = 1.0 / (pointLight.c + pointLight.l * pl_distance + pointLight.q * (pl_distance * pl_distance));
    const vec3 pl_halfwayDir = normalize(viewDir - pl_lightDir);

    const float pl_diff = max(dot(normalWorld, -pl_lightDir), 0.0);
    vec3 pl_diffuse = pl_diff * pointLight.emissiveColor;
    pl_diffuse *= pl_attenuation;

    // specular (point light)
    const vec3 pl_reflectDir = reflect(pl_lightDir, normalWorld);
    const float pl_spec = pow(max(dot(normalWorld, pl_halfwayDir), 0.0), material.shininess);
    vec3 pl_specular = material.specularStrength * pl_spec * pointLight.emissiveColor;
    pl_specular *= pl_attenuation;

    // ambient light
    vec3 ambient = ambientLight.emissiveColor;

    // result
    finalColor = (ambient + pl_diffuse + pl_specular) * albedo;
    //--------------------------------------------------------------------------------------

    // output
    fragOut = vec4(finalColor, 1);
}
