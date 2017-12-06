#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

out VD {
    vec2 uv;
    vec3 brightness;
} vs_out;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 normalMat;

uniform vec3 camPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

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
	gl_Position = (projMat * viewMat * modelMat) * v;
	vec3 worldPos = (modelMat * v).xyz;
	vec3 normal = (normalMat * vec4(v_normal, 0)).xyz;
	normal /= length(normal);
	vs_out.uv = v_uv;
	
	vec3 brightness = vec3(0, 0, 0);
	vec3 camDir = (camPos-worldPos)/length(camPos-worldPos);
	vec3 PLdir = vec3(0, 10, 0)-worldPos;
	float PLdistance = length(PLdir);
	PLdir /= PLdistance;
	brightness += lighting(PLdir, normal, camDir, vec3(100, 100, 100) / (PLdistance*PLdistance));
	vs_out.brightness = brightness;
}

