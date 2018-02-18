#include "KeyHandler.h"
#include "Globals.h"
#include "Utils.h"
#include <glm/glm.hpp>

bool _walk_mode = true; //True if user walks on landscape
float _walk_height = 45.0f; //If '_walk_mode' is true, determines how high off landscape the user is
bool _wireframe_enabled = false; //True if wireframe enabled
const Uint8* _keystate = SDL_GetKeyboardState(NULL);

void KeyHandler::handlekey(SDL_Keycode& key) {
    switch (key) {

    /* End program */
    case SDLK_ESCAPE:
        Globals::program_running = false;
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

void KeyHandler::handlekey_cont() {
    glm::vec3 pos = Globals::camera.get_pos();

    /* Movement along direction vector of camera */
    if (_keystate[SDL_SCANCODE_W]) {
        Globals::camera.set_pos(pos + Globals::movement_speed * Globals::camera.get_dir());
    }
    if (_keystate[SDL_SCANCODE_S]) {
        Globals::camera.set_pos(pos - Globals::movement_speed * Globals::camera.get_dir());
    }
    /* Movement along right vector of camera */
    if (_keystate[SDL_SCANCODE_A]) {
        Globals::camera.set_pos(pos - Globals::movement_speed *  glm::normalize(glm::cross(Globals::camera.get_dir(), Globals::camera.get_up())));
    }
    if (_keystate[SDL_SCANCODE_D]) {
        Globals::camera.set_pos(pos + Globals::movement_speed * glm::normalize(glm::cross(Globals::camera.get_dir(), Globals::camera.get_up())));
    }
    /* Movement along up vector of camera */
    if (!_walk_mode && _keystate[SDL_SCANCODE_SPACE]) {
        Globals::camera.set_pos(pos + Globals::movement_speed * Globals::camera.get_up());
    }
    if (!_walk_mode && _keystate[SDL_SCANCODE_LSHIFT]) {
        Globals::camera.set_pos(pos - Globals::movement_speed * Globals::camera.get_up());
    }

    /* Adjust user position if they are walking on landscape. */
    if (_walk_mode) {
        pos = Globals::camera.get_pos(); //update position if changed above
        Globals::camera.set_pos(glm::vec3(pos.x, Utils::get_value(pos.x, 0, pos.z) + _walk_height, pos.z));
    }
}