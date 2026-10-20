#include <stdio.h>
#include <stdint.h>
#include "types.h"
#include "gles2_loader.h"
#include <SDL.h>
#include <math.h>


static T_CONTEXT gg_context;
static uint32_t gg_width = 2;


void ggInit(T_CONTEXT *par_context) 
{
    gg_context = *par_context;
    
    return;
}

void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
    float_t loc_vertices[12] = {0.0f};
    int32_t loc_screensize[3] = {0.0f};    
    uint16_t loc_indicies[6] = {0,1,2,2,1,3};

    // Вычисляем вектор направления отрезка.
    float_t loc_dx = par_x2 - par_x1;
    float_t loc_dy = par_y2 - par_y1;

    float_t loc_length = sqrtf(loc_dx * loc_dx + loc_dy * loc_dy);

    if (loc_length < 1e-6f || gg_width <= 0.0f)
        return ;

    // Единичный перпендикуляр к отрезку.
    float_t loc_nx = -loc_dy / loc_length;
    float_t loc_ny =  loc_dx / loc_length;

    // Смещение на половину толщины.
    float_t loc_ox = loc_nx * gg_width * 0.5f;
    float_t loc_oy = loc_ny * gg_width * 0.5f;

    // Порядок вершин для GL_TRIANGLE_STRIP.
    loc_vertices[0] = par_x1 - loc_ox;  // P0
    loc_vertices[1] = par_y1 - loc_oy;
 
    loc_vertices[3] = par_x1 + loc_ox;  // P1
    loc_vertices[4] = par_y1 + loc_oy;

    loc_vertices[6] = par_x2 - loc_ox;  // P2
    loc_vertices[7] = par_y2 - loc_oy;

    loc_vertices[9] = par_x2 + loc_ox;  // P3
    loc_vertices[10] = par_y2 + loc_oy;

    GLint loc_position;

    SDL_GetWindowSize(gg_context.window, &loc_screensize[0], &loc_screensize[1]);

    p_glViewport(0, 0, loc_screensize[0], loc_screensize[1]);

    p_glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    p_glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(gg_context.shaderPrograms[0].shaderProgramID);

    loc_position = p_glGetUniformLocation(gg_context.shaderPrograms[0].shaderProgramID, "screensize");

    if (loc_position < 0) {
        printf("Uniform 'screensize' not found\n");
        return;
    }

    p_glUniform2f((GLuint)loc_position, (GLfloat)loc_screensize[0], (GLfloat)loc_screensize[1]);

    loc_position = p_glGetAttribLocation(gg_context.shaderPrograms[0].shaderProgramID, "position");

    if (loc_position < 0) {
        printf("Attribute 'position' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);

    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        3,                   // три компонента: x, y, z
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        0,                   // шаг между вершинами
        loc_vertices             // адрес данных
    );    


    p_glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, loc_indicies);
 

    return ;
}



