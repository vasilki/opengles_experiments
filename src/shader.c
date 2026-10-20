#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "gles2_loader.h"

static const T_SHADER_SOURCE g_basic_shader_sources[] = {
    {"..//..//..//src//shaders//vertex_base.glsl", "Vertex Shader", GL_VERTEX_SHADER},
    {"..//..//..//src//shaders//fragment_base.glsl", "Fragment Shader", GL_FRAGMENT_SHADER}
};

T_SHADER_PROGRAM g_shader_programs[] = {
    {0,g_basic_shader_sources, "Basic program", sizeof(g_basic_shader_sources)/sizeof(g_basic_shader_sources[0])}
};
GLuint g_shader_progrmams_number = sizeof(g_shader_programs)/sizeof(g_shader_programs[0]);

static unsigned int readShaderFromFile(const char *par_filename, void *par_out)
{
    FILE *loc_fp;
    unsigned int loc_length = 0;
    unsigned char *loc_p = (unsigned char *)par_out;

    loc_fp = fopen(par_filename,"r");

    if(loc_fp != NULL)
    {
        fseek(loc_fp, 0, SEEK_END);
        loc_length = ftell(loc_fp);
        rewind(loc_fp);

        fread(loc_p, 1, loc_length, loc_fp);
        loc_p[loc_length] = '\0';
        fclose(loc_fp);
    }
    else
    {
        /*nothing to do*/
    }
    return loc_length;
}       

static GLuint createAndCompileShaders(const T_SHADER_SOURCE *par_shader)
{
    char loc_infoLog[512];
    char loc_vertex[4096];
    GLuint loc_vertex_length;
    GLuint loc_shader_ID;
    GLint loc_success = 0;
    const char *loc_shader_source = (char*)&loc_vertex[0];

    loc_shader_ID = p_glCreateShader(par_shader->type);
    loc_vertex_length = readShaderFromFile(par_shader->path, &loc_vertex[0]);
    p_glShaderSource(loc_shader_ID, 1, &loc_shader_source, NULL);
    p_glCompileShader(loc_shader_ID);
    p_glGetShaderiv(loc_shader_ID, GL_COMPILE_STATUS, &loc_success);  
    if(!loc_success)
    {
        p_glGetShaderInfoLog(loc_shader_ID, 512, NULL, loc_infoLog);
        printf("Vertex Shader Error: %s\n", loc_infoLog);
        loc_shader_ID = 0;
    }
    else
    {
        /*nothing to do*/
    }

    return loc_shader_ID;
}

void createProgramsAndCompileShaders(T_SHADER_PROGRAM *par_shader_program)
{
    unsigned int loc_i = 0;
    char loc_infoLog[512];
    GLint loc_success = 0;
    GLuint loc_shader_ID[K_MAX_SHADERS_PER_PROGRAM];
    GLuint loc_number;
    GLint loc_shaderProgramID;

    loc_shaderProgramID = p_glCreateProgram();
    loc_number = ((par_shader_program->shaders_number <= K_MAX_SHADERS_PER_PROGRAM)?par_shader_program->shaders_number:K_MAX_SHADERS_PER_PROGRAM);

    for(loc_i = 0; loc_i < loc_number; loc_i++)
    {
        loc_shader_ID[loc_i] = createAndCompileShaders(&par_shader_program->shaders[loc_i]);
        if(loc_shader_ID[loc_i] != 0)
        {
            p_glAttachShader(loc_shaderProgramID, loc_shader_ID[loc_i]);
        }
        else
        {
            /*nothing to do*/
        }
    }

    p_glLinkProgram(loc_shaderProgramID);
    p_glGetProgramiv(loc_shaderProgramID, GL_LINK_STATUS, &loc_success);
   
    if(!loc_success)
    {
        p_glGetProgramInfoLog(loc_shaderProgramID, 512, NULL, loc_infoLog);
        printf("Shader Program Linking Error: %s\n", loc_infoLog);
        loc_shaderProgramID = 0;
    }
    else
    {
        /*nothing to do*/
    }

    for(loc_i = 0; loc_i < loc_number; loc_i++)
    {
        if(loc_shader_ID[loc_i] != 0)
        {
            p_glDeleteShader(loc_shader_ID[loc_i]);
        }
        else
        {
            /*nothing to do*/
        }
    }

    par_shader_program->shaderProgramID = loc_shaderProgramID;

    return;
}

