/* public domain rewrite of strchr(3) and strrchr(3) */

#include "ruby/missing.h"

size_t strlen(const char*);

char *
strchr(const char *s, int c)
{
    if (c == 0) return (char *)s + strlen(s);
    while (*s) {
	if (*s == c)
	    return (char *)s;
	s++;
    }
    return 0;
}

char *
strrchr(const char *s, int c)
{
    const char *save;

    if (c == 0) return (char *)s + strlen(s);
    save = 0;
    while (*s) {
	if (*s == c)
	    save = s;
	s++;
    }
    return (char *)save;
}
