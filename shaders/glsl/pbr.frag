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

    const float ao = 0.01;
    const vec3 viewPos = (inverse(_view)*vec4(0,0,0,1)).xyz;
    const vec3 V = normalize(viewPos - positionWorld);  

    // direct lights          
    vec3 Lo = calculatePBRLoFromSceneLights(
                albedo,
                roughness,
                metallic,
                positionWorld,
                normalWorld,
                viewPos);

    // reflection
    float pyramidIdx = pow(roughness,1) * _specularReflectionPyramidSize;
    float pyramid_a = fract(pyramidIdx);
    int pyramidIdx_i = int(pyramidIdx);
    vec4 inReflect;
    
    if(pyramidIdx < _specularReflectionPyramidSize - 1) {
        inReflect = mix(
            texture(_specularReflectionPyramid[pyramidIdx_i],   uv),
            texture(_specularReflectionPyramid[pyramidIdx_i+1], uv),
            pyramid_a);
    } else {
        inReflect = texture(_specularReflectionPyramid[pyramidIdx_i],   uv);
    }

    Lo += calculatePBRLo(
                albedo,
                roughness,
                metallic,
                normalWorld,
                V,
                inReflect.xyz,
                reflect(-V, normalWorld)
                );
   
    
    // final color
    vec3 color = Lo + ambient * albedo * ao;

    // shadow
	uint shadowIdx = texCoords.y * bounds.x + texCoords.x;
    float shadowCoeff = shadowMap[shadowIdx];
    color *= shadowCoeff;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

    // output  
    fragOut = vec4(color, gb2.w);  
}
