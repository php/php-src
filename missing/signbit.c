#include <limits.h>
#include "ruby.h"

int
signbit(double x)
{
    enum {double_per_long = sizeof(double) / sizeof(long)};
    enum {long_msb = sizeof(long) * CHAR_BIT - 1};
    union {double d; unsigned long i[double_per_long];} u;
    unsigned long l;

    u.d = x;
#ifdef WORDS_BIGENDIAN
    l = u.i[0];
#else
    l = u.i[double_per_long - 1];
#endif
    return (int)(l >> long_msb);
}
