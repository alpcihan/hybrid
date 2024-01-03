#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/math.glsl"

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
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

struct PointLight {
    vec3 color;
    vec3 position;

    float attenuationCoeff; // TODO: add constant and linear term
};

//--------------------------------------------------------------------------------------
// lights
#define LIGHT_COUNT 3
const PointLight lights[LIGHT_COUNT] = {
    {vec3(1), vec3(-0.5,1,2), 5},
    {vec3(1), vec3(1,0,2),    5},
    {vec3(1), vec3(-1,0.1,2), 5},
};

const vec3 ambient = vec3(0);
//--------------------------------------------------------------------------------------

void main() {
    // read g-buffer
    const vec3 albedo         = texture(gbuffer0, uv).xyz;
    const vec3 positionWorld  = texture(gbuffer1, uv).xyz;
    const vec3 normalWorld    = texture(gbuffer2, uv).xyz;

    // material
    const float metallic = 0.7;
    const float roughness = 0.2;
    const float ao = 0.5;

    //--------------------------------------------------------------------------------------
    // pbr shading | https://learnopengl.com/PBR/Lighting
    const vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;
    vec3 N = normalize(normalWorld);
    vec3 V = normalize(viewPos - positionWorld);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < LIGHT_COUNT; ++i) {
        // radiance
        const vec3 toLight = lights[i].position - positionWorld;
        const vec3 L = normalize(toLight);
        const vec3 H = normalize(V + L);
        float distance    = length(toLight);
        float attenuation = 1.0 / (pow(distance, lights[i].attenuationCoeff));
        vec3 radiance     = lights[i].color * attenuation;        
            
        // brdf
        float NDF = distributionGGX(N, H, roughness);        
        float G   = geometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
            
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
            
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
                
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  
    }
    
    vec3 ambient = vec3(0.0) * albedo * ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
    //--------------------------------------------------------------------------------------

    // output
    fragOut = vec4(color, 1);  
}
