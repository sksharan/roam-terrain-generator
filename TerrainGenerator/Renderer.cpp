#include "Renderer.h"
#include "Configuration.h"
#include <GL/glew.h>
#include <SDL.h>

Renderer::Renderer() {
}

void Renderer::add_object(Object& object) {
    _render_objects[object.get_vao()] = object;
}

void Renderer::clear() {
    _render_objects.clear();
}

int Renderer::get_num_objects() {
    return _render_objects.size();
}

void Renderer::render(SDL_Window* window) {
    glClearColor(0.6, 0.6, 0.6, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (std::map<GLuint, Object>::iterator iter = _render_objects.begin(); iter != _render_objects.end(); ++iter) {
        Object& obj = iter->second;
        obj.render();
    }
    SDL_GL_SwapWindow(window);
}

void Renderer::update_view_uniforms(std::string view_uni_name, glm::mat4& view) {
    for (std::map<GLuint, Object>::iterator iter = _render_objects.begin(); iter != _render_objects.end(); ++iter) {
        iter->second.set_view_matrix_uniform(view_uni_name, view);
    }
}