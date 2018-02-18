/* Pass-through vertex shader. */

#version 430

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_norm;

out vec3 f_pos;
out vec2 f_tex;
out vec3 f_norm;

void main() {
    gl_Position = vec4(v_pos, 1.0);
    f_pos = v_pos;
    f_tex = v_tex;
    f_norm = v_norm;
}