/* Pass-through fragment shader. */

#version 430

in vec3 f_pos;
in vec2 f_tex;
in vec3 f_norm;

out vec4 color;

void main() {
    color = vec4(0.0, 0.0, 1.0, 1.0);
}