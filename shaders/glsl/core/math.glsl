#ifndef HYBRID_CORE_MATH
#define HYBRID_CORE_MATH

//--------------------------------------------------------------------------------------
// math helpers
//--------------------------------------------------------------------------------------
#ifndef PI 
#define PI 3.14159265359
#endif

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec3 rot3D(in vec3 p, in vec3 axis, float angle) {
    return mix(dot(axis, p) * axis, p , cos(angle))
           + cross(axis, p) * sin(angle);
}

#endif