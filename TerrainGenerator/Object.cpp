#include "Object.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <SOIL.h>

Object::Object() {
    _draw_mode = GL_TRIANGLES;
    _num_vertices = 0;
    _num_indices = 0;
    _use_indices = false;
}

void Object::create_vao() {
    glGenVertexArrays(1, &_vao);
}

void Object::set_vertices(const std::vector<float>& vertices, GLenum usage) {
    _num_vertices = vertices.size();
    std::cout << "Number of triangles: " << _num_vertices / 3 << std::endl;

    glBindVertexArray(_vao);

    glDeleteBuffers(1, &_vbo_vert);
    glGenBuffers(1, &_vbo_vert);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_vert);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], usage);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Object::set_texcoords(const std::vector<float>& texcoords, GLenum usage) {
    glBindVertexArray(_vao);

    glDeleteBuffers(1, &_vbo_tex);
    glGenBuffers(1, &_vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(float), &texcoords[0], usage);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Object::set_normals(const std::vector<float>& normals, GLenum usage) {
    glBindVertexArray(_vao);

    glDeleteBuffers(1, &_vbo_norm);
    glGenBuffers(1, &_vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], usage);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Object::set_indices(const std::vector<GLuint>& indices, GLenum usage) {
    _num_indices = indices.size();

    glBindVertexArray(_vao);

    glDeleteBuffers(1, &_vbo_index);
    glGenBuffers(1, &_vbo_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], usage);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool Object::toggle_indexed_draw() {
    bool old = _use_indices;
    _use_indices = !_use_indices;
    return old;
}

void Object::set_program(GLuint program) {
    _program = program;
}

void Object::set_model_matrix_uniform(std::string model_uni_name, glm::mat4& model) {
    set_mat4_uniform(model_uni_name, model);
    _model = model;
}

void Object::set_view_matrix_uniform(std::string view_uni_name, glm::mat4& view) {
    set_mat4_uniform(view_uni_name, view);
    _view = view;
}

void Object::set_projection_matrix_uniform(std::string projection_uni_name, glm::mat4& projection) {
    set_mat4_uniform(projection_uni_name, projection);
    _projection = projection;
}

void Object::set_texture(int i, std::string tex_filename, std::string sampler_uni_name) {
    glDeleteTextures(1, &_textures[i]);
    
    /* Create texture. */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_textures[i]);
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Set texture data. */
    int width, height;
    unsigned char* data = SOIL_load_image(tex_filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    assert(data != NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    SOIL_free_image_data(data);

    /* Associate sampler in fragment shader with the new texture. */
    GLint sampler_uni_loc = glGetUniformLocation(_program, sampler_uni_name.c_str());
    assert(sampler_uni_loc != -1);
    glUseProgram(_program);
    glUniform1i(sampler_uni_loc, i);
    glUseProgram(0);
} 

void Object::set_draw_mode(GLenum mode) {
    _draw_mode = mode;
}

void Object::render() {
    glUseProgram(_program);
    glBindVertexArray(_vao);

    for (int i = 0; i < _num_tex_units; i++) {
        if (_textures[i] > 0) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, _textures[i]);
        }
    }

    if (_use_indices) {
        glDrawElements(_draw_mode, _num_indices, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(_draw_mode, 0, _num_vertices);
    }

    for (int i = 0; i < _num_tex_units; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

GLuint Object::get_vao() {
    return _vao;
}

GLuint Object::get_program() {
    return _program;
}

glm::mat4 Object::get_model() {
    return _model;
}

glm::mat4 Object::get_view() {
    return _view;
}

glm::mat4 Object::get_projection() {
    return _projection;
}

GLenum Object::get_draw_mode() {
    return _draw_mode;
}

int Object::get_num_vertices() {
    return _num_vertices;
}

int Object::get_num_indices() {
    return _num_indices;
}

bool Object::uses_index_draw() {
    return _use_indices;
}

/* Give 'uni_name' in this object's program the value 'mat'. */
void Object::set_mat4_uniform(std::string uni_name, glm::mat4& mat) {
    glUseProgram(_program);
    GLint uni_loc = glGetUniformLocation(_program, (GLchar*)uni_name.c_str());
    if (uni_loc != -1) {
        glUniformMatrix4fv(uni_loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
    glUseProgram(0);
}