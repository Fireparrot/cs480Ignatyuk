#version 330

in VD {
    vec2 uv;
    vec3 brightness;
} fs_in;

uniform sampler2D tex;
uniform int override;
uniform vec4 overrideColor;

out vec4 frag_color;

void main(void) {
	if(override != 1) {
	    frag_color = texture2D(tex, fs_in.uv);
	} else {
	    frag_color = overrideColor;
	}
	frag_color *= vec4(fs_in.brightness, 1);
}

