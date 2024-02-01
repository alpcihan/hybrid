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



//https://www.shadertoy.com/view/XsBXWt

// 2D rotation function
mat2 rot(float a) {
	return mat2(cos(a),sin(a),-sin(a),cos(a));	
}

// "Amazing Surface" fractal
vec4 formula(vec4 p) {
		p.xz = abs(p.xz+1.)-abs(p.xz-1.)-p.xz;
		p.y-=.25;
		p.xy*=rot(radians(35.));
		p=p*2./clamp(dot(p.xyz,p.xyz),.2,1.);
	return p;
}

// Distance function
float de(vec3 pos) {

    float t = _time;
	pos.y+=sin(pos.z-t*6.)*.15; //waves!
	float hid=0.;
	vec3 tpos=pos;
	tpos.z=abs(3.-mod(tpos.z,6.));
	vec4 p=vec4(tpos,1.);
	for (int i=0; i<4; i++) {p=formula(p);}
	float fr=(length(max(vec2(0.),p.yz-1.5))-1.)/p.w;
	float ro=max(abs(pos.x+1.)-.3,pos.y-.35);
		  ro=max(ro,-max(abs(pos.x+1.)-.1,pos.y-.5));
	pos.z=abs(.25-mod(pos.z,.5));
		  ro=max(ro,-max(abs(pos.z)-.2,pos.y-.3));
		  ro=max(ro,-max(abs(pos.z)-.01,-pos.y+.32));
	float d=min(fr,ro);
	return d;
}

#endif