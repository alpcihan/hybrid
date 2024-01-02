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
    float specularStrenght;
    float shininess;
};

struct AmbientLight {
    vec3 color;
    float strength;
};

struct DirectionalLight {
    vec3 color;
    float strength;
    vec3 direction;
};

const Material material = {0.2, 64};
const AmbientLight ambientLight = {vec3(1), 0.01};
const DirectionalLight directionalLight = {vec3(1), 0.6, vec3(0.2f, 1.0f, 0.3f)};

void main()  {
    // read g-buffer
    const vec3 diffuseColor   = texture(gbuffer0, uv).xyz;
    const vec3 positionWorld  = texture(gbuffer1, uv).xyz;
    const vec3 normalWorld    = texture(gbuffer2, uv).xyz;

    //--------------------------------------------------------------------------------------
    // shading
    vec3 finalColor = vec3(0);

    // diffuse
    vec3 norm = normalWorld;
    vec3 lightDir = directionalLight.direction;  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * directionalLight.color * directionalLight.strength;

    // specular
    vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;
    vec3 viewDir = normalize(viewPos - positionWorld);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specularStrenght * spec * directionalLight.color * directionalLight.strength;  

    // ambient
    vec3 ambient = ambientLight.strength * ambientLight.color;

    // result
    finalColor = (ambient + diffuse + specular) * diffuseColor;
    //--------------------------------------------------------------------------------------

    // output
    fragOut = vec4(finalColor, 1);
}
