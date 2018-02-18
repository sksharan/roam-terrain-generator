#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

/* Class representing a camera in the scene. */
class Camera {
public:
    /* Initialize a Camera at the origin facing in the -z direction with up vector +y . */
    Camera();
    /* Returns camera position. */
    glm::vec3 get_pos();
    /* Returns camera direction. */
    glm::vec3 get_dir();
    /* Returns camera up vector. */
    glm::vec3 get_up();
    /* Set camera position. */
    void set_pos(glm::vec3& position);
    /* Set camera direction. */
    void set_dir(glm::vec3& direction);
    /* Set camera up vector. */
    void set_up(glm::vec3& up);
private:
    glm::vec3 _position;
    glm::vec3 _direction;
    glm::vec3 _up;
};

#endif