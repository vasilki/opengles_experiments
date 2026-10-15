#define SDL_MAIN_HANDLED /*меняем точку входа с SDL_main на наш main*/

#include <SDL.h>
#include <SDL_opengles2.h>
#include <windows.h>
#include "stdio.h"
#include "gles2_loader.h"
#include "types.h"


int init(T_CONTEXT *par_context) {
    GLuint loc_SDL_error = 0;
    unsigned char loc_runing = 1;
    SDL_Event loc_event;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    loc_SDL_error = SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_ES
    );
    if (loc_SDL_error != 0) {
        printf("SDL_GL_SetAttribute: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    loc_SDL_error = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    if (loc_SDL_error != 0) {
        printf("SDL_GL_SetAttribute: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    loc_SDL_error = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    if (loc_SDL_error != 0) {
        printf("SDL_GL_SetAttribute: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* loc_window = SDL_CreateWindow(
        "OpenGL ES 2.0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!loc_window) {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext loc_context = SDL_GL_CreateContext(loc_window);

    if (!loc_context) {
        printf("SDL_GL_CreateContext: %s\n", SDL_GetError());
        SDL_DestroyWindow(loc_window);
        SDL_Quit();
        return 1;
    }

    if (gles2_load_functions() != 1) {
        printf("OpenGL ES function loading failed\n");
        SDL_GL_DeleteContext(loc_context);
        SDL_DestroyWindow(loc_window);
        SDL_Quit();
        return 1;
    }

    par_context->window = loc_window;
    par_context->context = loc_context;

    return 0;
}

