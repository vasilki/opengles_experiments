#ifndef MATRIX_H_
#define MATRIX_H_
#include <stdint.h>

extern void ggRotateMatrix3(float_t *par_in, float_t *par_out, float_t par_angle);

extern void ggScaleMatrix3(float_t *par_in, float_t *par_out, float_t par_scale_x, float_t par_scale_y);

extern void ggTranslateMatrix3(float_t *par_in, float_t *par_out, float_t par_translate_x, float_t par_translate_y);

extern void ggRotateMatrix2(float_t *par_in, float_t *par_out, float_t par_angle);

extern void ggScaleMatrix2(float_t *par_in, float_t *par_out, float_t par_scale_x, float_t par_scale_y);

extern void ggTranslateVertex2(float_t *par_in, float_t *par_out, float_t par_translate_x, float_t par_translate_y);

/* Начало координат — левый верхний угол, Y направлена вниз. */
extern void ggGetNDCTopLeftMatrix3(float_t par_width, float_t par_height, float_t par_out[9]);

/* Начало координат — левый нижний угол, Y направлена вверх. */
extern void ggGetNDCBottomLeftMatrix3(float_t par_width, float_t par_height, float_t par_out[9]);

/* Начало координат — центр окна, Y направлена вверх. */
extern void ggGetNDCCenterMatrix3(float_t par_width, float_t par_height, float_t par_out[9]);

/* Начало координат — левый верхний угол, Y направлена вниз. */
extern void ggGetNDCTopLeftMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2]);

/* Начало координат — левый нижний угол, Y направлена вверх. */
extern void ggGetNDCBottomLeftMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2]);

/* Начало координат — центр окна, Y направлена вверх. */
extern void ggGetNDCCenterMatrix2(float_t par_width, float_t par_height, float_t par_out[4], float_t par_offset[2]);

#endif /* MATRIX_H_ */


