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

Ray transformRay(in Ray ray,in mat4 transform){

    Ray r = ray;

    vec4 homogeniousOrigin = vec4(r.origin, 1.0);
    vec4 homogeniousDirection = vec4(r.direction, 0.0);

    vec4 transformedOrigin = transform * homogeniousOrigin;
    vec4 transformedDirection =transform * homogeniousDirection;

    r.origin = transformedOrigin.xyz;
    r.direction = transformedDirection.xyz;

    return r;
}

#endif