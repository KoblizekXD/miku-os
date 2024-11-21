#ifndef GDTUTIL_H
#define GDTUTIL_H

#include <stdint.h>
#include <stddef.h>

typedef struct _gdt_descriptor {
    uint16_t size;
    uint64_t offset;
} gdt_t;

typedef struct _gdt_entry {
    // Limit and base address
    uint16_t limit_low : 16;      // Lower 16 bits of the segment limit (ignored in 64-bit mode)
    uint16_t base_low : 16;       // Lower 16 bits of the base address
    uint8_t base_middle : 8;      // Next 8 bits of the base address

    // Access byte
    uint8_t accessed : 1;         // Set to 1 if the segment has been accessed
    uint8_t readable : 1;         // For code segments: readable; for data: writable
    uint8_t conforming : 1;       // For code segments: conforming; for data: expand-down
    uint8_t executable : 1;       // 1 = Code segment, 0 = Data segment
    uint8_t descriptor_type : 1;  // 1 = Code/data segment, 0 = System segment
    uint8_t privilege_level : 2;  // Descriptor privilege level (0 = kernel, 3 = user)
    uint8_t present : 1;          // Segment present in memory

    // Flags and limit high
    uint8_t limit_high : 4;       // Upper 4 bits of the segment limit (ignored in 64-bit mode)
    uint8_t available : 1;        // Available for use by the OS
    uint8_t long_mode : 1;        // 1 = 64-bit segment
    uint8_t default_operation_size : 1; // 0 = 16-bit segment, 1 = 32-bit segment (ignored in 64-bit mode)
    uint8_t granularity : 1;      // Granularity: 0 = Byte, 1 = 4 KB

    uint8_t base_high : 8;        // Last 8 bits of the base address
    uint32_t base_upper;          // Upper 32 bits of the base address (for system segments like TSS)
    uint32_t reserved;            // Reserved, must be zero
} __attribute__((packed)) gdt_entry_t;

void lgdt(gdt_t *table);
gdt_t get_table(gdt_entry_t *entries, size_t size);

#endif
