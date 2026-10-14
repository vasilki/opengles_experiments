#define SDL_MAIN_HANDLED /*меняем точку входа с SDL_main на наш main*/

#include <SDL.h>
#include <SDL_opengles2.h>
#include <windows.h>
#include "stdio.h"

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_ES
    );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

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
/*
    // Загружаем функцию через SDL, чтобы не зависеть
    // от непосредственного линкования с библиотекой GLES.
    using GetStringFunc = const GLubyte* (GL_APIENTRYP)(GLenum);

    auto getString = reinterpret_cast<GetStringFunc>(
        SDL_GL_GetProcAddress("glGetString")
    );
*/
    // Определяем тип указателя согласно сигнатуре функции 
    typedef const GLubyte *(*glGetStringType)(GLenum name);
    // Объявляем переменную для указателя 
    glGetStringType glGetString = (glGetStringType)SDL_GL_GetProcAddress("glGetString");

    if (glGetString != NULL) {
        printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
        printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
        printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    unsigned char loc_runing = 1;
    SDL_Event loc_event;

    while (loc_runing) {
        while (SDL_PollEvent(&loc_event)) {
            if (loc_event.type == SDL_QUIT)
                loc_runing = 0;
        }

        SDL_GL_SwapWindow(loc_window);
    }

    SDL_GL_DeleteContext(loc_context);
    SDL_DestroyWindow(loc_window);
    SDL_Quit();

    return 0;
}

