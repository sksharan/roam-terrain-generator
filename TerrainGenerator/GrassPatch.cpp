#include "GrassPatch.h"
#include "Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <iostream>

/* A template object from which all other GrassPatch objects are made. This allows for
rapid creation of multiple GrassPatch objects. */
Object GrassPatch::_static_obj;

void GrassPatch::init(const Configuration& configuration) {
    /* Initialize the static 'template' object. */
    GLuint shaders[2];
    shaders[0] = Utils::create_shader("shaders/grass.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/grass.frag", GL_FRAGMENT_SHADER);
    assert(shaders[0] > 0 && shaders[1] > 0);
    GLuint program = Utils::create_program(shaders, 2);
    assert(program > 0);
    _static_obj.create_vao();
    _static_obj.set_program(program);
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 projection = glm::perspective(45.0f, 1.0f, configuration.perspective_near, configuration.perspective_far);
    _static_obj.set_model_matrix_uniform("model", model);
    _static_obj.set_view_matrix_uniform("view", view);
    _static_obj.set_projection_matrix_uniform("projection", projection);
    _static_obj.set_texture(0, "textures/grass_blades.png", "tex0");
}

GrassPatch::GrassPatch(glm::vec3 lowest_extent, glm::vec3 highest_extent, Configuration& configuration)
    : _configuration(configuration) {
    _obj = _static_obj; // make a copy of the static 'template' object
    _obj.create_vao();

    float height = configuration.grass_height;
    float width = configuration.grass_width;
    float yAdjust = configuration.grass_y_adjust;
    // Vertices of grass quad 1
    std::vector<float> verts = {
        -width, -height + yAdjust,  width,
         width, -height + yAdjust, -width,
        -width,  height + yAdjust,  width,
         width,  height + yAdjust, -width,
         width, -height + yAdjust, -width,
        -width,  height + yAdjust,  width,
    };
    // Vertices of grass quad 2
    for (int i = 0; i < 18; i += 3) {
        glm::mat4 rotation = glm::rotate(3.14f/2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 grassPoint = glm::mat4();
        grassPoint[3][0] = verts[i];
        grassPoint[3][1] = verts[i + 1];
        grassPoint[3][2] = verts[i + 2];
        glm::mat4 rotated = rotation * grassPoint;
        verts.push_back(rotated[3][0]);
        verts.push_back(rotated[3][1]);
        verts.push_back(rotated[3][2]);
    }
    // Vertices of grass quad 3
    for (int i = 0; i < 18; i += 3) {
        glm::mat4 rotation = glm::rotate(3.14f / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 grassPoint = glm::mat4();
        grassPoint[3][0] = verts[i];
        grassPoint[3][1] = verts[i + 1];
        grassPoint[3][2] = verts[i + 2];
        glm::mat4 rotated = rotation * grassPoint;
        verts.push_back(rotated[3][0]);
        verts.push_back(rotated[3][1]);
        verts.push_back(rotated[3][2]);
    }
    _obj.set_vertices(verts, GL_STATIC_DRAW);

    std::vector<float> texcoords = {
        0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, // grass quad 1
        0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, // grass quad 2
        0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, // grass quad 3
    };
    _obj.set_texcoords(texcoords, GL_STATIC_DRAW);

    // No normals for grass
    std::vector<float> normals = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // grass quad 1
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // grass quad 2
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // grass quad 3
    };
    _obj.set_normals(normals, GL_STATIC_DRAW);

    std::vector<float> grassRootPositions;
    for (float x = lowest_extent.x; x < highest_extent.x; x += _configuration.grass_separation) {
        for (float z = lowest_extent.z; z > highest_extent.z; z -= _configuration.grass_separation) {
            glm::vec3 pos = glm::vec3(x, Utils::get_value(x, 0, z, _configuration), z);

            glm::vec3 edge1 = glm::normalize(pos - glm::vec3(x, Utils::get_value(x, 0, z - 1, _configuration), z - 1));
            glm::vec3 edge2 = glm::normalize(pos - glm::vec3(x + 1, Utils::get_value(x + 1, 0, z, _configuration), z));
            glm::vec3 normal = glm::cross(edge1, edge2);
            if (normal.y < 0.0f) { // ensure the normal always faces upwards
                normal = -normal;
            }
            normal = glm::normalize(normal);

            // Only generate grass if surface is flat enough
            if (glm::dot(glm::vec3(0, 1, 0), normal) > 0.95) {
                grassRootPositions.push_back(pos.x);
                grassRootPositions.push_back(pos.y);
                grassRootPositions.push_back(pos.z);
            }
        }
    }
    GLuint buffer;
    _obj.set_attribute(grassRootPositions, GL_STATIC_DRAW, buffer, 3, 3, 1);
    _obj.set_num_instances(grassRootPositions.size() / 3);
}

Object GrassPatch::get_object() {
    return _obj;
}
