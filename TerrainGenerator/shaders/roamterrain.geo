/* Geometry shader for ROAMTerrain. */

#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(triangles) in;
layout(triangle_strip, max_vertices=27) out;

in vec3 g_pos_obj[];
in vec2 g_tex[];
in vec3 g_norm[];

out vec3 f_pos_obj;
out vec2 f_tex;
out vec3 f_norm;
flat out int f_is_grass;

/* Canonical GLSL random function found on the web. */
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* Generate grass given an index 'i'. */
void gen_grass(int i, bool flip) {
    vec3 vertex = g_pos_obj[i];
    float width = 12.0f;
    float height = 15.0f;
    int modifier;
    if (flip) {
        modifier = -1;
    } else {
        modifier = 1;
    }

    f_pos_obj = vertex + vec3(-width, -height, width * modifier);
    f_tex = vec2(0, 1);
    f_norm = g_norm[i]; //FIXME
    f_is_grass = 1;
    gl_Position = projection * view * model * vec4(f_pos_obj, 1);
    EmitVertex();

    f_pos_obj = vertex + vec3(width, -height, -width * modifier);
    f_tex = vec2(1, 1);
    f_norm = g_norm[i]; //FIXME
    f_is_grass = 1;
    gl_Position = projection * view * model * vec4(f_pos_obj, 1);
    EmitVertex();

    f_pos_obj = vertex + vec3(-width, height, width * modifier);
    f_tex = vec2(0, 0);
    f_norm = g_norm[i]; //FIXME
    f_is_grass = 1;
    gl_Position = projection * view * model * vec4(f_pos_obj, 1);
    EmitVertex();

    f_pos_obj = vertex + vec3(width, height, -width * modifier);
    f_tex = vec2(1, 0);
    f_norm = g_norm[i]; //FIXME
    f_is_grass = 1;
    gl_Position = projection * view * model * vec4(f_pos_obj, 1);
    EmitVertex();

    EndPrimitive();
}

void main() {
    /* Generate the landscape triangle. */
    for (int i = 0; i < gl_in.length(); i++) {
        f_pos_obj = g_pos_obj[i];
        f_tex = g_tex[i];
        f_norm = g_norm[i];
        f_is_grass = 0;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();

    /* Generate grass. */
    for (int i = 0; i < gl_in.length(); i++) {
        /* Skip if grass is not on land that is flat enough. */
        if (dot(vec3(0, 1, 0), f_norm) < 0.99) {
            continue;
        }
        
        gen_grass(i, true);
        gen_grass(i, false);
    }
}