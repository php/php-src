/* public domain rewrite of isinf(3) */

#ifdef __osf__

#define _IEEE 1
#include <nan.h>

int
isinf(double n)
{
    if (IsNANorINF(n) && IsINF(n)) {
	return 1;
    }
    else {
	return 0;
    }
}

#else

#include "ruby/config.h"

#if defined(HAVE_FINITE) && defined(HAVE_ISNAN)

#include <math.h>
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

/*
 * isinf may be provided only as a macro.
 * ex. HP-UX, Solaris 10
 * http://www.gnu.org/software/automake/manual/autoconf/Function-Portability.html
 */
#ifndef isinf
int
isinf(double n)
{
    return (!finite(n) && !isnan(n));
}
#endif

#else

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

static double zero(void) { return 0.0; }
static double one (void) { return 1.0; }
static double inf (void) { return one() / zero(); }

int
isinf(double n)
{
    static double pinf = 0.0;
    static double ninf = 0.0;

    if (pinf == 0.0) {
	pinf = inf();
	ninf = -pinf;
    }
    return memcmp(&n, &pinf, sizeof n) == 0
	|| memcmp(&n, &ninf, sizeof n) == 0;
}
#endif
#endif
