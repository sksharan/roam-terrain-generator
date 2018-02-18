#include "ObjectTest.h"
#include "Globals.h"
#include "GridTerrain.h"
#include "ROAMTerrain.h"
#include "Object.h"
#include "Utils.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

bool ObjectTest::gen_triangle(bool use_indices) {
    GLuint shaders[2];
    shaders[0] = Utils::create_shader("shaders/simple.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/simple.frag", GL_FRAGMENT_SHADER);
    if (shaders[0] == 0 || shaders[1] == 0) {
        return false;
    }

    GLuint program = Utils::create_program(shaders, 2);
    if (program == 0) {
        return false;
    }

    Object simple;
    std::vector<float> v = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
    std::vector<float> t = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> n = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<GLuint> i = { 0, 1, 2 };

    simple.create_vao();
    simple.set_vertices(v, GL_STATIC_DRAW);
    simple.set_texcoords(t, GL_STATIC_DRAW);
    simple.set_normals(n, GL_STATIC_DRAW);
    if (use_indices) {
        simple.toggle_indexed_draw();
        simple.set_indices(i, GL_STATIC_DRAW);
    }
    simple.set_program(program);
    simple.set_draw_mode(GL_TRIANGLES);

    Globals::renderer.add_object(simple);

    return true;
}

bool ObjectTest::use_matrices() {
    GLuint shaders[2];
    shaders[0] = Utils::create_shader("shaders/matrix.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/matrix.frag", GL_FRAGMENT_SHADER);
    if (shaders[0] == 0 || shaders[1] == 0) {
        return false;
    }

    GLuint program = Utils::create_program(shaders, 2);
    if (program == 0) {
        return false;
    }

    Object obj;
    std::vector<float> v = { -0.5f, -0.5f, -1.0f, 0.5f, -0.5f, -1.0f, 0.0f, 0.5f, -1.0f };
    std::vector<float> t = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> n = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

    obj.create_vao();
    obj.set_vertices(v, GL_STATIC_DRAW);
    obj.set_texcoords(t, GL_STATIC_DRAW);
    obj.set_normals(n, GL_STATIC_DRAW);

    obj.set_program(program);

    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0, 0, -1.0));
    glm::mat4 view = glm::mat4();
    glm::mat4 projection = glm::perspective(45.0, 1.0, 0.1, 20.0);
    obj.set_model_matrix_uniform("model", model);
    obj.set_view_matrix_uniform("view", view);
    obj.set_projection_matrix_uniform("projection", projection);

    Globals::renderer.add_object(obj);

    return true;
}

bool ObjectTest::use_indices_and_matrices(bool use_triangle_fan) {
    GLuint shaders[2];
    shaders[0] = Utils::create_shader("shaders/matrix.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/matrix.frag", GL_FRAGMENT_SHADER);
    if (shaders[0] == 0 || shaders[1] == 0) {
        return false;
    }

    GLuint program = Utils::create_program(shaders, 2);
    if (program == 0) {
        return false;
    }

    Object obj;
    std::vector<float> v = { -0.5f,  -0.5f, -1.0f,
                              0.5f,  -0.5f, -1.0f,
                              0.5f,   0.5f, -1.0f,
                             -0.5f,   0.5f, -1.0f };
    std::vector<float> t = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> n = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<GLuint> i;
    if (use_triangle_fan) {
        i = { 0, 1, 2, 3 };
    } else {
        i = { 0, 1, 2, 0, 2, 3 };  //2 triangles using GL_TRAINGLES draw mode
    }

    obj.create_vao();
    obj.set_vertices(v, GL_STATIC_DRAW);
    obj.set_texcoords(t, GL_STATIC_DRAW);
    obj.set_normals(n, GL_STATIC_DRAW);
    obj.toggle_indexed_draw();
    obj.set_indices(i, GL_STATIC_DRAW);

    obj.set_program(program);

    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 projection = glm::perspective(45.0, 1.0, 0.1, 20.0);
    obj.set_model_matrix_uniform("model", model);
    obj.set_view_matrix_uniform("view", view);
    obj.set_projection_matrix_uniform("projection", projection);

    if (use_triangle_fan) {
        obj.set_draw_mode(GL_TRIANGLE_FAN);
    }

    Globals::renderer.add_object(obj);

    return true;
}

bool ObjectTest::grid_terrain() {
    GLuint shaders[2];
    shaders[0] = Utils::create_shader("shaders/matrix.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/matrix.frag", GL_FRAGMENT_SHADER);
    if (shaders[0] == 0 || shaders[1] == 0) {
        return false;
    }

    GLuint program = Utils::create_program(shaders, 2);
    if (program == 0) {
        return false;
    }

    GridTerrain gt(50, 0.5, 1.0);  //length is (50-1)*0.5 = 24.5
    Object obj = gt.get_object();

    obj.create_vao();
    obj.set_program(program);

    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 projection = glm::perspective(45.0, 1.0, 0.1, 60.0);
    obj.set_model_matrix_uniform("model", model);
    obj.set_view_matrix_uniform("view", view);
    obj.set_projection_matrix_uniform("projection", projection);

    Globals::renderer.add_object(obj);

    return true;
}

bool ObjectTest::roam_terrain() {
    ROAMTerrain rt(glm::vec3(0, 0, 0), glm::vec3(24.5, 0, -24.5)); //length is 24.5
    Object obj = rt.get_object();
    Globals::renderer.add_object(obj);
    return true;
}