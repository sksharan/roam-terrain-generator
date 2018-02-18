#ifndef TG_MOUSEHANDLER_H
#define TG_MOUSEHANDLER_H

#include <SDL.h>

/* Mouse handler */
class MouseHandler {
public:
    /* Handles movement of the mouse. */
    static void handlemouse(SDL_Window* window, int window_width, int window_height);
};

#endif