#version 330

in VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} fs_in;

uniform sampler2D tex;

out vec4 frag_color;

void main(void) {
	frag_color = texture2D(tex, fs_in.uv);
	if(frag_color.a < 0.3) {discard;}
	/*
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
	brightness *= ka*la + kd*ld + ks*ls;
	frag_color *= brightness;
	*/
}

