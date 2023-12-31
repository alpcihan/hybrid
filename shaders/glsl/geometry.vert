#version 450
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"

//--------------------------------------------------------------------------------------
// triangle data
//--------------------------------------------------------------------------------------
vec3[3] colors = vec3[](
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1)
);

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout(location = 0) out FragData {
    vec3 positionWorld;
    vec3 normalWorld;
} frag;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    const vec3 worldPos = vec3(positions[gl_VertexIndex % positions.length()], 0);
    const vec3 worldNormal = vec3(0, 0, 1);

    gl_Position = _projection * _view * vec4(worldPos, 1.0);
    
    frag.positionWorld = worldPos;
    frag.normalWorld = worldNormal;
}
