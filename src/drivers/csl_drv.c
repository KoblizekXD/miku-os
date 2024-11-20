#include <drivers/csl_drv.h>
#include <stdint.h>
#include <util/sysutil.h>
#include <stddef.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

struct limine_framebuffer* ensure_fb()
{
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    return framebuffer_request.response->framebuffers[0];
}

extern uint8_t _binary_font_psfu_start;
extern uint8_t _binary_font_psfu_end;

uint8_t *glyphs_start;
PSF_font *font_data = NULL;

void plot_pixel(struct limine_framebuffer *fb, int x, int y)
{
    volatile uint32_t *ptr = fb->address;
    ptr[x * (fb->pitch / 4) + y] = 0xffffff; 
}

void draw_character(uint8_t *addr, int x, int y)
{
    struct limine_framebuffer *fb = ensure_fb();
    for (uint32_t row = 0; row < font_data->height; row++) {
        uint8_t *row_data = addr + (row * ((font_data->width + 7) / 8));
        for (uint32_t col = 0; col < font_data->width; col++) {
            if (row_data[col / 8] & (0x80 >> (col % 8))) {
                plot_pixel(fb, y + row, x + col); // Adjust x, y as needed
            }
        }
    }
}

void _sys_csl_init()
{
    struct limine_framebuffer *fb = ensure_fb();
    font_data = (PSF_font*) &_binary_font_psfu_start;
//    if (font->flags) return;
  
    glyphs_start = (uint8_t*)font_data + font_data->headersize;
    uint32_t x_offset = 0, y_offset = 0;
    for (uint32_t glyph = 60; glyph < 80; glyph++) {
        uint8_t *glyph_data = glyphs_start + (glyph * font_data->bytesperglyph);
        for (uint32_t row = 0; row < font_data->height; row++) {
            uint8_t *row_data = glyph_data + (row * ((font_data->width + 7) / 8));
            for (uint32_t col = 0; col < font_data->width; col++) {
                if (row_data[col / 8] & (0x80 >> (col % 8))) {
                    // plot_pixel(fb, y_offset + row, x_offset + col); // Adjust x, y as needed
                }
            }
        }
        x_offset += font_data->width + 1;
        if (x_offset + font_data->width >= fb->width) {
            x_offset = 0;
            y_offset += font_data->height + 1;
        }
    }
}

void sys_putc(char c)
{
    draw_character(glyphs_start + (c * font_data->bytesperglyph), 1, 1);
}

void sys_draw(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint64_t color)
{
    struct limine_framebuffer* framebuffer = ensure_fb();
    
    volatile uint32_t *addr = framebuffer->address;
    for (size_t i = x1; i < x2; i++) {
        for (size_t j = y1; j < y2; j++) {
            addr[i * (framebuffer->pitch / 4) + j] = color;
        }
    }
}
