#ifndef TYPES_H
#define TYPES_H
#include "shader.h"
#include <SDL.h>

typedef struct
{
    SDL_Window *window;
    SDL_GLContext context;
    T_SHADER_PROGRAM *shaderPrograms;
    int32_t windowWidth;
    int32_t windowHeight;
    unsigned int shaderProgramsNumber;
}T_CONTEXT;

#endif /* TYPES_H */


