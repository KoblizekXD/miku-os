#include <util/idtutil.h>

void lidt(idt_t idt)
{
    asm volatile("lidt %0"::"m"(idt));
}

idt_t get_idt_table(idt_entry_t *entries, size_t size)
{
    return (idt_t) {
        .size = size * sizeof(idt_entry_t) - 1,
        .offset = (uint64_t)(uintptr_t)entries
    };

}
