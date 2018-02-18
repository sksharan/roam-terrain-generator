#include "Camera.h"

Camera::Camera() {
    _position = glm::vec3(0.0, 0.0, 0.0);
    _direction = glm::vec3(0.0, 0.0, -1.0);
    _up = glm::vec3(0.0, 1.0, 0.0);
}

glm::vec3 Camera::get_pos() {
    return _position;
}

glm::vec3 Camera::get_dir() {
    return _direction;
}

glm::vec3 Camera::get_up() {
    return _up;
}

void Camera::set_pos(glm::vec3& position) {
    _position = position;
}

void Camera::set_dir(glm::vec3& direction) {
    _direction = direction;
}

void Camera::set_up(glm::vec3& up) {
    _up = up;
}
