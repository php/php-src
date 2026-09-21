/*
   * gd_security.c
   *
   * Implements buffer overflow check routines.
   *
   * Written 2004, Phil Knirsch.
   * Based on netpbm fixes by Alan Cox.
   *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "gd.h"
#include "gd_errors.h"

int overflow2(int a, int b)
{
	if(a <= 0 || b <= 0) {
		gd_error("One parameter to a memory allocation multiplication is negative or zero, failing operation gracefully\n");
		return 1;
	}
	if(a > INT_MAX / b) {
		gd_error("Product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully\n");
		return 1;
	}
	return 0;
}

int overflowMul3(int a, int b, int c)
{
	if (a < 0 || b < 0 || c < 0) {
		return 1;
	}
	if (a == 0 || b == 0 || c == 0) {
		return 0;
	}
	if (a > INT_MAX / b) {
		return 1;
	}
	if ((int64_t)a * b > INT64_MAX / c) {
		return 1;
	}
	return 0;
}
