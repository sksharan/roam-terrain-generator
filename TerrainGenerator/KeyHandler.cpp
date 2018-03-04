#include "KeyHandler.h"
#include "Configuration.h"
#include "Utils.h"
#include <glm/glm.hpp>

bool _walk_mode = true; //True if user walks on landscape
float _walk_height = 45.0f; //If '_walk_mode' is true, determines how high off landscape the user is
bool _wireframe_enabled = false; //True if wireframe enabled
const Uint8* _keystate = SDL_GetKeyboardState(NULL);

void KeyHandler::handlekey(const SDL_Keycode& key, Configuration& configuration) {
    switch (key) {

    /* End program */
    case SDLK_ESCAPE:
        configuration.program_running = false;
        break;
    /* Toggle walk mode on/off */
    case SDLK_e:
        _walk_mode = !_walk_mode;
        break;
    /* Toggle wireframe mode on/off */
    case SDLK_r:
        _wireframe_enabled = !_wireframe_enabled;
        if (_wireframe_enabled) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        break;
    default:
        break;
    }
}

void KeyHandler::handlekey_cont(Configuration& configuration, Camera& camera) {
    glm::vec3 pos = camera.get_pos();

    /* Movement along direction vector of camera */
    if (_keystate[SDL_SCANCODE_W]) {
        camera.set_pos(pos + configuration.movement_speed * camera.get_dir());
    }
    if (_keystate[SDL_SCANCODE_S]) {
        camera.set_pos(pos - configuration.movement_speed * camera.get_dir());
    }
    /* Movement along right vector of camera */
    if (_keystate[SDL_SCANCODE_A]) {
        camera.set_pos(pos - configuration.movement_speed *  glm::normalize(glm::cross(camera.get_dir(), camera.get_up())));
    }
    if (_keystate[SDL_SCANCODE_D]) {
        camera.set_pos(pos + configuration.movement_speed * glm::normalize(glm::cross(camera.get_dir(), camera.get_up())));
    }
    /* Movement along up vector of camera */
    if (!_walk_mode && _keystate[SDL_SCANCODE_SPACE]) {
        camera.set_pos(pos + configuration.movement_speed * camera.get_up());
    }
    if (!_walk_mode && _keystate[SDL_SCANCODE_LSHIFT]) {
        camera.set_pos(pos - configuration.movement_speed * camera.get_up());
    }

    /* Adjust user position if they are walking on landscape. */
    if (_walk_mode) {
        pos = camera.get_pos(); //update position if changed above
        camera.set_pos(glm::vec3(pos.x, Utils::get_value(pos.x, 0, pos.z, configuration) + _walk_height, pos.z));
    }
}