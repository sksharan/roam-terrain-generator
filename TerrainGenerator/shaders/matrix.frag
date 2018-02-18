/* Fragment shader with matrices. */

#version 430

in vec3 f_pos;
in vec2 f_tex;
in vec3 f_norm;

out vec4 color;

void main() {
    //color = vec4(17.0/255.0, 139.0/255.0, 17.0/255.0, 1.0);
    color = normalize(vec4(f_pos.x, 0, f_pos.z, 1.0));
}