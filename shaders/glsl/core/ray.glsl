#ifndef HYBRID_CORE_RAY
#define HYBRID_CORE_RAY

//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------
struct Ray {
    vec3 origin;
    vec3 direction;
};

//--------------------------------------------------------------------------------------
// ray functions
//--------------------------------------------------------------------------------------
Ray createRay(vec3 origin, vec3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;

    return ray;
}

Ray createCameraRay(vec2 uv, in mat4 projectionInverse, in mat4 viewInverse) {
    // camera origin
    vec3 origin = (viewInverse*vec4(0,0,0,1)).xyz;
    
    // invert the perspective projection of the view-space position
    vec3 direction = (projectionInverse * vec4(uv, 0.0f, 1.0f)).xyz;

    // direction from camera to world space and normalize
    direction = normalize((viewInverse * vec4(direction, 0.0f)).xyz);

    return createRay(origin, direction);
}

#endif