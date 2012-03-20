/* public domain rewrite of strstr(3) */

#include "ruby/missing.h"

size_t strlen(const char*);

char *
strstr(const char *haystack, const char *needle)
{
    const char *hend;
    const char *a, *b;

    if (*needle == 0) return (char *)haystack;
    hend = haystack + strlen(haystack) - strlen(needle) + 1;
    while (haystack < hend) {
	if (*haystack == *needle) {
	    a = haystack;
	    b = needle;
	    for (;;) {
		if (*b == 0) return (char *)haystack;
		if (*a++ != *b++) {
		    break;
		}
	    }
	}
	haystack++;
    }
    return 0;
}
