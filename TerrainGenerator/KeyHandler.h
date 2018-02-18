#ifndef TG_KEYHANDLER_H
#define TG_KEYHANDLER_H

#include <SDL.h>

/* Keyboard handler. */
class KeyHandler {
public:
    /* Handles a single press of the 'key'. */
    static void handlekey(SDL_Keycode& key);
    /* Handles continuous keyboard input. (Needed for smooth movement.) */
    static void handlekey_cont();
};

#endif