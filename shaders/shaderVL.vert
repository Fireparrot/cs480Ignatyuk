#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

out VD {
    vec2 uv;
    vec3 brightness;
} vs_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

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

float smoothstep(float min, float max, float x) {
    x -= min;
    x /= max-min;
    if(x < 0) {return 0.f;}
    else if(x > 1) {return 1.f;}
    else {return 3*x*x - 2*x*x*x;}
}

void main(void) {
	vec4 v = vec4(v_position, 1.0);
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v;
	vec3 worldPos = (modelMatrix * v).xyz;
	vec3 normal = (normalMatrix * vec4(v_normal, 0)).xyz;
	normal /= length(normal);
	vs_out.uv = v_uv;
	
	vec3 lv = spotlightPos - worldPos;
	float l = length(lv);
	float n = length(normal);
	vec3 brightness = smoothstep(spotlightFade, spotlightFull, dot(spotlightDir, -lv) / l)/(l*l) * spotlightBrightness;
	if(brightness.x > 0) {
	    float ld = dot(lv, normal) / l / n;
	    if(ld < 0) {ld = 0;}
	    float ls = dot(camPos-worldPos, 2*dot(lv, normal)*normal/(n*n) - lv) / length(camPos-worldPos) / l;
	    ls = pow(ls, shininess) * ld;
	    if(ls < 0) {ls = 0;}
	    brightness *= ka + kd*ld + ks*ls;
	}
	vs_out.brightness = brightness;
}

