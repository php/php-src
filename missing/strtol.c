/* public domain rewrite of strtol(3) */

#include "ruby/missing.h"
#include <ctype.h>

long
strtol(const char *nptr, char **endptr, int base)
{
    long result;
    const char *p = nptr;

    while (isspace(*p)) {
	p++;
    }
    if (*p == '-') {
	p++;
	result = -strtoul(p, endptr, base);
    }
    else {
	if (*p == '+') p++;
	result = strtoul(p, endptr, base);
    }
    if (endptr != 0 && *endptr == p) {
	*endptr = (char *)nptr;
    }
    return result;
}
