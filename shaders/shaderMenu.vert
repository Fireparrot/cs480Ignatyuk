#version 330

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

uniform mat4 modelMat = mat4(1.0f);
uniform mat4 viewMat = mat4(1.0f);
uniform mat4 projMat = mat4(1.0f);
uniform mat4 normalMat = mat4(1.0f);

out vec3 pos;
out vec2 uv;
out vec3 normal;

void main() {
    gl_Position = projMat * viewMat * modelMat * vec4(v_pos, 1.0f);
    pos = (modelMat * vec4(v_pos, 1.0f)).xyz;
    uv = v_uv;
    normal = v_normal;
}

