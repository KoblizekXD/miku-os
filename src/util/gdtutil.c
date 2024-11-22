#include <stdint.h>
#include <util/gdtutil.h>

void lgdt(gdt_t table)
{
    asm volatile("lgdt %0" : : "m"(table));
}

gdt_t get_table(gdt_entry_t *entries, size_t size)
{
    return (gdt_t) {
        .size = size * sizeof(gdt_entry_t) - 1,
        .offset = (uint64_t)(uintptr_t)entries
    };
}
