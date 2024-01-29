#version 450
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"

//--------------------------------------------------------------------------------------
// inputs
//--------------------------------------------------------------------------------------
// Position , uv, normal of the hybrid::Vertex struct
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(set = 0, binding = 1) uniform ModelData {
    mat4 _model;
};
//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout(location = 0) out FragData {
    vec3 positionWorld;
    vec3 normalWorld;
    vec2 uv;
} frag;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    vec4 worldPos = _model * vec4(position, 1);
    vec3 worldNormal = normalize((_model * vec4(normal, 0)).xyz);

    gl_Position = _projection * _view * worldPos;
    
    frag.positionWorld = worldPos.xyz;
    frag.normalWorld = worldNormal.xyz;
    frag.uv = uv;
}