#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/pbr.glsl"

//--------------------------------------------------------------------------------------
// g-buffer (read)
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_SAMPLER

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;

layout(set = 1, binding = 3) readonly buffer shadowStorage{
    float shadowMap[];
};

layout(set = 2, binding = 0) readonly uniform SpecularReflectionPyramidSize {
    int _specularReflectionPyramidSize;
};

layout(set = 2, binding = 1) uniform sampler2D _specularReflectionPyramid[];

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout (location = 0) out vec4 fragOut;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    // read g-buffer
    const vec4 gb0  = texture(gbuffer0, uv);
    const vec4 gb1  = texture(gbuffer1, uv);
    const vec4 gb2  = texture(gbuffer2, uv);

    ivec2 texCoords = ivec2(_projectionParams.zw * uv);
    ivec2 bounds = ivec2( _projectionParams.z, _projectionParams.w);
    
    const vec3  albedo          = gb0.xyz;
    const float roughness       = gb0.w;
    const vec3  positionWorld   = gb1.xyz;
    const float metallic        = gb1.w;
    const vec3  normalWorld     = gb2.xyz;

    const float ao = 0.01;
    const vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;
    const vec3  V = normalize(viewPos - positionWorld);  

    //TODO: Check max light count before loop
    uint shadowIdx = texCoords.y * bounds.x + texCoords.x;
    float shadowVal = shadowMap[shadowIdx];

    vec3 color = vec3(0.0);
    const vec3 ambient = vec3(0.1);
	for(int i = 0; i < HYBRID_LIGHT_COUNT; ++i){
        color += ambient * albedo * ao;
        vec3 Lo = calculatePBRLoFromSceneLights(
                albedo,
                roughness,
                metallic,
                positionWorld,
                normalWorld,
                viewPos,
                i);

        color += Lo * shadowVal;
    }

    // sample reflection map
    const vec4 inReflect = texture(_specularReflectionPyramid[0], uv);

    // indirect light
    const float indirectIntensity = 1;
    vec3 f0 = 0.04 * (1.0 - metallic) + color * metallic;

    // indirect diffuse
    vec3 diffuseColor = (1.0 - metallic) * color;
    vec3 indirectDiffuse = irradiance_SphericalHarmonics(normalWorld) * fd_Lambert();
    indirectDiffuse *= diffuseColor;
    color += indirectDiffuse;

    // indirect specular
    float NoV = abs(dot(normalWorld, -V)) + 1e-5;
    vec2 dfg = prefilteredDFG_Karis(roughness, NoV);
    vec3 specularColor = f0 * dfg.x + dfg.y;
    vec3 ibl = inReflect.xyz * specularColor;
    color += ibl * indirectIntensity * pow(1-roughness, 5);
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

    //color = vec3(shadowVal);

    // output  
    fragOut = vec4(color, gb2.w);  
}
