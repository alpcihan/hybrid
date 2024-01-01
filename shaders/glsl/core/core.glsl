#ifndef HYBRID_CORE_UTILS
#define HYBRID_CORE_UTILS

#define MAX_FLOAT 3.402823466e+38

//--------------------------------------------------------------------------------------
// default layouts
//--------------------------------------------------------------------------------------
layout(set = 0, binding = 0) uniform HYBRID_CORE_UNIFORM_DATA {
    mat4 _projection;
    mat4 _view;
    vec4 _zBufferParams;
    vec4 _projectionParams; // x: near, y: far, z: -, w: -
};

// gbuffer 0: xyz: diffuse color,   w: -  
// gbuffer 1: xyz: world pos,       w: -
// gbuffer 2: xyz: world normal,    w: - 
#define HYBRID_CORE_GBUFFER_TARGET \
layout(location = 0) out vec4 gbuffer0; \
layout(location = 1) out vec4 gbuffer1; \
layout(location = 2) out vec4 gbuffer2; \

#define HYBRID_CORE_GBUFFER_SAMPLER \
layout(set = 1, binding = 0) uniform sampler2D gbuffer0; \
layout(set = 1, binding = 1) uniform sampler2D gbuffer1; \
layout(set = 1, binding = 2) uniform sampler2D gbuffer2; \

//--------------------------------------------------------------------------------------
// utils
//--------------------------------------------------------------------------------------

// depth to eye z distance
float depthToEyeZ(float depth, in vec3 direction) {
    const vec3 eyeForward = vec3(0.,0.,-1.) * mat3(_view);
    return depth * dot(direction, eyeForward);
}

// linear depth to z buffer depth
float linearToZDepth(float z) {
    return (1.0 / z - _zBufferParams.w) / _zBufferParams.z;
}

#endif