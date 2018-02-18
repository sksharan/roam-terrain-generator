/* Vertex shader for ROAMTerrain. */

#version 430

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 v_pos_obj;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_norm;

out vec3 g_pos_obj;
out vec2 g_tex;
out vec3 g_norm;

void main() {
    gl_Position = projection * view * model * vec4(v_pos_obj, 1.0);
    g_pos_obj = v_pos_obj;
    g_tex = v_tex;
    g_norm = v_norm;
}