#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void outb(uint16_t port, uint8_t c);
uint8_t inb(uint16_t port);

#endif
