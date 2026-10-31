#ifndef GGLIB_H_
#define GGLIB_H_
#include <stdint.h>
#include "types.h"

extern void ggInit(T_CONTEXT *par_context);
extern void ggLine(float_t par_x1, float_t par_y1, float_t par_x2, float_t par_y2);
extern void ggDrawArrays(void);

#endif /* GGLIB_H_ */


