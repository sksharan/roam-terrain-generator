/* Fragment shader for ROAMTerrain. */

#version 430

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex0; //grass
uniform sampler2D tex1; //dirt
uniform sampler2D tex2; //rock
uniform sampler2D tex3; //foliage

in vec3 f_pos_obj;
in vec2 f_tex;
in vec3 f_norm;
flat in int f_is_grass;

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
    /* Texture grass. */
    if (f_is_grass == 1) {
        color = texture(tex3, f_tex);
        if (color.a < 0.7) {
            discard;
        }
        add_fog(color);
        return;
    }

    /* Texture landscape. The texture of the landscape depends on the slope
    of the terrain. We take the dot product of the normal with the up vector 
    (0, 1, 0) to get a value from -1 to 1. The dot product of two normalized
    vectors is 0 if they are perpendicular (orthogonal) and 1 if they are
    facing in the same direction. In our case, we expect values from 0 to 1,
    so we have flat land if we have a dot product close to 1, and we have
    steeper land if we have a dot product closer to 0. */

    float tex_dot_prod = dot(f_norm, vec3(0, 1, 0));
    if (tex_dot_prod < 0.8) {
        color = texture(tex2, f_tex);
    }
    else if (tex_dot_prod < 0.9) {
        color = mix(texture(tex2, f_tex),
                    texture(tex1, f_tex),
                    (tex_dot_prod - 0.8) / (1 - 0.8));
    } 
    else {
        color = mix(texture(tex1, f_tex),
                    texture(tex0, f_tex),
                    (tex_dot_prod - 0.9) / (1 - 0.9));
    }
    add_fog(color);
}