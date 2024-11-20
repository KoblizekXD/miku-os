#include <stddef.h>
#include <stdint.h>

typedef struct _gdt_entry {
	unsigned int limit_low              : 16; // ignored in 64 bits
	unsigned int base_low               : 24; // ignored in 64 bits
	unsigned int accessed               :  1; // 1
	unsigned int read_write             :  1; // readable for code, writable for data
	unsigned int conforming_expand_down :  1; // conforming for code, expand down for data
	unsigned int code                   :  1; // 1 for code, 0 for data
	unsigned int code_data_segment      :  1; // should be 1 for everything but TSS and LDT
	unsigned int DPL                    :  2; // privilege level
	unsigned int present                :  1; // always 1
	unsigned int limit_high             :  4;
	unsigned int available              :  1; // only used in software; has no effect on hardware
	unsigned int long_mode              :  1;
	unsigned int big                    :  1; // 32-bit opcodes for code, uint32_t stack for data
	unsigned int gran                   :  1; // 1 to use 4k page addressing, 0 for byte addressing
	unsigned int base_high              : 16; // would be 8 in 32 bit
} __attribute__((packed)) gdt_entry_t;

void _ring_3_init(gdt_entry_t *ring3_data, gdt_entry_t *ring3_code)
{
    ring3_code->limit_low = 0xFFFF;
    ring3_code->base_low = 0;
    ring3_code->accessed = 0;
    ring3_code->read_write = 1; // since this is a code segment, specifies that the segment is readable
    ring3_code->conforming_expand_down = 0;
    ring3_code->code = 1;
    ring3_code->code_data_segment = 1;
    ring3_code->DPL = 3; // ring 3
    ring3_code->present = 1;
    ring3_code->limit_high = 0xF;
    ring3_code->available = 1;
    ring3_code->long_mode = 0;
    ring3_code->big = 1; // it's 32 bits
    ring3_code->gran = 1; // 4KB page addressing
    ring3_code->base_high = 0;

    *ring3_data = *ring3_code; // contents are similar so save time by copying
    ring3_data->code = 0; // not code but data
}

void _gdt_init()
{
    static gdt_entry_t entries[6];

    entries[1] = (gdt_entry_t) {
        .limit_low = 0x0,
        .base_low  = 0x0,
        .accessed  = 0x1,
        .read_write = 0x1,
        .conforming_expand_down = 0x0,
        .code = 0x1,
        .code_data_segment = 0x1,
        .DPL = 0x0,
        .present = 0x1,
        .limit_high = 0x0,
        .long_mode = 0x1,
        .big = 0x0,
        .gran = 0x1,
        .base_high = 0x00
    };

    entries[2] = (gdt_entry_t) {
        .limit_low = 0x0,
        .base_low = 0x0,
        .accessed = 0x1,
        .read_write = 0x1,
        .conforming_expand_down = 0x0,
        .code = 0x0,
        .code_data_segment = 0x1,
        .DPL = 0x0,
        .present = 0x1,
        .limit_high = 0x0,
        .long_mode = 0x1,
        .big = 0x1,
        .gran = 0x0,
        .base_high = 0x00
    };

    _ring_3_init(&entries[4], &entries[3]);

    asm("lgdt (%0)" : : "r"(&entries));
}
