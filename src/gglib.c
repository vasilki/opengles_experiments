#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "types.h"
#include "gles2_loader.h"
#include <SDL.h>
#include <math.h>
#include "matrix.h"

typedef struct
{
    T_CONTEXT global_context;
    float_t current_matrix[4]; //текущая матрица масштабирования, вращения
    float_t translate_vector[2]; //вектор перемещения
    float_t NDC_matrix[9]; //матрица преобразования координат в нормализованные координаты устройства
    float_t translate_vector[2];
}T_GGLIB_CONTEXT;



static T_GGLIB_CONTEXT gg_context;
static uint32_t gg_width = 2;

/*
 M — Model matrix — матрица модели: масштабирование, вращение и перемещение объекта.
 S — Scale: масштабирование.
 R — Rotation: вращение.
 T — Translation: перемещение.
 M = T*R*S

 V — View matrix — матрица вида (камера): положение и ориентация камеры. Не используется
 P — Projection matrix — матрица проекции, в нашем случае перевод координат в NDC.
 MVP — итоговая комбинированная матрица преобразования (combined transformation matrix, иногда MVP matrix) с учетом NDC.

 MVP = P*V*M

 вектор вершины умножается на матрицы справа: pNDC = MVP * p - координаты вершины в NDC; 
        p' = M * p - координаты вершины после масштабирования, вращения и перемещения.

 */


void ggInit(T_CONTEXT *par_context) 
{
    //Начало в центре окна, направление оси Y вверх
    float_t loc_current_matrix[4] =
    {
        1, 0,
        0, 1
    };

    float_t loc_scale_matrix[4] =
    {
        0, 0,
        0, 0
    };
    
    float_t loc_rotate_matrix[4] =
    {
        0, 0,
        0, 0
    };

    ggScaleMatrix2(loc_current_matrix, loc_scale_matrix, 1, 1);

    ggRotateMatrix2(loc_scale_matrix, loc_current_matrix, 90.0f);

    float_t loc_translate_vector[2] =
    {
        par_context->windowWidth / 2.0f, par_context->windowHeight / 2.0f
    };


    float_t loc_NDC_matrix[9] =
    {0};    

    ggGetNDCCenterMatrix3(par_context->windowWidth, par_context->windowHeight, loc_NDC_matrix);

    gg_context.global_context = *par_context;
    
    memcpy(gg_context.current_matrix, loc_current_matrix, sizeof(gg_context.loc_current_matrix));
    memcpy(gg_context.translate_vector, loc_translate_vector, sizeof(gg_context.loc_translate_vector));
    memcpy(gg_context.NDC_matrix, loc_NDC_matrix, sizeof(gg_context.NDC_matrix));

    return;
}

void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
typedef struct 
{
    float_t position[2];
    float_t color[4];
}T_VERTEX;

    T_VERTEX loc_vertex[4];
    uint16_t loc_indicies[6] = {0,1,2,2,1,3};
    GLint loc_position;

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
    loc_vertex[0].position[0] = par_x1 - loc_ox;  // P0
    loc_vertex[0].position[1] = par_y1 - loc_oy;
 
    loc_vertex[1].position[0] = par_x1 + loc_ox;  // P1
    loc_vertex[1].position[1] = par_y1 + loc_oy;

    loc_vertex[2].position[0] = par_x2 - loc_ox;  // P2
    loc_vertex[2].position[1] = par_y2 - loc_oy;

    loc_vertex[3].position[0] = par_x2 + loc_ox;  // P3
    loc_vertex[3].position[1] = par_y2 + loc_oy;

    for(int32_t i = 0; i < 4; i ++)
    {
        loc_vertex[i].position[0]
    }
    loc_vertex[0].position[0] = 

    memcpy(loc_vertex[0].transform, &gg_context.transform_matrix[0], sizeof(loc_vertex[0].transform));
    memcpy(loc_vertex[1].transform, &gg_context.transform_matrix[0], sizeof(loc_vertex[1].transform));
    memcpy(loc_vertex[2].transform, &gg_context.transform_matrix[0], sizeof(loc_vertex[2].transform));
    memcpy(loc_vertex[3].transform, &gg_context.transform_matrix[0], sizeof(loc_vertex[3].transform));

    loc_vertex[0].position[2] = 1.0f;
    loc_vertex[1].position[2] = 1.0f;
    loc_vertex[2].position[2] = 1.0f;
    loc_vertex[3].position[2] = 1.0f;

    SDL_GetWindowSize(gg_context.global_context.window, &loc_screensize[0], &loc_screensize[1]);

    p_glViewport(0, 0, loc_screensize[0], loc_screensize[1]);

    p_glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    p_glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(gg_context.global_context.shaderPrograms[0].shaderProgramID);
    
    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "position");

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
        sizeof(T_VERTEX),                   // шаг между вершинами
        &loc_vertex[0]             // адрес данных
    );    


    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "transform0");

    if (loc_position < 0) {
        printf("Attribute 'transform0' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        3,                   // три компонента: x, y, z
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&loc_vertex[0] + offsetof(T_VERTEX, transform))             // адрес данных
    );    

    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "transform1");

    if (loc_position < 0) {
        printf("Attribute 'transform1' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        3,                   // три компонента: x, y, z
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&loc_vertex[0] + offsetof(T_VERTEX, transform)) + 3 * sizeof(float_t)             // адрес данных
    ); 

    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "transform2");

    if (loc_position < 0) {
        printf("Attribute 'transform2' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        3,                   // три компонента: x, y, z
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&loc_vertex[0] + offsetof(T_VERTEX, transform)) + 6 * sizeof(float_t)             // адрес данных
    ); 


    p_glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, loc_indicies);
 

    return ;
}

void ggDrawArrays()
{

}