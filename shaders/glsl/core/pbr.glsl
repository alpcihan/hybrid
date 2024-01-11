#ifndef HYBRID_CORE_PBR
#define HYBRID_CORE_PBR

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core.glsl"
#include "math.glsl"

//--------------------------------------------------------------------------------------
// pbr utils
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

//--------------------------------------------------------------------------------------
// pbr lighting functions | https://learnopengl.com/PBR/Lighting
//--------------------------------------------------------------------------------------
vec3 calculatePBRFromActiveSceneLights(
    in vec3 albedo,
    float roughness,
    float metallic,
    in vec3 positionWorld,
    in vec3 normalWorld,
    in vec3 viewPos,
    in int i
) {
    vec3 N = normalize(normalWorld);
    vec3 V = normalize(viewPos - positionWorld);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    vec3 Lo = vec3(0.0);


    // radiance
    const vec3 toLight = _lights[i].position - positionWorld;
    const vec3 L = normalize(toLight);
    const vec3 H = normalize(V + L);
    float distance    = length(toLight);
    float attenuation = 1.0 / (pow(distance, _lights[i].attenuationCoeff));
    vec3 radiance     = _lights[i].color * attenuation;        
        
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

    return Lo;
}

#endif