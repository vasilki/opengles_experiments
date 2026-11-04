#ifndef GGLIB_H_
#define GGLIB_H_
#include <stdint.h>
#include "types.h"

extern void ggInit(T_CONTEXT *par_context);
extern void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2);
extern void ggColor(GLubyte par_red, GLubyte par_green, GLubyte par_blue, GLubyte par_alpha);
extern void ggRectangle(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2);
extern void ggRectangleFilled(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2);
extern void ggSetWidth(uint32_t par_width);
extern void ggDrawArrays(void);

#endif /* GGLIB_H_ */


