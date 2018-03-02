#ifndef TG_MOUSEHANDLER_H
#define TG_MOUSEHANDLER_H

#include "Camera.h"
#include "Configuration.h"
#include "Renderer.h"
#include <SDL.h>

class MouseHandler {
public:
    static void handlemouse(SDL_Window* window, int window_width, int window_height,
        Configuration& configuration, Camera& camera, Renderer& renderer);
};

#endif