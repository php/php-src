/* public domain rewrite of isnan(3) */

#include "ruby/missing.h"

static int double_ne(double n1, double n2);

int
isnan(double n)
{
    return double_ne(n, n);
}

static int
double_ne(double n1, double n2)
{
    return n1 != n2;
}
