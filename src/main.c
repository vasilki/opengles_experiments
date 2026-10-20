#define SDL_MAIN_HANDLED /*меняем точку входа с SDL_main на наш main*/

#include <SDL.h>
#include <SDL_opengles2.h>
#include <windows.h>
#include <stdio.h>
#include "types.h"
#include "gles2_loader.h"
#include "init.h"
#include "draw.h"

T_CONTEXT g_context;

int main(int argc, char *argv[]) {
    GLuint loc_error = 0;
    unsigned char loc_runing = 1;
    SDL_Event loc_event;

    loc_error = init(&g_context);

    if (loc_error != 0) {
        printf("Initialization failed with error code: %d\n", loc_error);
        return loc_error;
    }

    if (p_glGetString != NULL) {
        printf("GL_VERSION: %s\n", p_glGetString(GL_VERSION));
        printf("GL_RENDERER: %s\n", p_glGetString(GL_RENDERER));
        printf("GLSL: %s\n", p_glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    while (loc_runing) {
        while (SDL_PollEvent(&loc_event)) {
            if (loc_event.type == SDL_QUIT)
                loc_runing = 0;
        }
        draw(&g_context);
        SDL_GL_SwapWindow(g_context.window);
    }

    SDL_GL_DeleteContext(g_context.context);
    SDL_DestroyWindow(g_context.window);
    SDL_Quit();

    return 0;
}

