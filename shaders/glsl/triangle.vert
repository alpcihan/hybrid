#version 450

//--------------------------------------------------------------------------------------
// uniform buffer
//--------------------------------------------------------------------------------------
layout(set = 0, binding = 0) uniform UniformData {
    mat4 u_projection;
    mat4 u_view;
};

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
layout(location = 0) out Frag {
    vec3 color;
} frag;

void main() {
    vec3 worldPos = vec3(positions[gl_VertexIndex % positions.length()], -3);

    gl_Position = u_projection * u_view * vec4(worldPos, 1.0);
    
    frag.color = colors[gl_VertexIndex];
}
