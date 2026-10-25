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
    float_t origin_matrix[9]; //матрица преобразования координат в зависимости от выбранного центра координат
    float_t scale_matrix[9]; //матрица преобразования координат в нормализованные координаты устройства
    float_t NDC_matrix[9]; //матрица преобразования координат в нормализованные координаты устройства
    float_t transform_matrix[9]; //матрица, включающая все преобразования для текущего шейдера (координаты, поворот, масштаб, NDC)
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
    float_t loc_origin_matrix[9] =
    {
        1, 0, par_context->windowWidth / 2.0f,
        0, -1, par_context->windowHeight / 2.0f,
        0, 0, 1
    };

    float_t loc_scale_matrix[9] =
    {
        1, 0, 0, /*Sx,0,0*/
        0, 1, 0, /*0,Sy,0*/
        0, 0, 1
    };

    float_t loc_rotate_rad = 90.0f  * (M_PI / 180.0f);
    float_t loc_rotate_matrix[9] =
    {
        cosf(loc_rotate_rad), -sinf(loc_rotate_rad), 0, /*cos(a),-sin(a),0*/
        sinf(loc_rotate_rad), cosf(loc_rotate_rad), 0, /*sin(a),cos(a),0*/
        0, 0, 1
    };    

    float_t loc_translate_matrix[9] =
    {
        1, 0, par_context->windowWidth / 2.0f, /*1,0,Tx*/
        0, 1, par_context->windowHeight / 2.0f, /*0,1,Ty*/
        0, 0, 1
    };    

    float_t loc_NDC_matrix[9] =
    {
        2.0f / par_context->windowWidth, 0, -1,
        0, -2.0f / par_context->windowHeight, 1,
        0, 0, 1
    };    

    /*M=TRS*/
    float_t loc_current_matrix[9] =
    {
        2.0f / par_context->windowWidth, 0, -1,
        0, -2.0f / par_context->windowHeight, 1,
        0, 0, 1
    };    

    float_t loc_temp_matrix[9] = {0};
    gg_context.global_context = *par_context;

/*
    mat3_multiply(loc_scale_matrix, loc_rotate_matrix, loc_temp_matrix);
    mat3_multiply(loc_temp_matrix, loc_translate_matrix, loc_origin_matrix);

   
    mat3_multiply(loc_rotate_matrix, loc_scale_matrix, loc_temp_matrix);
    mat3_multiply(loc_translate_matrix, loc_temp_matrix, loc_origin_matrix);

*/
    memcpy(gg_context.origin_matrix, loc_origin_matrix, sizeof(gg_context.origin_matrix));
    memcpy(gg_context.scale_matrix, loc_scale_matrix, sizeof(gg_context.scale_matrix));
    memcpy(gg_context.NDC_matrix, loc_NDC_matrix, sizeof(gg_context.NDC_matrix));

    //mat3_multiply(loc_origin_matrix, loc_NDC_matrix, gg_context.transform_matrix);

    return;
}

void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
typedef struct 
{
    float_t position[3];
    float_t transform[9];
    float_t color[4];
}T_VERTEX;

    T_VERTEX loc_vertex[4];
    float_t loc_vertices[12] = {0.0f};
    int32_t loc_screensize[3] = {0.0f};    
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
    loc_vertices[0] = par_x1 - loc_ox;  // P0
    loc_vertices[1] = par_y1 - loc_oy;
 
    loc_vertices[3] = par_x1 + loc_ox;  // P1
    loc_vertices[4] = par_y1 + loc_oy;

    loc_vertices[6] = par_x2 - loc_ox;  // P2
    loc_vertices[7] = par_y2 - loc_oy;

    loc_vertices[9] = par_x2 + loc_ox;  // P3
    loc_vertices[10] = par_y2 + loc_oy;

    memcpy(loc_vertex[0].position, &loc_vertices[0], sizeof(loc_vertex[0].position));
    memcpy(loc_vertex[1].position, &loc_vertices[3], sizeof(loc_vertex[1].position));
    memcpy(loc_vertex[2].position, &loc_vertices[6], sizeof(loc_vertex[2].position));
    memcpy(loc_vertex[3].position, &loc_vertices[9], sizeof(loc_vertex[3].position));
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

