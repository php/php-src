/* public domain rewrite of memcmp(3) */

#include "ruby/missing.h"
#include <stddef.h>

void *
memmove(void *d, const void *s, size_t n)
{
    char *dst = (char *)d;
    const char *src = (const char *)s;

    if (src < dst) {
	src += n;
	dst += n;
	for (; n; --n)
	    *--dst = *--src;
    }
    else if (dst < src)
	for (; n; --n)
	    *dst++ = *src++;
    return d;
}
