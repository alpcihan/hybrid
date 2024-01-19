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
// https://iquilezles.org/articles/menger/
float sdMengerSponge( in vec3 p ) {
    float d = sdBox(p,vec3(1.0));
    
    float s = 1.0;
    for( int m=0; m<4; m++ ){
        vec3 a = mod( (p - vec3(m+1, m+1, m+1)) *s, 2.0 )-1.0;
        s *= 3.0;
        vec3 r = abs(2.0 - 3.0*abs(a));

        float da = max(r.x,r.y);
        float db = max(r.y,r.z);
        float dc = max(r.z,r.x);
        float c = (min(da,min(db,dc))-1.0)/s;

        d = max(d,c);
    }

    return d;
}

#endif