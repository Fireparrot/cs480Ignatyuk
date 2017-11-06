#version 330

in VD {
    vec2 uv;
    vec3 brightness;
} fs_in;

uniform sampler2D tex;

out vec4 frag_color;

void main(void) {
	frag_color = texture2D(tex, fs_in.uv);
	frag_color *= vec4(fs_in.brightness, 1);
}

