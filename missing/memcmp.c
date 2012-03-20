/* public domain rewrite of memcmp(3) */

#include "ruby/missing.h"
#include <stddef.h>

int
memcmp(const void *s1, const void *s2, size_t len)
{
    register unsigned char *a = (unsigned char*)s1;
    register unsigned char *b = (unsigned char*)s2;
    register int tmp;

    for (; len; --len) {
	if (tmp = *a++ - *b++)
	    return tmp;
    }
    return 0;
}
