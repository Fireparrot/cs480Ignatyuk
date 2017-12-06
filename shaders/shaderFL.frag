#version 330

in VD {
    vec3 worldPos;
    vec2 uv;
    vec3 norm;
} fs_in;

uniform sampler2D tex;
uniform vec3 camPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

out vec4 frag_color;



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
	frag_color = texture2D(tex, fs_in.uv);
	
	vec3 brightness = vec3(0, 0, 0);
	vec3 normal = fs_in.norm/length(fs_in.norm);
	vec3 camDir = (camPos-fs_in.worldPos)/length(camPos-fs_in.worldPos);
	vec3 PLdir = vec3(0, 10, 0)-fs_in.worldPos;
	float PLdistance = length(PLdir);
	PLdir /= PLdistance;
	brightness += lighting(PLdir, normal, camDir, vec3(100, 100, 100) / (PLdistance*PLdistance));
	frag_color *= vec4(brightness, 1);
}

