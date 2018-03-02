#ifndef TG_KEYHANDLER_H
#define TG_KEYHANDLER_H

#include "Camera.h"
#include "Configuration.h"
#include <SDL.h>

class KeyHandler {
public:
    /* Handles a single press of the 'key'. */
    static void handlekey(const SDL_Keycode& key, Configuration& configuration);
    /* Handles continuous keyboard input (needed for smooth movement). */
    static void handlekey_cont(const Configuration& configuration, Camera& camera);
};

#endif