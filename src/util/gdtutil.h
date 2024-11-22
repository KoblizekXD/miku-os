#ifndef GDTUTIL_H
#define GDTUTIL_H

#include <stdint.h>
#include <stddef.h>

typedef struct _gdt_descriptor {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdt_t;

typedef struct _gdt_entry {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_hi;
} __attribute__((packed)) gdt_entry_t;

void lgdt(gdt_t table);
gdt_t get_table(gdt_entry_t *entries, size_t size);

#endif
