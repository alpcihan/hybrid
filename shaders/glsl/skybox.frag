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
layout(set = 1, binding = 4) uniform sampler2D _skybox;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout (location = 0) out vec4 fragOut;

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
void main() {
    // TODO: handle this with pipeline's depth test
    if(texture(gbuffer2, uv).w != HYBRID_BACKGROUND_FLAG) {
        discard;
    }
    // direction
    vec3 dir = (inverse(_projection) * vec4(uv*2-1, 0.0f, 1.0f)).xyz;
    dir = normalize((inverse(_view) * vec4(dir, 0.0f)).xyz);

    fragOut = hybrid_sampleEnvironmentMap(dir) * 0.5;
}
