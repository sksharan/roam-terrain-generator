#ifndef TG_GLOBALS_H
#define TG_GLOBALS_H

#include "Camera.h"
#include "LandscapeManager.h"
#include "Renderer.h"
#include "ROAMTerrain.h"
#include "Object.h"

/* Global variables. */
struct Globals {
    /* Global camera. */
    static Camera camera;

    /* Global LandscapeManager. */
    static LandscapeManager landscape_manager;

    /* True while the program should continue to run. */
    static bool program_running;

    /* Controls sensitivity of mouse movement. */
    static float mouse_sensitivity;

    /* Controls how fast the user can move around the scene. */
    static float movement_speed;

    /* Renderer used in the project. */
    static Renderer renderer;
};

#endif