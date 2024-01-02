#ifndef HYBRID_CORE_SDF
#define HYBRID_CORE_SDF

//--------------------------------------------------------------------------------------
// signed distance functions
//--------------------------------------------------------------------------------------
// https://iquilezles.org/articles/distfunctions/
float sdSphere(in vec3 p, float r) {
    return length(p) - r;
}

// https://iquilezles.org/articles/distfunctions/
float sdBox(in vec3 p, in vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// https://iquilezles.org/articles/distfunctions/
float sdRoundBox(in vec3 p, in vec3 b, float r) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float sdZPlane(in vec3 p, in float y) {
    return p.y - y;
}

#endif