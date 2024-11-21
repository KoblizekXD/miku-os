#include <stddef.h>
#include <stdint.h>
#include <util/gdtutil.h>

void _ring_3_init(gdt_entry_t *ring3_data, gdt_entry_t *ring3_code)
{
    ring3_code->limit_low = 0xFFFF;
    ring3_code->base_low = 0;
    ring3_code->accessed = 0;
    ring3_code->readable = 1; // since this is a code segment, specifies that the segment is readable
    ring3_code->conforming = 0;
    ring3_code->executable = 1;
    ring3_code->descriptor_type = 1;
    ring3_code->privilege_level = 3; // ring 3
    ring3_code->present = 1;
    ring3_code->limit_high = 0xF;
    ring3_code->available = 1;
    ring3_code->long_mode = 0;
    ring3_code->default_operation_size = 1; // it's 32 bits
    ring3_code->granularity = 1; // 4KB page addressing
    ring3_code->base_high = 0;

    *ring3_data = *ring3_code; // contents are similar so save time by copying
    ring3_data->executable = 0; // not code but data
}

void _gdt_init()
{
    static gdt_entry_t entries[6];

    entries[1] = (gdt_entry_t) {
        .limit_low = 0x0,
        .base_low  = 0x0,
        .accessed  = 0x1,
        .readable = 0x1,
        .conforming = 0x0,
        .executable = 0x1,
        .descriptor_type = 0x1,
        .privilege_level = 0x0,
        .present = 0x1,
        .limit_high = 0x0,
        .long_mode = 0x1,
        .default_operation_size = 0x0,
        .granularity = 0x1,
        .base_high = 0x00
    };

    entries[2] = (gdt_entry_t) {
        .limit_low = 0x0,
        .base_low = 0x0,
        .accessed = 0x1,
        .readable = 0x1,
        .conforming = 0x0,
        .executable = 0x0,
        .descriptor_type = 0x1,
        .privilege_level = 0x0,
        .present = 0x1,
        .limit_high = 0x0,
        .long_mode = 0x1,
        .default_operation_size = 0x1,
        .granularity = 0x0,
        .base_high = 0x00
    };

    _ring_3_init(&entries[4], &entries[3]);

    gdt_t table = get_table(entries, 6);
    lgdt(&table);
}
