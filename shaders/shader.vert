#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

smooth out vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform sampler2D tex;

void main(void) {
	vec4 v = vec4(v_position, 1.0);
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v;
	color = texture2D(tex, v_uv);
}

