#ifndef HYBRID_CORE_UTILS
#define HYBRID_CORE_UTILS

//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------
// TODO: might create a seperate file
struct hybrid_PointLight {
    vec3 color;
    vec3 position;

    float attenuationCoeff; // TODO: add constant and linear term
};

struct hybrid_PBRMaterial {
    vec3 albedo;
    float roughness;
    float metallic;
};

//--------------------------------------------------------------------------------------
// default layouts
//--------------------------------------------------------------------------------------
layout(set = 0, binding = 0) uniform HYBRID_CORE_UNIFORM_DATA {
    mat4 _projection;
    mat4 _view;
    vec4 _zBufferParams;
    vec4 _projectionParams; // x: near, y: far, z: resX, w: resY
    float _time;
};

// TODO: set different names for read and write gbuffers
// gbuffer 0: xyz: albedo,          w: roughness  
// gbuffer 1: xyz: world pos,       w: metallic
// gbuffer 2: xyz: world normal,    w: 0 -> background 1 -> object
#define HYBRID_CORE_GBUFFER_TARGET \
layout(location = 0) out vec4 gbuffer0; \
layout(location = 1) out vec4 gbuffer1; \
layout(location = 2) out vec4 gbuffer2; \

#define HYBRID_CORE_GBUFFER_SAMPLER \
layout(set = 1, binding = 0) uniform sampler2D gbuffer0; \
layout(set = 1, binding = 1) uniform sampler2D gbuffer1; \
layout(set = 1, binding = 2) uniform sampler2D gbuffer2; \

layout(set = 1, binding = 4) uniform sampler2D _environmentMap;

// TODO: this will be replaced with light buffer
#define HYBRID_LIGHT_COUNT 3
const hybrid_PointLight _lights[HYBRID_LIGHT_COUNT] = {
    {vec3(0.5), vec3(   0,    1,      1.5),         1},
    {vec3(0.5), vec3(   1,    0.1,    1.5),         1},
    {vec3(0.5), vec3(  -1,    0.1,    1.5),         1},
};

//--------------------------------------------------------------------------------------
// macros
//--------------------------------------------------------------------------------------
#define HYBRID_BACKGROUND_FLAG 0
#define HYBRID_OBJECT_FLAG 1

//--------------------------------------------------------------------------------------
// utils
//--------------------------------------------------------------------------------------
#define MAX_FLOAT 3.402823466e+38

// depth to eye z distance
float hybrid_depthToEyeZ(float depth, in vec3 direction) {
    const vec3 eyeForward = vec3(0.,0.,-1.) * mat3(_view);
    return depth * dot(direction, eyeForward);
}

// linear depth to z buffer depth
float hybrid_linearToZDepth(float z) {
    return (1.0 / z - _zBufferParams.w) / _zBufferParams.z;
}

// sample environment map by normalized direction vector
vec4 hybrid_sampleEnvironmentMap(vec3 dir) {
    const float PI = 3.14159265359;
    const vec2 uv = vec2(atan(dir.x, dir.z) / (2.0 * PI) + 0.5, asin(-dir.y) / PI + 0.5);
    return texture(_environmentMap, uv);
}

#endif