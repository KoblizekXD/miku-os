#include <stdbool.h>
#include <limine.h>
#include <util/sysutil.h>
#include <drivers/serial.h>
#include <drivers/csl_drv.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

extern void _gdt_init();

void kmain()
{
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    _gdt_init();
    _sys_csl_init();
    
    volatile char x = 0; x /= x;

    sys_puts("[Miku OS] Initializing...\nHi");
    // outb(0x3F8, 'A');

    hcf();
}
