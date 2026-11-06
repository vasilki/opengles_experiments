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
#define K_MAX_INDICIES (2*K_MAX_VERTICIES)

typedef struct
{
    T_CONTEXT global_context;
    float_t current_matrix[4]; //текущая матрица масштабирования, вращения
    float_t translate_vector[2]; //вектор перемещения
    float_t NDC_matrix[9]; //матрица преобразования координат в нормализованные координаты устройства
    T_VERTEX verticies[K_MAX_VERTICIES]; //массив вершин
    uint16_t indicies[K_MAX_INDICIES]; //массив индексов вершин
    uint32_t verticies_number; //количество вершин
    uint32_t indicies_number; //количество вершин
    GLubyte color[4]; //rgba
    uint32_t line_width; //толщина линии
}T_GGLIB_CONTEXT;


static T_GGLIB_CONTEXT gg_context;

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
    uint32_t loc_verticies_base_number = gg_context.verticies_number;
    if(gg_context.verticies_number + par_verticies_number < K_MAX_VERTICIES)
    {
        memcpy(&gg_context.verticies[gg_context.verticies_number], par_vertex, sizeof(T_VERTEX) * par_verticies_number);
        gg_context.verticies_number += par_verticies_number;

        for(uint32_t i = 0; i < par_indicies_number; i++)
        {
            gg_context.indicies[gg_context.indicies_number + i] = par_indicies[i] + loc_verticies_base_number;
        }
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

    gg_context.line_width = 1;

    return;
}

void ggColor(GLubyte par_red, GLubyte par_green, GLubyte par_blue, GLubyte par_alpha)
{
    gg_context.color[0] = par_red;
    gg_context.color[1] = par_green;
    gg_context.color[2] = par_blue;
    gg_context.color[3] = par_alpha;

    return;
}

void ggSetWidth(uint32_t par_width)
{
    gg_context.line_width = par_width;
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

    if (loc_length < 1e-6f || gg_context.line_width <= 0.0f)
        return ;

    // Единичный перпендикуляр к отрезку.
    float_t loc_nx = -loc_dy / loc_length;
    float_t loc_ny =  loc_dx / loc_length;

    // Смещение на половину толщины.
    float_t loc_ox = loc_nx * gg_context.line_width * 0.5f;
    float_t loc_oy = loc_ny * gg_context.line_width * 0.5f;

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
        memcpy(&loc_vertex[loc_i].color[0], &gg_context.color[0], sizeof(gg_context.color));
    }

    ggAddVerticiesAndIndicies(loc_vertex, sizeof(loc_vertex)/sizeof(loc_vertex[0]), loc_indicies, sizeof(loc_indicies)/sizeof(loc_indicies[0]));    
   
    return ;
}

void ggRectangle(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
    float_t loc_half_width = gg_context.line_width * 0.5f; //pseudo miter

    ggLine(par_x1 - loc_half_width, par_y1, par_x2 + loc_half_width, par_y1);
    ggLine(par_x2, par_y1 - loc_half_width, par_x2, par_y2 + loc_half_width);
    ggLine(par_x2 + loc_half_width, par_y2, par_x1 - loc_half_width, par_y2);
    ggLine(par_x1, par_y2 + loc_half_width, par_x1, par_y1 - loc_half_width);
    
    return ;
}


void ggRectangleFilled(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2)
{
    T_VERTEX loc_vertex[4];
    uint16_t loc_indicies[6] = {0,1,2,2,1,3};

    // Порядок вершин для GL_TRIANGLE_STRIP.
    loc_vertex[0].position[0] = par_x1;
    loc_vertex[0].position[1] = par_y1;
 
    loc_vertex[1].position[0] = par_x1;
    loc_vertex[1].position[1] = par_y2;

    loc_vertex[2].position[0] = par_x2;
    loc_vertex[2].position[1] = par_y1;

    loc_vertex[3].position[0] = par_x2;
    loc_vertex[3].position[1] = par_y2;

    for(int32_t loc_i = 0; loc_i < sizeof(loc_vertex)/sizeof(loc_vertex[0]); loc_i++)
    {
        loc_vertex[loc_i].rotate_angle = 0.0f;
        loc_vertex[loc_i].scale[0] = 1.0f;
        loc_vertex[loc_i].scale[1] = 1.0f;
        loc_vertex[loc_i].translate[0] = gg_context.translate_vector[0];
        loc_vertex[loc_i].translate[1] = gg_context.translate_vector[1];
        memcpy(&loc_vertex[loc_i].color[0], &gg_context.color[0], sizeof(gg_context.color));
    }

    ggAddVerticiesAndIndicies(loc_vertex, sizeof(loc_vertex)/sizeof(loc_vertex[0]), loc_indicies, sizeof(loc_indicies)/sizeof(loc_indicies[0]));    
   
    return ;
}

// par_radius - радиус в пикселях
// par_accuracy - желаемая точность в пикселях (например, 1.0)
static int32_t ggCalculateCircleSegments(float_t par_radius, float_t par_accuracy) {
    uint32_t loc_segments_number = 6;
    
    // Защита от деления на ноль и микро-объектов
    if ((par_radius <= 0.1f) || (par_radius < par_accuracy)) {
        return loc_segments_number;
    }
    else
    {
        /*nothin to do*/
    }

    /*
        Расстояние до хорды H = R * cos(tetta/2), где theta - угол между радиусами, проведенными к концам хорды.
        Нас интересует минимизация расстояния от дуги до хорды.
        Расстояние от центра до дуги равно R, а до хорды H. Разница S (par_accuracy) между ними и есть максимальное отклонение дуги от хорды.
        S = R - H = R - R * cos(tetta/2) = R * (1 - cos(tetta/2))
        Полный радиус круга 2pi. Угол одного сегмента равен 2pi/N, где N - количество сегментов. Тогда tetta = 2pi/N.
        Тогда S = R * (1 - cos(pi/N))
        отсюда cos(pi/N) = 1 - S/R
        pi/N = acos(1 - S/R)
        N = pi / acos(1 - S/R)
    */

    float_t loc_temp = 1.0f - par_accuracy / par_radius;

    if((loc_temp <= 1.02f) && (loc_temp >= 0.98f)) 
    {
        loc_segments_number = 128;
    }
    else
    {
        loc_temp = 3.1415926f / acosf(loc_temp);
        loc_segments_number = (uint32_t)round(loc_temp);
        // Ограничиваем диапазон
        if (loc_segments_number < 16) loc_segments_number = 16;
        if (loc_segments_number > 256) loc_segments_number = 256;
    }

    return loc_segments_number;
}

void ggCircle(float_t par_x, float_t par_y, float_t par_radius)
{
    uint32_t loc_segments_number;
    float_t loc_x;
    float_t loc_y;
    float_t loc_angle;

    loc_segments_number = ggCalculateCircleSegments(par_radius, 1.0f);

    loc_angle = 2.0f * 3.1415926f / loc_segments_number;

    for(uint32_t i = 0; i < loc_segments_number; i++)
    {
        loc_x = par_x + par_radius * cosf(loc_angle * i);
        loc_y = par_y + par_radius * sinf(loc_angle * i);

        if(i > 0)
        {
            ggLine(loc_x, loc_y, par_x + par_radius * cosf(loc_angle * (i - 1)), par_y + par_radius * sinf(loc_angle * (i - 1)));
        }
        else
        {
            /*nothing to do*/
        }
    }
    ggLine(par_x + par_radius, par_y, loc_x, loc_y);

    return;
}


void ggPolygonFan(float_t *par_x, float_t *par_y, uint32_t par_number)
{
    T_VERTEX loc_vertex;
    uint16_t loc_indicies[3];
    uint32_t loc_count = 0;

    loc_indicies[0] = (uint16_t)gg_context.verticies_number;
    loc_indicies[1] = loc_indicies[0] + 1;
    loc_indicies[2] = loc_indicies[1] + 1;

    loc_vertex.rotate_angle = 0.0f;
    loc_vertex.scale[0] = 1.0f;
    loc_vertex.scale[1] = 1.0f;
    loc_vertex.translate[0] = gg_context.translate_vector[0];
    loc_vertex.translate[1] = gg_context.translate_vector[1];
    memcpy(&loc_vertex.color[0], &gg_context.color[0], sizeof(gg_context.color));

    while(loc_count < par_number)
    {    
        loc_vertex.position[0] = par_x[loc_count];
        loc_vertex.position[1] = par_y[loc_count];

        if(gg_context.verticies_number + par_number < K_MAX_VERTICIES)
        {
            memcpy(&gg_context.verticies[gg_context.verticies_number], &loc_vertex, sizeof(loc_vertex));
            gg_context.verticies_number++;
            if(loc_count < 3)
            {
                gg_context.indicies[gg_context.indicies_number] = loc_indicies[loc_count % 3];
                gg_context.indicies_number++;
            }
            else
            {
                gg_context.indicies[gg_context.indicies_number] = loc_indicies[0];
                gg_context.indicies_number++;
                gg_context.indicies[gg_context.indicies_number] = loc_indicies[1];
                gg_context.indicies_number++;
                gg_context.indicies[gg_context.indicies_number] = loc_indicies[2];
                gg_context.indicies_number++;
            }
        }
        else
        {
            /*nothing to do*/
        }
        loc_count++;

        if(loc_count % 3 == 0)
        {
            loc_indicies[1]++;
            loc_indicies[2]++;
        }
        else
        {
            /*nothing to do*/
        }
    }

    return;
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
        &gg_context.verticies[0].position[0]             // адрес данных
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
        (const void *)((const unsigned char *)&gg_context.verticies[0] + offsetof(T_VERTEX, rotate_angle))             // адрес данных
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
        (const void *)((const unsigned char *)&gg_context.verticies[0] + offsetof(T_VERTEX, scale))             // адрес данных
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
        (const void *)((const unsigned char *)&gg_context.verticies[0] + offsetof(T_VERTEX, translate))             // адрес данных
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
        (const void *)((const unsigned char *)&gg_context.verticies[0] + offsetof(T_VERTEX, color))             // адрес данных
    );    

    p_glDrawElements(GL_TRIANGLES, gg_context.indicies_number, GL_UNSIGNED_SHORT, &gg_context.indicies[0]);

    
    return;
}

