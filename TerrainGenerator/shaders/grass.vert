#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 v_pos_obj;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec3 v_pos_root;

out vec3 f_pos_obj;
out vec2 f_tex;
out vec3 f_norm;

void main() {
    gl_Position = projection * view * model * vec4(v_pos_obj + v_pos_root, 1.0);
    f_pos_obj = v_pos_obj + v_pos_root;
    f_tex = v_tex;
    f_norm = v_norm;
}
