#ifndef MEM_IO_H
#define MEM_IO_H

#include <stddef.h>

void* memcpy(void* dest, const void* src, size_t count);
void *memset(void *address, int c, size_t count);
void *memmove(void *dest, const void *src, size_t count);
int memcmp(const void *s1, const void *s2, size_t count);
size_t strlen(const char* start);

#endif
