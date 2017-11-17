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

uniform vec3 pointlightPos;
uniform vec3 pointlightBrightness;

uniform vec3 dirlightDir;
uniform vec3 dirlightBrightness;

uniform int bumped;

float length2(vec2 v) {return dot(v, v);}
float length2(vec3 v) {return dot(v, v);}
float length2(vec4 v) {return dot(v, v);}

float smoothstep(float min, float max, float x) {
    x -= min;
    x /= max-min;
    if(x < 0) {return 0.f;}
    else if(x > 1) {return 1.f;}
    else {return 3*x*x - 2*x*x*x;}
}

vec3 lighting(vec3 lightDir, vec3 normal, vec3 camDir, vec3 brightness) {
	if(brightness.x > 0) {
	    float ld = dot(lightDir, normal);
	    if(ld < 0) {ld = 0;}
	    float ls = dot(camDir, 2*dot(lightDir, normal)*normal - lightDir);
	    if(ls < 0) {ls = 0;}
	    else {ls = pow(ls, shininess) * ld;}
	    brightness *= ka + kd*ld + ks*ls;
	    return brightness;
	} else {
	    return vec3(0, 0, 0);
	}
}

void main(void) {
	vec4 v = vec4(v_position, 1.0);
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v;
	vec3 worldPos = (modelMatrix * v).xyz;
	vec3 normal = (normalMatrix * vec4(v_normal, 0)).xyz;
	normal /= length(normal);
	vs_out.uv = v_uv;
	
	vec3 brightness = vec3(0, 0, 0);
	vec3 camDir = (camPos-worldPos)/length(camPos-worldPos);
	vec3 SLdir = spotlightPos-worldPos;
	float SLdistance = length(SLdir);
	SLdir /= SLdistance;
	brightness += lighting(SLdir, normal, camDir, smoothstep(spotlightFade, spotlightFull, -dot(spotlightDir, SLdir)) * spotlightBrightness / (SLdistance*SLdistance));
	vec3 PLdir = pointlightPos-worldPos;
	float PLdistance = length(PLdir);
	PLdir /= PLdistance;
	brightness += lighting(PLdir, normal, camDir, pointlightBrightness / (PLdistance*PLdistance));
	brightness += lighting(-dirlightDir, normal, camDir, dirlightBrightness);
	if(bumped == 1) {brightness += vec3(0.6f, 0.6f, 0.6f);}
	vs_out.brightness = brightness;
}

