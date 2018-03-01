#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex0;

in vec3 f_pos_obj;
in vec2 f_tex;
in vec3 f_norm;

out vec4 color;

/* Adds influence of fog to 'color'. */
void add_fog(inout vec4 color) {
    //code from OpenGL 4.0 Shading Language Cookbook
    float dist = abs((view * model * vec4(f_pos_obj, 1)).z);
    float fogfactor = (2500.0 - dist) / (2500.0 - 250.0);
    fogfactor = clamp(fogfactor, 0, 1);
    color = mix(vec4(0.6, 0.6, 0.6, 1), color, fogfactor);
}

void main() {
    color = texture(tex0, f_tex);
    if (color.a < 0.7) {
        discard;
    }
    add_fog(color);
}
