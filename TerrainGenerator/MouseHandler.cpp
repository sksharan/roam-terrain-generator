#include "MouseHandler.h"
#include "Globals.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

float _pitch = 0.0f;
float _yaw = -90.0f;

void MouseHandler::handlemouse(SDL_Window* window, int window_width, int window_height) {
    /* Get current mouse position. */
    int x, y;
    SDL_GetMouseState(&x, &y);
    /* Reset mouse postion to center of window. */
    SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);

    /* Calculate new yaw and pitch */
    float delta_x = x - (window_width / 2.0f);
    float delta_y = (window_height / 2.0f) - y;
    _yaw += (delta_x * Globals::mouse_sensitivity);
    _pitch += (delta_y * Globals::mouse_sensitivity);
    /* Clamp pitch to range [-89.5, 89.5] to prevent moving upside down */
    if (_pitch > 89.5f) _pitch = 89.5f;
    else if (_pitch < -89.5f) _pitch = -89.f;

    /* Update camera direction (up vector can stay parallel to world y-axis) */
    glm::vec3 direction = Globals::camera.get_dir();
    direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.y = sin(glm::radians(_pitch));
    direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction = glm::normalize(direction);
    Globals::camera.set_dir(direction);

    /* Calculate view matrix */
    glm::mat4 view = glm::lookAt(Globals::camera.get_pos(), Globals::camera.get_pos() + Globals::camera.get_dir(), Globals::camera.get_up());

    /* Set view matrix uniform for every object to be rendered */
    Globals::renderer.update_view_uniforms("view", view);
}