#include <drivers/serial.h>
#include <stddef.h>
#include <util/memio.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t c)
{
    asm(
        "out %0, %1"
        : 
        : "a"(c), "Nd"(port)
    );
}

uint8_t inb(uint16_t port)
{
    uint8_t value;
    asm(
        "in %1, %0"
        : "=a"(value) : "Nd"(port)
    );
    return value;
}

void outs(uint16_t port, const char *string)
{
    for (size_t i = 0; i < strlen(string); i++)
        outb(port, string[i]);
}
