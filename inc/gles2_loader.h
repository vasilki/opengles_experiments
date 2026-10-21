
#ifndef GLES2_LOADER_H
#define GLES2_LOADER_H

#include <SDL.h>
#include <SDL_opengles2.h>

/* Функции компиляции шейдеров */
extern PFNGLCREATESHADERPROC       p_glCreateShader;
extern PFNGLSHADERSOURCEPROC       p_glShaderSource;
extern PFNGLCOMPILESHADERPROC      p_glCompileShader;
extern PFNGLGETSHADERIVPROC        p_glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC   p_glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC       p_glDeleteShader;

/* Функции работы с программой */
extern PFNGLCREATEPROGRAMPROC      p_glCreateProgram;
extern PFNGLATTACHSHADERPROC       p_glAttachShader;
extern PFNGLLINKPROGRAMPROC        p_glLinkProgram;
extern PFNGLGETPROGRAMIVPROC       p_glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC  p_glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC         p_glUseProgram;
extern PFNGLDELETEPROGRAMPROC      p_glDeleteProgram;

/* Атрибуты вершин */
extern PFNGLGETATTRIBLOCATIONPROC  p_glGetAttribLocation;
extern PFNGLGETUNIFORMLOCATIONPROC  p_glGetUniformLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC p_glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC p_glEnableVertexAttribArray;
extern PFNGLUNIFORM2IPROC          p_glUniform2i;
extern PFNGLUNIFORM2FPROC          p_glUniform2f;
extern PFNGLUNIFORM3FPROC          p_glUniform3f;
extern PFNGLUNIFORMMATRIX3FVPROC          p_glUniformMatrix3fv;

/* Рисование */
extern PFNGLVIEWPORTPROC           p_glViewport;
extern PFNGLCLEARCOLORPROC         p_glClearColor;
extern PFNGLCLEARPROC              p_glClear;
extern PFNGLDRAWARRAYSPROC         p_glDrawArrays;
extern PFNGLDRAWELEMENTSPROC         p_glDrawElements;

/* Сервисы */
extern PFNGLGETSTRINGPROC          p_glGetString;
extern PFNGLGETERRORPROC           p_glGetError;

/* Инициализация загрузчика */
int gles2_load_functions(void);

#endif /* GLES2_LOADER_H */

