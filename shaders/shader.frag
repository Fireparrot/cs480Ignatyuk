#version 330

in VD {
    vec3 modelPos;
    vec2 uv;
    vec3 norm;
} fs_in;

uniform sampler2D tex;
uniform bool useLighting;

out vec4 frag_color;

void main(void) {
	frag_color = texture2D(tex, fs_in.uv);
	if(useLighting) {
	    frag_color *= dot(fs_in.modelPos, -fs_in.norm)/length(fs_in.modelPos)/length(fs_in.norm);
	    frag_color /= dot(fs_in.modelPos, fs_in.modelPos)/64;
	}
}

