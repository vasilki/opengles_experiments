#include <stdio.h>
#include "types.h"
#include "gles2_loader.h"
#include "types.h"
#include <SDL.h>
#include "gglib.h"

void draw(T_CONTEXT *par_context) {

    int32_t i;
    for (i=0; i < 10; i++)
    {
        ggColor(255, 128 +10*i, 0, 255);
        ggLine(10 + i*10, 10, 50 + i*10, 50);
    }
    ggColor(50, 128, 0, 255);
//    ggRectangle(-100,-100,-50,-50);

    ggColor(50, 128, 70, 255);
    //ggRectangleFilled(100,100,150,150);

    ggDrawArrays();

    return;
}








/*





void draw2(T_CONTEXT *par_context) {

    const GLfloat loc_vertices[] = {
        0.0f,  0.5f, 0.0f,  // верхняя вершина
        -0.5f, -0.5f, 0.0f,  // нижняя левая
        0.5f, -0.5f, 0.0f   // нижняя правая
    };
    GLint loc_position;
    GLuint loc_width, loc_height;

    SDL_GetWindowSize(par_context->window, &loc_width, &loc_height);

    p_glViewport(0, 0, loc_width, loc_height);

    p_glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    p_glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(par_context->shaderPrograms[0].shaderProgramID);

    loc_position = p_glGetAttribLocation(par_context->shaderPrograms[0].shaderProgramID, "position");

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

    p_glDrawArrays(GL_TRIANGLES, 0, 3);

    return;
}

*/