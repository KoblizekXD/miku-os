#ifndef IDTUTIL_H
#define IDTUTIL_H

#include <stddef.h>
#include <stdint.h>

typedef struct _idt_table {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) idt_t;

typedef struct _idt_entry {
    uint16_t offset_low;
    uint16_t segment_s;
    uint8_t  ist: 3;
    uint8_t  reserved: 5;
    uint8_t  gate: 4;
    uint8_t  zero: 1;
    uint8_t  dpl: 2;
    uint8_t  present: 1;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved_high;
} __attribute__((packed)) idt_entry_t;

void lidt(idt_t idt);
idt_t get_idt_table(idt_entry_t *entries, size_t size);

#endif
