#include "ruby/missing.h"
#include <math.h>

double cbrt(double x)
{
    if (x < 0)
        return -pow(-x, 1/3.0);
    else
        return pow(x, 1/3.0);
}

