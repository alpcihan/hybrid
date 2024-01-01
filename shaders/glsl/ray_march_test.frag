#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

//--------------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------------
#include "core/core.glsl"
#include "core/ray.glsl"

//--------------------------------------------------------------------------------------
// inputs 
//--------------------------------------------------------------------------------------
layout (location = 0) in vec2 uv;

//--------------------------------------------------------------------------------------
// outputs
//--------------------------------------------------------------------------------------
HYBRID_CORE_GBUFFER_TARGET

//--------------------------------------------------------------------------------------
// program
//--------------------------------------------------------------------------------------
const int max_iterations = 255;
const float march_stop_threshold = 0.001;

float dist_sphere(in vec3 p, in vec3 c, float r)
{
	return length(p - c) - r;
}

float ray_march(in vec3 origin, in vec3 dir, float start, float end, out vec3 position, out vec3 normal) {
    
    normal = vec3(0,0,0);
    float depth = start;

    for (int i = 0; i < max_iterations; i++) {
        vec3 current_pos = origin + dir * depth;

        vec3 sphere_center = vec3(0,0,0);
        float sphere_radius = 0.2;

        float dist = dist_sphere( current_pos, sphere_center, sphere_radius );

        if ( dist < march_stop_threshold ) {
            position = current_pos;
            normal = normalize(current_pos - sphere_center);

            return depth;
        }

        depth += dist;
        if ( depth >= end) {
            return end;
        }

    }
    return end;
}

void main()  {
    Ray ray = createCameraRay(uv*2-1, inverse(_projection), inverse(_view));
    
    vec3 positionWorld;
    vec3 normalWorld;
    float depth = ray_march(ray.origin, ray.direction, _projectionParams[0], _projectionParams[1], positionWorld, normalWorld);

    if(depth >= _projectionParams[1]) {
      gl_FragDepth = 1;
      return;
    } 

    // output
    gl_FragDepth = linearToZDepth(distanceToEyeZ(depth, ray.direction));
    gbuffer0.xyz = vec3(0.7,0.2,0.2);
    gbuffer1.xyz = positionWorld;
    gbuffer2.xyz = normalWorld;
}
