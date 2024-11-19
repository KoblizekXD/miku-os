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

void sys_putc(char c)
{
    struct limine_framebuffer *framebuffer = ensure_fb();
    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (size_t i = 0; i < 100; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
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
