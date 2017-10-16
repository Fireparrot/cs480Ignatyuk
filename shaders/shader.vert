#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

out VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} vs_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void) {
	vec4 v = vec4(v_position, 1.0);
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v;
	vs_out.worldPos = (modelMatrix * v).xyz;
	vs_out.uv = v_uv;
	vs_out.norm = (modelMatrix * vec4(v_normal, 0)).xyz;
	vs_out.norm /= length(vs_out.norm);
}

