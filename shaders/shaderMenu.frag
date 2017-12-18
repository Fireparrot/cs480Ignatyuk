#version 330

in vec3 pos;
in vec2 uv;
in vec3 normal;

uniform sampler2D tex;
uniform sampler2D overlay;

out vec4 frag_color;

void main() {
    frag_color = texture2D(tex, uv);
}

