#include <stdbool.h>
#include <limine.h>
#include <util/sysutil.h>
#include <drivers/csl_drv.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

void kmain()
{
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }
    _sys_csl_init();

    sys_puts("[Miku OS] Initializing...\nHi");

    hcf();
}
