#ifndef CSL_DRV_H
#define CSL_DRV_H

#include <stdint.h>

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

#define SYS_COLOR(r, g, b) (((uint64_t)(r) << 16) | ((uint64_t)(g) << 8) | (uint64_t)(b))

void _sys_csl_init();
void sys_draw(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint64_t color);
void sys_putchar(char c);
void sys_puts(const char *string);

#endif
