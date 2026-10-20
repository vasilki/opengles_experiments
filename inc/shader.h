#ifndef SHADER_H
#define SHADER_H
#include <SDL_opengles2.h>


#define K_MAX_SHADERS_PER_PROGRAM 10

typedef struct
{
    const GLbyte *path;
    const GLbyte *description;
    GLenum type;
}T_SHADER_SOURCE;

typedef struct
{
    GLuint shaderProgramID;
    const T_SHADER_SOURCE *shaders;
    const GLbyte *name;
    GLuint shaders_number;
}T_SHADER_PROGRAM;

//extern T_SHADER_SOURCE g_basic_shader_sources[];
extern T_SHADER_PROGRAM g_shader_programs[];
extern GLuint g_shader_progrmams_number;


extern void createProgramsAndCompileShaders(T_SHADER_PROGRAM *par_shader_programs);


#endif /* SHADER_H */

