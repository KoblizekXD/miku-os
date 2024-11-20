#include <stddef.h>
#include <util/memio.h>

size_t strlen(const char *start)
{
    const char* marker = start;
    while (*marker != '\0') {
        marker++;
    }
    return marker - start;
}
