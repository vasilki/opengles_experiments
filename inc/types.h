#ifndef TYPES_H
#define TYPES_H
#include "shader.h"

typedef struct
{
    SDL_Window *window;
    SDL_GLContext context;
    T_SHADER_PROGRAM *shaderProgram;
}T_CONTEXT;

#endif /* TYPES_H */


