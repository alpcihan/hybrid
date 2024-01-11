#version 450
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

layout(set = 1, binding = 3 ) readonly buffer shadowStorage{
    float shadowMap[];
};
//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;
layout (set = 2, binding = 0) uniform sampler2D _specularReflectionMap;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout (location = 0) out vec4 fragOut;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
const vec3 ambient = vec3(0.0001);

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
    const vec4  reflectionMap   = texture(_specularReflectionMap, uv);

    const float ao = 0.01;
    const vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;

    //TODO: Check max light count before loop
    uint shadowIdx = texCoords.y * bounds.x + texCoords.x;
    float shadowVal = shadowMap[shadowIdx];

    vec3 color = vec3(0.0);
	for(int i = 0; i < HYBRID_LIGHT_COUNT; ++i){
        color += ambient * albedo * ao;
        vec3 Lo =  calculatePBRFromActiveSceneLights(
                albedo,
                roughness,
                metallic,
                positionWorld,
                normalWorld,
                viewPos,
                i);

        color += Lo * shadowVal;
    }

    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    //--------------------------------------------------------------------------------------
    vec3 shadowColor = vec3(shadowVal);
    // output
    fragOut = vec4(color, 1);
    //fragOut = vec4(shadowColor,1); 
}
