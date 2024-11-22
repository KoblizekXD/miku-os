#include <stddef.h>
#include <stdint.h>
#include <util/gdtutil.h>
#include <util/idtutil.h>
#include <util/sysutil.h>
#include <drivers/serial.h>
#include <drivers/csl_drv.h>

extern void reload_segments();

void _ring_3_init(gdt_entry_t *ring3_code, gdt_entry_t *ring3_data)
{
    ring3_code->limit = 0xFFFF;
    ring3_code->base_low = 0;
    ring3_code->base_mid = 0;
    ring3_code->access = 0b11111010;
    ring3_code->granularity = 0b00100000;
    ring3_code->base_hi = 0;

    *ring3_data = *ring3_code;
    ring3_data->access = 0b11110010;
}

__attribute__((interrupt)) void _handler(void* frame)
{
    //outs(COM1, "Division by zero error.\n\r");
    sys_puts("Internal exception: Division by zero");
    hcf();
}

__attribute__((interrupt)) void _gpf_handler(void* frame)
{
    sys_puts("ERROR: GENERAL PROTECTION FAULT\n");
}

idt_entry_t idt_entries[256];
idt_t idt;
gdt_entry_t entries[10];
gdt_t gdt;

void _gdt_init()
{
    // Defined as per https://github.com/limine-bootloader/limine/blob/0e44051ce51457744e5bf7ca7e7c5a7583183fe1/common/sys/gdt.s2.c#L8
    entries[0] = (gdt_entry_t) {0};
    /*entries[1] = (gdt_entry_t) {
        .limit       = 0xffff,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011011,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    };
    entries[2] = (gdt_entry_t) {
        .limit       = 0xffff,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010011,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    };
    entries[3] = (gdt_entry_t) {
        .limit       = 0xffff,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011011,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    };
    entries[4] = (gdt_entry_t) {
        .limit       = 0xffff,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010011,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    };*/
    entries[1] = (gdt_entry_t) {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011011,
        .granularity = 0b00100000,
        .base_hi     = 0x00
    };
    entries[2] = (gdt_entry_t) {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010011,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    };

    _ring_3_init(&entries[3], &entries[4]); // todo: add the tss
    gdt = get_table(entries, 5);
    lgdt(gdt);
    reload_segments();
    
    uint64_t pHandler = (uint64_t) (uintptr_t) _handler;
    uint64_t pGpfHandler = (uint64_t) (uintptr_t) _gpf_handler;

    idt_entries[0] = (idt_entry_t) {
        .offset_1 = pHandler & 0xFFFF,
        .selector = 0x08,
        .ist = 0,
        .type_attributes = 0x8F,
        .offset_2 = (pHandler >> 16) & 0xFFFF,
        .offset_3 = (pHandler >> 32) & 0xFFFFFFFF,
        .zero = 0
    };

    idt_entries[13] = (idt_entry_t) {
        .offset_1 = pGpfHandler & 0xFFFF,
        .selector = 0x08,
        .ist = 0,
        .type_attributes = 0x8F,
        .offset_2 = (pGpfHandler >> 16) & 0xFFFF,
        .offset_3 = (pGpfHandler >> 32) & 0xFFFFFFFF,
        .zero = 0
    };

    idt = get_idt_table(idt_entries, 256);
    
    lidt(idt);
}
