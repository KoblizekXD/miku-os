#include <drivers/csl_drv.h>
#include <stdint.h>
#include <util/sysutil.h>
#include <util/memio.h>
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
uint32_t col = 0;
uint32_t row = 0;

void plot_pixel(struct limine_framebuffer *fb, int x, int y)
{
    volatile uint32_t *ptr = fb->address;
    ptr[x * (fb->pitch / 4) + y] = 0xC0C0C0; 
}

void draw_character(uint8_t *addr, int x, int y)
{
    struct limine_framebuffer *fb = ensure_fb();
    for (uint32_t row = 0; row < font_data->height; row++) {
        uint8_t *row_data = addr + (row * ((font_data->width + 7) / 8));
        for (uint32_t col = 0; col < font_data->width; col++) {
            if (row_data[col / 8] & (0x80 >> (col % 8))) {
                plot_pixel(fb, y + row, x + col);
            }
        }
    }
}

void _sys_csl_init()
{
    font_data = (PSF_font*) &_binary_font_psfu_start;
    glyphs_start = (uint8_t*)font_data + font_data->headersize;
}

void sys_putchar(char c)
{
    struct limine_framebuffer *fb = ensure_fb();
    draw_character(glyphs_start + (c * font_data->bytesperglyph), (col * (font_data->width)) + 1, (row * (font_data->height)) + 1);
    col++;
    if (fb->width <= (col * (font_data->width + 1))) {
        col = 0;
        row++;
    }
}

void sys_puts(const char *string)
{
    for (size_t i = 0; i < strlen(string); i++) {
        if (string[i] == '\n') {
            row++;
            col = 0;
        } else sys_putchar(string[i]);
    }
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
