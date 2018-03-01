#include "Globals.h"
#include "GrassPatch.h"
#include "KeyHandler.h"
#include "MouseHandler.h"
#include "Renderer.h"
#include "ROAMTerrain.h"
#include "Utils.h"
#include <GL/glew.h>
#include <iostream>
#include <omp.h>
#include <SDL.h>

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

    /* Allow nested parallelism with OpenMP. */
    omp_set_nested(1);

    /* Call init methods for classes that require them. */
    ROAMTerrain::init();
    GrassPatch::init();

    /* Create objects and add to render list. */
    Globals::landscape_manager.create_landscape();
    Globals::landscape_manager.update_in_render_list(); //add landscape to render list

    /* Initialize mouse position and hide the cursor */
    SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);
    SDL_ShowCursor(0);
    
    /* Main loop. One thread handles the main loop logic while the other thread focuses on
    updating the landscape. */
    SDL_Event event;
    #pragma omp parallel num_threads(2)
    {
        while (Globals::program_running) {
            /* One thread focuses on updating landscape only. */
            if (omp_get_thread_num() > 0) {
                Globals::landscape_manager.update_landscape();
            }
            /* The other thread handles the rest of the main loop logic. */
            else {
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                    case SDL_KEYDOWN:
                        KeyHandler::handlekey(event.key.keysym.sym);
                        break;
                    default:
                        continue;
                    }
                }
                MouseHandler::handlemouse(window, window_width, window_height);
                KeyHandler::handlekey_cont();
                Globals::landscape_manager.update_in_render_list(); //update landscape in render list
                Globals::renderer.render(window);
            }
        }
    }

    /* Clean up and return */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    return 0;
}