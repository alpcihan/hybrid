#version 450
#extension GL_ARB_separate_shader_objects : enable

//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------
struct Ray {
    vec3 origin;
    vec3 direction;
};

//--------------------------------------------------------------------------------------
// uniform buffer
//--------------------------------------------------------------------------------------
layout(set = 0, binding = 0) uniform UniformData {
    mat4 u_projection;
    mat4 u_view;
};

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
layout(location = 0) out vec4 gbuffer0; // xyz: diffuse color, w: -
layout(location = 1) out vec4 gbuffer1; // xyzw: -
layout(location = 2) out vec4 gbuffer2; // xyzw: -

//--------------------------------------------------------------------------------------
// program
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

float distance_from_sphere(in vec3 p, in vec3 c, float r)
{
	return length(p - c) - r;
}

vec3 ray_march(in vec3 ro, in vec3 rd)
{
    float total_distance_traveled = 0.0;
    const int NUMBER_OF_STEPS = 32;
    const float MINIMUM_HIT_DISTANCE = 0.001;
    const float MAXIMUM_TRACE_DISTANCE = 1000.0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        // Calculate our current position along the ray
        vec3 current_position = ro + total_distance_traveled * rd;

        // We wrote this function earlier in the tutorial -
        // assume that the sphere is centered at the origin
        // and has unit radius
        vec3 sphere_center = vec3(0,0,-1);
        float distance_to_closest = distance_from_sphere(current_position, sphere_center, 0.5);

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) // hit
        {
            // We hit something! Return normal color for now
            return (normalize(current_position - sphere_center)) * 0.5 + 0.5;
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE) // miss
        {
            break;
        }

        // accumulate the distance traveled thus far
        total_distance_traveled += distance_to_closest;
    }

    // If we get here, we didn't hit anything so just
    // return a background color (black)
    return vec3(0.0);
}

void main()  {
    // output
    gl_FragDepth = 0.99999;

    Ray ray = createCameraRay(uv * 2 - 1, inverse(u_projection), inverse(u_view));
    
    vec3 color = ray_march(ray.origin, ray.direction);

    gbuffer0.xyz = color;
}
