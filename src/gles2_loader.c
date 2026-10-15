#include "gles2_loader.h"
#include <stdio.h>

PFNGLCREATESHADERPROC        p_glCreateShader = NULL;
PFNGLSHADERSOURCEPROC        p_glShaderSource = NULL;
PFNGLCOMPILESHADERPROC       p_glCompileShader = NULL;
PFNGLGETSHADERIVPROC         p_glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC    p_glGetShaderInfoLog = NULL;
PFNGLDELETESHADERPROC        p_glDeleteShader = NULL;

PFNGLCREATEPROGRAMPROC       p_glCreateProgram = NULL;
PFNGLATTACHSHADERPROC        p_glAttachShader = NULL;
PFNGLLINKPROGRAMPROC         p_glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC        p_glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC   p_glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC          p_glUseProgram = NULL;
PFNGLDELETEPROGRAMPROC       p_glDeleteProgram = NULL;

PFNGLGETATTRIBLOCATIONPROC   p_glGetAttribLocation = NULL;
PFNGLVERTEXATTRIBPOINTERPROC p_glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC p_glEnableVertexAttribArray = NULL;

PFNGLVIEWPORTPROC            p_glViewport = NULL;
PFNGLCLEARCOLORPROC          p_glClearColor = NULL;
PFNGLCLEARPROC               p_glClear = NULL;
PFNGLDRAWARRAYSPROC          p_glDrawArrays = NULL;

PFNGLGETSTRINGPROC          p_glGetString = NULL;
PFNGLGETERRORPROC           p_glGetError = NULL;


int gles2_load_functions(void)
{
#define LOAD_GL(variable, function)                                  \
    do {                                                             \
        variable = ( __typeof__(variable) )                          \
            SDL_GL_GetProcAddress(#function);                        \
        if (variable == NULL) {                                      \
            fprintf(stderr, "Failed to load %s\n", #function);       \
            return 0;                                                \
        }                                                            \
    } while (0)

    LOAD_GL(p_glCreateShader, glCreateShader);
    LOAD_GL(p_glShaderSource, glShaderSource);
    LOAD_GL(p_glCompileShader, glCompileShader);
    LOAD_GL(p_glGetShaderiv, glGetShaderiv);
    LOAD_GL(p_glGetShaderInfoLog, glGetShaderInfoLog);
    LOAD_GL(p_glDeleteShader, glDeleteShader);

    LOAD_GL(p_glCreateProgram, glCreateProgram);
    LOAD_GL(p_glAttachShader, glAttachShader);
    LOAD_GL(p_glLinkProgram, glLinkProgram);
    LOAD_GL(p_glGetProgramiv, glGetProgramiv);
    LOAD_GL(p_glGetProgramInfoLog, glGetProgramInfoLog);
    LOAD_GL(p_glUseProgram, glUseProgram);
    LOAD_GL(p_glDeleteProgram, glDeleteProgram);

    LOAD_GL(p_glGetAttribLocation, glGetAttribLocation);
    LOAD_GL(p_glVertexAttribPointer, glVertexAttribPointer);
    LOAD_GL(p_glEnableVertexAttribArray, glEnableVertexAttribArray);

    LOAD_GL(p_glViewport, glViewport);
    LOAD_GL(p_glClearColor, glClearColor);
    LOAD_GL(p_glClear, glClear);
    LOAD_GL(p_glDrawArrays, glDrawArrays);

    LOAD_GL(p_glGetString, glGetString);
    LOAD_GL(p_glGetError, glGetError);

#undef LOAD_GL

    return 1;
}

