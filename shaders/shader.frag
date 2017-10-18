#version 330

in VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} fs_in;

uniform sampler2D tex;
uniform float ka;
uniform float kd;
uniform float ks;
uniform vec3 camPos;
uniform int reflectOnlyBlue;
uniform float sunR;
uniform vec3 shadowP;
uniform float shadowR;

out vec4 frag_color;

void main(void) {
	frag_color = texture2D(tex, fs_in.uv);
	if(frag_color.a < 0.3) {discard;}
	
	vec3 r = fs_in.worldPos - 2*dot(fs_in.worldPos, fs_in.norm)*fs_in.norm;

	float brightness = 1;//dot(fs_in.worldPos, fs_in.worldPos);
	float la = 1, ld = 0, ls = 0;
	ld = dot(-fs_in.worldPos/length(fs_in.worldPos), fs_in.norm);
	if(length(r) != 0) {ls = dot(-r/length(r), camPos)/length(camPos);}
	if(ld < 0) {ld = 0;}
	ls = pow(ls, 9) * ld;
	if(ls < 0) {ls = 0;}
	if(reflectOnlyBlue == 1) {if(frag_color.b < 0.4 || frag_color.r > 0.4 || frag_color.g > 0.4) {ls = 0;}}
	if(kd < 0 && ld > 0.1 + 0.04*cos(fs_in.uv.x*100)) {discard;}
	if(shadowR > 0.f && kd >= 0) {
	    float r, d, h, e;
	    r = length(fs_in.worldPos - shadowP);
	    d = dot(fs_in.worldPos, fs_in.worldPos - shadowP)/length(fs_in.worldPos);
	    if(d > 0) {
	        h = sqrt(r*r - d*d);
	        e = sunR*d/length(fs_in.worldPos);
	        if(shadowR > e) {
	            float x = (h+e-shadowR)/(2*e);
	            if(x > 1) {x = 1;}
	            if(x < 0) {x = 0;}
	            float y = 3*x*x - 2*x*x*x;
	            if(ld > 0) {ld *= y;}
	            if(ls > 0) {ls *= y;}
	        } else {
	            float x = (h-e+shadowR)/(2*shadowR);
	            if(x > 1) {x = 1;}
	            if(x < 0) {x = 0;}
	            float y = (e*e - shadowR*shadowR)/(e*e) + (1 - (e*e - shadowR*shadowR)/(e*e)) * (3*x*x - 2*x*x*x);
	            if(ld > 0) {ld *= y;}
	            if(ls > 0) {ls *= y;}
	        }
	    }
	}
	brightness *= ka*la + kd*ld + ks*ls;
	frag_color *= brightness;
}

