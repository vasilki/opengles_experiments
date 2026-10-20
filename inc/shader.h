#ifndef SHADER_H
#define SHADER_H
#include <SDL_opengles2.h>


typedef struct
{
    GLbyte path[256];
    GLbyte description[256];
}T_SHADER_SOURCE;

typedef struct T_SHADER T_SHADER;

typedef struct T_SHADER
{
    GLbyte *path;
    GLbyte *description;
    GLuint shaderID;
    T_SHADER *next;
    T_SHADER *prev;
    T_SHADER *head;
}T_SHADER;

typedef struct T_SHADER_PROGRAM T_SHADER_PROGRAM;

typedef struct T_SHADER_PROGRAM
{
    T_SHADER *vertexShader;
    T_SHADER *fragmentShader;
    GLuint shaderProgramID;
    T_SHADER_PROGRAM *next;
    T_SHADER_PROGRAM *prev;
    T_SHADER_PROGRAM *head;
}T_SHADER_PROGRAM;

extern T_SHADER_PROGRAM *g_shader_program;

extern void createProgramsAndCompileShaders(T_SHADER_PROGRAM **par_shader_program);


#endif /* SHADER_H */

