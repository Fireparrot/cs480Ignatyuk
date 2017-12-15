#version 330

in vec3 pos;
in vec2 uv;
in vec3 normal;

uniform sampler2D tex;
uniform sampler2D overlay;

out vec4 frag_color;

void main() {
    if(false) {
        frag_color = texture2D(tex, uv);
    } else {
        vec4 top = texture2D(overlay, uv);
        vec4 bottom = texture2D(tex, uv);
        frag_color.rgb = top.rgb*top.a + bottom.rgb*(1-top.a);
        frag_color.a = 1-(1-top.a)*(1-bottom.a);
    }
}

