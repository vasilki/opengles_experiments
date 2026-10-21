#include <stdio.h>
#include <stdint.h>
#include "types.h"
#include "gles2_loader.h"
#include <SDL.h>
#include <math.h>

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


static void mat3_multiply(const float_t par_in1[9], const float_t par_in2[9], float_t *par_out)
{
    float_t loc_temp[9]; // временная матрица, чтобы разрешить R == P или R == M
    int32_t loc_col, loc_row;

    for (loc_col = 0; loc_col < 3; loc_col++) {
        for (loc_row = 0; loc_row < 3; loc_row++) {
            loc_temp[loc_col * 3 + loc_row] =
                par_in1[0 * 3 + loc_row] * par_in2[loc_col * 3 + 0] +
                par_in1[1 * 3 + loc_row] * par_in2[loc_col * 3 + 1] +
                par_in1[2 * 3 + loc_row] * par_in2[loc_col * 3 + 2];
        }
    }

    for (loc_col = 0; loc_col < 9; loc_col++) {
        par_out[loc_col] = loc_temp[loc_col];
    }

    return;
}

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
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };    

    float_t loc_NDC_matrix[9] =
    {
        2.0f / par_context->windowWidth, 0, -1,
        0, -2.0f / par_context->windowHeight, 1,
        0, 0, 0
    };    

    gg_context.global_context = *par_context;

    memcpy(gg_context.origin_matrix, loc_origin_matrix, sizeof(gg_context.origin_matrix));
    memcpy(gg_context.scale_matrix, loc_scale_matrix, sizeof(gg_context.scale_matrix));
    memcpy(gg_context.NDC_matrix, loc_NDC_matrix, sizeof(gg_context.NDC_matrix));

    mat3_multiply(loc_origin_matrix, loc_NDC_matrix, gg_context.transform_matrix);

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

    SDL_GetWindowSize(gg_context.global_context.window, &loc_screensize[0], &loc_screensize[1]);

    p_glViewport(0, 0, loc_screensize[0], loc_screensize[1]);

    p_glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    p_glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(gg_context.global_context.shaderPrograms[0].shaderProgramID);

    loc_position = p_glGetUniformLocation(gg_context.global_context.shaderPrograms[0].shaderProgramID, "transform");

    if (loc_position < 0) {
        printf("Uniform 'transform' not found\n");
        return;
    }

    p_glUniformMatrix3fv((GLuint)loc_position, 1, GL_FALSE, gg_context.transform_matrix);

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
        0,                   // шаг между вершинами
        loc_vertices             // адрес данных
    );    


    p_glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, loc_indicies);
 

    return ;
}



