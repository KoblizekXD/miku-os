#ifndef SYSUTIL_H
#define SYSUTIL_H

static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

#endif
