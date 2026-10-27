#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>


static void mat2_multiply(const float_t *par_mat1, const float_t *par_mat2, float_t *par_out) 
{
    par_out[0] = par_mat1[0] * par_mat2[0]
           + par_mat1[2] * par_mat2[1];

    par_out[1] = par_mat1[1] * par_mat2[0]
           + par_mat1[3] * par_mat2[1];

    par_out[2] = par_mat1[0] * par_mat2[2]
           + par_mat1[2] * par_mat2[3];

    par_out[3] = par_mat1[1] * par_mat2[2]
           + par_mat1[3] * par_mat2[3];

    return;
}

static void mat3_multiply(const float_t par_in1[9], const float_t par_in2[9], float_t par_out[9])
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

void ggRotateMatrix3(float_t *par_in, float_t *par_out, float_t par_angle)
{
    float_t loc_rotate_rad = par_angle * (M_PI / 180.0f);

    /*column-major matrix */
    float_t loc_rotate_matrix[9] =
    {
        cosf(loc_rotate_rad), sinf(loc_rotate_rad), 0, /*cos(a),sin(a),0*/
        -sinf(loc_rotate_rad), cosf(loc_rotate_rad), 0, /*-sin(a),cos(a),0*/
        0, 0, 1
    };        
    
    mat3_multiply(par_in, loc_rotate_matrix, par_out);

    return;
}

void ggScaleMatrix3(float_t *par_in, float_t *par_out, float_t par_scale_x, float_t par_scale_y)
{
    /*column-major matrix */
    float_t loc_scale_matrix[9] =
    {
        par_scale_x, 0, 0, /*Sx,0,0*/
        0, par_scale_y, 0, /*0,Sy,0*/
        0, 0, 1
    };        
    
    mat3_multiply(par_in, loc_scale_matrix, par_out);

    return;
}

void ggTranslateMatrix3(float_t *par_in, float_t *par_out, float_t par_translate_x, float_t par_translate_y)
{
    /*column-major matrix */
    float_t loc_translate_matrix[9] =
    {
        1, 0, 0,
        0, 1, 0,
        par_translate_x, par_translate_y, 1
    };        

    mat3_multiply(par_in, loc_translate_matrix, par_out);

    return;
}

void ggRotateMatrix2(float_t *par_in, float_t *par_out, float_t par_angle)
{
    float_t loc_rotate_rad = par_angle * (M_PI / 180.0f);

    /*column-major matrix */
    float_t loc_rotate_matrix[4] =
    {
        cosf(loc_rotate_rad), sinf(loc_rotate_rad), /*cos(a),sin(a),0*/
        -sinf(loc_rotate_rad), cosf(loc_rotate_rad) /*-sin(a),cos(a),0*/
    };        
    
    mat2_multiply(par_in, loc_rotate_matrix, par_out);

    return;
}

void ggScaleMatrix2(float_t *par_in, float_t *par_out, float_t par_scale_x, float_t par_scale_y)

{
    /*column-major matrix */
    float_t loc_scale_matrix[4] =
    {
        par_scale_x, 0, /*Sx,0,0*/
        0, par_scale_y, /*0,Sy,0*/
    };        
    
    mat2_multiply(par_in, loc_scale_matrix, par_out);

    return;
}

void ggTranslateVertex2(float_t *par_in, float_t *par_out, float_t par_translate_x, float_t par_translate_y)
{
    par_out[0] = par_in[0] + par_translate_x;
    par_out[1] = par_in[1] + par_translate_y;

    return;
}


/* Начало координат — левый верхний угол, Y направлена вниз. */
void ggGetNDCTopLeftMatrix3(float_t par_width, float_t par_height, float_t par_out[9])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL) return;

    par_out[0] =  2.0f / par_width;
    par_out[1] =  0.0f;
    par_out[2] =  0.0f;

    par_out[3] =  0.0f;
    par_out[4] = -2.0f / par_height;
    par_out[5] =  0.0f;

    par_out[6] = -1.0f;
    par_out[7] =  1.0f;
    par_out[8] =  1.0f;

    return;
}

/* Начало координат — левый нижний угол, Y направлена вверх. */
void ggGetNDCBottomLeftMatrix3(float_t par_width, float_t par_height, float_t par_out[9])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL) return;

    par_out[0] = 2.0f / par_width;
    par_out[1] = 0.0f;
    par_out[2] = 0.0f;

    par_out[3] = 0.0f;
    par_out[4] = 2.0f / par_height;
    par_out[5] = 0.0f;

    par_out[6] = -1.0f;
    par_out[7] = -1.0f;
    par_out[8] =  1.0f;

    return;
}

/* Начало координат — центр окна, Y направлена вверх. */
void ggGetNDCCenterMatrix3(float_t par_width, float_t par_height, float_t par_out[9])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL) return;

    par_out[0] = 2.0f / par_width;
    par_out[1] = 0.0f;
    par_out[2] = 0.0f;

    par_out[3] = 0.0f;
    par_out[4] = 2.0f / par_height;
    par_out[5] = 0.0f;

    par_out[6] = 0.0f;
    par_out[7] = 0.0f;
    par_out[8] = 1.0f;

    return;
}




/* Начало координат — левый верхний угол, Y направлена вниз. */
void ggGetNDCTopLeftMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL || par_offset == NULL) return;

    par_out[0] =  2.0f / par_width;
    par_out[1] =  0.0f;

    par_out[2] =  0.0f;
    par_out[3] = -2.0f / par_height;

    par_offset[0] =  -1.0f;
    par_offset[1] =  1.0f;

    return;
}

/* Начало координат — левый нижний угол, Y направлена вверх. */
void ggGetNDCBottomLeftMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL || par_offset == NULL) return;

    par_out[0] = 2.0f / par_width;
    par_out[1] = 0.0f;

    par_out[2] = 0.0f;
    par_out[3] = 2.0f / par_height;

    par_offset[0] = -1.0f;
    par_offset[1] = -1.0f;

    return;
}

/* Начало координат — центр окна, Y направлена вверх. */
void ggGetNDCCenterMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2])
{
    if (par_width <= 0.0f || par_height <= 0.0f || par_out == NULL || par_offset == NULL) return;

    par_out[0] = 2.0f / par_width;
    par_out[1] = 0.0f;

    par_out[2] = 0.0f;
    par_out[3] = 2.0f / par_height;

    par_offset[0] = 0.0f;
    par_offset[1] = 0.0f;

    return;
}


