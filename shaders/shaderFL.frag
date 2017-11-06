#version 330

in VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} fs_in;

uniform sampler2D tex;
uniform vec3 camPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform float spotlightFull;
uniform float spotlightFade;
uniform vec3 spotlightBrightness;

out vec4 frag_color;

float smoothstep(float min, float max, float x) {
    x -= min;
    x /= max-min;
    if(x < 0) {return 0.f;}
    else if(x > 1) {return 1.f;}
    else {return 3*x*x - 2*x*x*x;}
}

void main(void) {
	frag_color = texture2D(tex, fs_in.uv);
	
	vec3 lv = spotlightPos - fs_in.worldPos;
	float l = length(lv);
	float n = length(fs_in.norm);
	vec3 brightness = smoothstep(spotlightFade, spotlightFull, dot(spotlightDir, -lv) / l)/(l*l) * spotlightBrightness;
	if(brightness.x > 0) {
	    float ld = dot(lv, fs_in.norm) / l / n;
	    if(ld < 0) {ld = 0;}
	    float ls = dot(camPos-fs_in.worldPos, 2*dot(lv, fs_in.norm)*fs_in.norm/(n*n) - lv) / length(camPos-fs_in.worldPos) / l;
	    ls = pow(ls, shininess) * ld;
	    if(ls < 0) {ls = 0;}
	    brightness *= ka + kd*ld + ks*ls;
	}
	frag_color *= vec4(brightness, 1);
}

