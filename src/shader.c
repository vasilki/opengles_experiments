#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "gles2_loader.h"

/*
#define VERTEXSHADERSOURCE "..//src//shaders//vertex_base.glsl"
#define FRAGMENTSHADERSOURCE "..//src//shaders//fragment_base.glsl"
*/

static T_SHADER_SOURCE g_vertex_shader_sources[] = {
    {"..//..//..//src//shaders//vertex_base.glsl", "Vertex Shader"},
};

static T_SHADER_SOURCE g_fragment_shader_sources[] = {
    {"..//..//..//src//shaders//fragment_base.glsl", "Fragment Shader"}
};

T_SHADER_PROGRAM *gl_shader_program = NULL;

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

static void createAndCompileShaders(T_SHADER *par_shader_head, GLenum par_shaderType)
{
    char loc_infoLog[512];
    char loc_vertex[4096];
    unsigned int loc_vertex_length;
    GLint loc_success = 0;
    T_SHADER *loc_shader;
    const char *loc_shader_source = (char*)&loc_vertex[0];

    loc_shader = par_shader_head;
    while(loc_shader != NULL)
    {
        loc_shader->shaderID = p_glCreateShader(par_shaderType);
        loc_vertex_length = readShaderFromFile(loc_shader->path, &loc_vertex[0]);
        p_glShaderSource(loc_shader->shaderID, 1, &loc_shader_source, NULL);
        p_glCompileShader(loc_shader->shaderID);
        p_glGetShaderiv(loc_shader->shaderID, GL_COMPILE_STATUS, &loc_success);  
        if(!loc_success)
        {
            p_glGetShaderInfoLog(loc_shader->shaderID, 512, NULL, loc_infoLog);
            printf("Vertex Shader Error: %s\n", loc_infoLog);
        }
        loc_shader = loc_shader->next;
    }

    return;
}

void createProgramsAndCompileShaders(T_SHADER_PROGRAM *par_shader_program)
{
    unsigned int loc_i = 0;
    char loc_infoLog[512];
    GLint loc_success = 0;
    T_SHADER *loc_shader;
    T_SHADER *loc_shader_prev;
    par_shader_program = (T_SHADER_PROGRAM *)malloc(sizeof(T_SHADER_PROGRAM));
    memset(par_shader_program, 0, sizeof(T_SHADER_PROGRAM));
    
    for(loc_i = 0; loc_i < sizeof(g_vertex_shader_sources)/sizeof(T_SHADER_SOURCE); loc_i++)
    {
        loc_shader = (T_SHADER *)malloc(sizeof(T_SHADER));
        memset(loc_shader, 0, sizeof(T_SHADER));
        loc_shader->path = g_vertex_shader_sources[loc_i].path;
        loc_shader->description = g_vertex_shader_sources[loc_i].description;
        if(loc_i == 0)
        {
            par_shader_program->vertexShader = loc_shader;
            loc_shader->head = loc_shader;
            loc_shader->prev = NULL;
        }
        else
        {
            loc_shader->prev = loc_shader_prev;
            loc_shader_prev->next = loc_shader;
            loc_shader->head = loc_shader_prev->head;
        }
    }

    for(loc_i = 0; loc_i < sizeof(g_fragment_shader_sources)/sizeof(T_SHADER_SOURCE); loc_i++)
    {
        loc_shader = (T_SHADER *)malloc(sizeof(T_SHADER));
        memset(loc_shader, 0, sizeof(T_SHADER));
        loc_shader->path = g_fragment_shader_sources[loc_i].path;
        loc_shader->description = g_fragment_shader_sources[loc_i].description;
        if(loc_i == 0)
        {
            par_shader_program->fragmentShader = loc_shader;
            loc_shader->head = loc_shader;
            loc_shader->prev = NULL;
        }
        else
        {
            loc_shader->prev = loc_shader_prev;
            loc_shader_prev->next = loc_shader;
            loc_shader->head = loc_shader_prev->head;
        }
    }

    createAndCompileShaders(par_shader_program->vertexShader, GL_VERTEX_SHADER);
    createAndCompileShaders(par_shader_program->fragmentShader, GL_FRAGMENT_SHADER);


    // Программа шейдера (пока сделана одна)
    par_shader_program->shaderProgramID = p_glCreateProgram();
    par_shader_program->head = par_shader_program;
    par_shader_program->next = NULL;
    par_shader_program->prev = NULL;

    loc_shader = par_shader_program->vertexShader;
    while(loc_shader != NULL)
    {
        if(loc_shader->shaderID != 0)
        {
            p_glAttachShader(par_shader_program->shaderProgramID, loc_shader->shaderID);
        }
        loc_shader = loc_shader->next;
    }

    loc_shader = par_shader_program->fragmentShader;
    while(loc_shader != NULL)
    {
        if(loc_shader->shaderID != 0)
        {
            p_glAttachShader(par_shader_program->shaderProgramID, loc_shader->shaderID);
        }
        loc_shader = loc_shader->next;
    }

    p_glLinkProgram(par_shader_program->shaderProgramID);
    p_glGetProgramiv(par_shader_program->shaderProgramID, GL_LINK_STATUS, &loc_success);
    if(!loc_success)
    {
        p_glGetProgramInfoLog(par_shader_program->shaderProgramID, 512, NULL, loc_infoLog);
        printf("Shader Program Linking Error: %s\n", loc_infoLog);
    }

    /*
    loc_shader = par_shader_program->vertexShader;
    while(loc_shader != NULL)
    {
        if(loc_shader->shaderID != 0)
        {
            p_glDeleteShader(loc_shader->shaderID);
        }
        loc_shader = loc_shader->next;
    }

    loc_shader = par_shader_program->fragmentShader;
    while(loc_shader != NULL)
    {
        if(loc_shader->shaderID != 0)
        {
            p_glDeleteShader(loc_shader->shaderID);
        }
        loc_shader = loc_shader->next;
    }    
    */

    return;
}

