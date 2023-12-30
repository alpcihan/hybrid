#version 450
#extension GL_ARB_separate_shader_objects : enable

//--------------------------------------------------------------------------------------
// uniform buffer
//--------------------------------------------------------------------------------------
layout(set = 0, binding = 0) uniform UniformData {
    mat4 u_projection;
    mat4 u_view;
};

//--------------------------------------------------------------------------------------
// g-buffer (read)
//--------------------------------------------------------------------------------------
layout(set = 1, binding = 0) uniform sampler2D gbuffer0;
layout(set = 1, binding = 1) uniform sampler2D gbuffer1;
layout(set = 1, binding = 2) uniform sampler2D gbuffer2;

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
void main()  {
    // read g-buffer
    vec4 diffuse   = texture(gbuffer0, uv);
    vec4 position  = texture(gbuffer1, uv);
    vec4 normal    = texture(gbuffer2, uv);

    // output
    fragOut = diffuse;
}
