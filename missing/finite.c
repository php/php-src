/* public domain rewrite of finite(3) */

#include "ruby/missing.h"

int
finite(double n)
{
    return !isnan(n) && !isinf(n);
}
