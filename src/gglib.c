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
    float_t position[2]; //x,y
    float_t rotate_angle; //grad
    float_t scale[2]; //x,y
    float_t translate[2]; //x,y
    GLubyte color[4]; //rgba
}T_VERTEX;

#define K_MAX_VERTICIES 10000

typedef struct
{
    T_CONTEXT global_context;
    float_t current_matrix[4]; //текущая матрица масштабирования, вращения
    float_t translate_vector[2]; //вектор перемещения
    float_t NDC_matrix[9]; //матрица преобразования координат в нормализованные координаты устройства
    T_VERTEX verticies[K_MAX_VERTICIES]; //массив вершин
    uint16_t indicies[2*K_MAX_VERTICIES]; //массив индексов вершин
    uint32_t verticies_number; //количество вершин
    uint32_t indicies_number; //количество вершин
    uint32_t vertex_current; //количество вершин
    uint32_t index_current; //количество вершин
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


static void ggAddVerticiesAndIndicies(T_VERTEX *par_vertex, uint32_t par_verticies_number, uint16_t *par_indicies, uint32_t par_indicies_number)
{
    if(gg_context.verticies_number + par_verticies_number < K_MAX_VERTICIES)
    {
        memcpy(&gg_context.verticies[gg_context.verticies_number], par_vertex, sizeof(T_VERTEX) * par_verticies_number);
        gg_context.verticies_number += par_verticies_number;
        memcpy(&gg_context.indicies[gg_context.indicies_number], par_indicies, sizeof(uint16_t) * par_indicies_number);
        gg_context.indicies_number += par_indicies_number;
    }
    else
    {
        /*nothing to do*/
    }

    return;
}


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

    float_t loc_translate_vector[2] = {0, 0};


    float_t loc_NDC_matrix[9] = {0};    

    ggGetNDCCenterMatrix3(par_context->windowWidth, par_context->windowHeight, loc_NDC_matrix);

    gg_context.global_context = *par_context;
    
    memcpy(gg_context.current_matrix, loc_current_matrix, sizeof(gg_context.current_matrix));
    memcpy(gg_context.translate_vector, loc_translate_vector, sizeof(gg_context.translate_vector));
    memcpy(gg_context.NDC_matrix, loc_NDC_matrix, sizeof(gg_context.NDC_matrix));

    return;
}

void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
    T_VERTEX loc_vertex[4];
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
    loc_vertex[0].position[0] = par_x1 - loc_ox;  // P0
    loc_vertex[0].position[1] = par_y1 - loc_oy;
 
    loc_vertex[1].position[0] = par_x1 + loc_ox;  // P1
    loc_vertex[1].position[1] = par_y1 + loc_oy;

    loc_vertex[2].position[0] = par_x2 - loc_ox;  // P2
    loc_vertex[2].position[1] = par_y2 - loc_oy;

    loc_vertex[3].position[0] = par_x2 + loc_ox;  // P3
    loc_vertex[3].position[1] = par_y2 + loc_oy;

    for(int32_t loc_i = 0; loc_i < sizeof(loc_vertex)/sizeof(loc_vertex[0]); loc_i++)
    {
        loc_vertex[loc_i].rotate_angle = 0.0f;
        loc_vertex[loc_i].scale[0] = 1.0f;
        loc_vertex[loc_i].scale[1] = 1.0f;
        loc_vertex[loc_i].translate[0] = gg_context.translate_vector[0];
        loc_vertex[loc_i].translate[1] = gg_context.translate_vector[1];
        loc_vertex[loc_i].color[0] = 255;
        loc_vertex[loc_i].color[1] = 128;
        loc_vertex[loc_i].color[2] = 0;
        loc_vertex[loc_i].color[3] = 255;
    }

    ggAddVerticiesAndIndicies(loc_vertex, sizeof(loc_vertex)/sizeof(loc_vertex[0]), loc_indicies, sizeof(loc_indicies)/sizeof(loc_indicies[0]));    

    return ;
}

void ggDrawArrays()
{
    int32_t loc_screensize[2];
    GLint loc_position;

    SDL_GetWindowSize(gg_context.global_context.window, &loc_screensize[0], &loc_screensize[1]);

    p_glViewport(0, 0, loc_screensize[0], loc_screensize[1]);

    p_glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    p_glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(gg_context.global_context.shaderPrograms[0].shaderProgramID);
    

   loc_position = p_glGetUniformLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "ndc_matrix");

    if (loc_position < 0) {
        printf("Uniform 'ndc_matrix' not found\n");
        return;
    }

    p_glUniformMatrix3fv((GLuint)loc_position, 1, GL_FALSE, gg_context.NDC_matrix);    

    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "position");

    if (loc_position < 0) {
        printf("Attribute 'position' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);

    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        2,                   // два компонента: x, y
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        &gg_context.verticies[gg_context.vertex_current].position[0]             // адрес данных
    );    

    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "rotate_angle");

    if (loc_position < 0) {
        printf("Attribute 'rotate_angle' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        1,                   //
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&gg_context.verticies[gg_context.vertex_current] + offsetof(T_VERTEX, rotate_angle))             // адрес данных
    );    


    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "scale");

    if (loc_position < 0) {
        printf("Attribute 'scale' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        2,                   //
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&gg_context.verticies[gg_context.vertex_current] + offsetof(T_VERTEX, scale))             // адрес данных
    );    


    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "translate");

    if (loc_position < 0) {
        printf("Attribute 'translate' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        2,                   //
        GL_FLOAT,            // тип каждой компоненты
        GL_FALSE,            // не нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&gg_context.verticies[gg_context.vertex_current] + offsetof(T_VERTEX, translate))             // адрес данных
    );
   
    loc_position = p_glGetAttribLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "color");

    if (loc_position < 0) {
        printf("Attribute 'translate_y' not found\n");
        return;
    }

    p_glEnableVertexAttribArray((GLuint)loc_position);
    p_glVertexAttribPointer(
        (GLuint)loc_position, // индекс атрибута
        4,                   // 4 компонента
        GL_UNSIGNED_BYTE,            // тип каждой компоненты
        GL_TRUE,            // нормализовать
        sizeof(T_VERTEX),                   // шаг между вершинами
        (const void *)((const unsigned char *)&gg_context.verticies[gg_context.vertex_current] + offsetof(T_VERTEX, color))             // адрес данных
    );    


    p_glDrawElements(GL_TRIANGLE_STRIP, gg_context.indicies_number, GL_UNSIGNED_SHORT, &gg_context.indicies[gg_context.index_current]); // рисуем треугольник

    
    return;
}

