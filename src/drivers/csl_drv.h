#ifndef CSL_DRV_H
#define CSL_DRV_H

#include <stdint.h>

#define SYS_COLOR(r, g, b) (((uint64_t)(r) << 16) | ((uint64_t)(g) << 8) | (uint64_t)(b))

void sys_draw(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint64_t color);
void sys_putc(char c);

#endif
