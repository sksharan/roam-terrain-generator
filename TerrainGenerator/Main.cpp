#include "Camera.h"
#include "Configuration.h"
#include "GrassPatch.h"
#include "KeyHandler.h"
#include "LandscapeManager.h"
#include "MouseHandler.h"
#include "Renderer.h"
#include "ROAMTerrain.h"
#include "Utils.h"
#include <GL/glew.h>
#include <iostream>
#include <SDL.h>
#include <thread>

const char* window_title = "TerrainGenerator";
const int window_x = SDL_WINDOWPOS_UNDEFINED;
const int window_y = SDL_WINDOWPOS_UNDEFINED;
const int window_width = 1080;
const int window_height = 720;
const Uint32 window_flags = SDL_WINDOW_OPENGL;

int main(int argc, char** argv) {
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init() failed: %s", SDL_GetError());
        return -1;
    }
    /* Create a window with SDL */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window* window = SDL_CreateWindow(window_title, window_x, window_y,
                                          window_width, window_height, window_flags);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow() failed: %s", SDL_GetError());
        return -2;
    }
    /* Associate an OpenGL context with the window */
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        fprintf(stderr, "SDL_GL_CreateContext() failed: %s", SDL_GetError());
        return -3;
    }
    /* Initialize GLEW */
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(status));
        return -4;
    }
    glGetError(); //toss error reported by GLEW for invalid enum

    std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    /* Enable depth buffering */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Enable blending. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Initialize mouse position and hide the cursor */
    SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);
    SDL_ShowCursor(0);

    Camera camera;
    Configuration configuration;
    Renderer renderer;

    /* Call init methods for classes that require them. */
    ROAMTerrain::init(configuration);
    GrassPatch::init(configuration);

    LandscapeManager landscape_manager(camera, configuration, renderer);
    landscape_manager.create_landscape();

    /* Since landscape updates are CPU-intensive, handle them in a separate thread */
    std::thread update_thread([&landscape_manager, &configuration]() {
        while (configuration.program_running) {
            landscape_manager.update_landscape();
        }
    });

    SDL_Event event;
    while (configuration.program_running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                KeyHandler::handlekey(event.key.keysym.sym, configuration);
                break;
            default:
                continue;
            }
        }
        MouseHandler::handlemouse(window, window_width, window_height, configuration, camera, renderer);
        KeyHandler::handlekey_cont(configuration, camera);
        landscape_manager.update_in_render_list();
        renderer.render(window);
    }

    update_thread.join();

    /* Clean up and return */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    return 0;
}