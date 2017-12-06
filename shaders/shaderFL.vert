#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

out VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} vs_out;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 normalMat;

void main(void) {
	vec4 v = vec4(v_position, 1.0);
	gl_Position = (projMat * viewMat * modelMat) * v;
	vs_out.worldPos = (modelMat * v).xyz;
	vs_out.uv = v_uv;
	vs_out.norm = (normalMat * vec4(v_normal, 0)).xyz;
	vs_out.norm /= length(vs_out.norm);
}

