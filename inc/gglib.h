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
extern void ggCircle(float_t par_x, float_t par_y, float_t par_radius);
extern void ggPolygonFan(float_t *par_x, float_t *par_y, uint32_t par_number);
extern void ggCircleFilled(float_t par_x, float_t par_y, float_t par_radius);
extern void ggArc(float_t par_x, float_t par_y, float_t par_radius, float_t par_start_angle, float_t par_end_angle);
extern void ggPolyline(float_t *par_x, float_t *par_y, uint32_t par_number);
extern void ggPartialCircle(float_t par_x, float_t par_y, float_t par_radius, float_t par_start_angle, float_t par_end_angle);
extern void ggPartialCircleFilled(float_t par_x, float_t par_y, float_t par_radius, float_t par_start_angle, float_t par_end_angle);
extern void ggPolygonStrip(float_t *par_x, float_t *par_y, uint32_t par_number);
extern void ggRingFilled(float_t par_x, float_t par_y, float_t par_inner_radius, float_t par_outer_radius);




extern void ggDrawArrays(void);


#endif /* GGLIB_H_ */


