/* ffs.c - find first set bit */
/* ffs() is defined by Single Unix Specification. */

#include "ruby.h"

int ffs(int arg)
{
    unsigned int x = (unsigned int)arg;
    int r;

    if (x == 0)
        return 0;

    r = 1;

#if 32 < SIZEOF_INT * CHAR_BIT
    if ((x & 0xffffffff) == 0) {
        x >>= 32;
        r += 32;
    }
#endif

    if ((x & 0xffff) == 0) {
        x >>= 16;
        r += 16;
    }

    if ((x & 0xff) == 0) {
        x >>= 8;
        r += 8;
    }

    if ((x & 0xf) == 0) {
        x >>= 4;
        r += 4;
    }

    if ((x & 0x3) == 0) {
        x >>= 2;
        r += 2;
    }

    if ((x & 0x1) == 0) {
        x >>= 1;
        r += 1;
    }

    return r;
}
