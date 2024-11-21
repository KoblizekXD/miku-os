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

typedef struct idt_entry {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10))) 
static idt_entry_t idt_entries[256];

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

extern void _gdt_init();

void kload_idt()
{
    // asm("lidt %0" : : "m"(0L));
}

void kmain()
{
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    _gdt_init();
    _sys_csl_init();

    sys_puts("[Miku OS] Initializing...\nHi");
    outb(0x3F8, 'A');


    hcf();
}
