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

#include "gd.h"
#include "gd_errors.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int overflow2(int a, int b) {
	if(a <= 0 || b <= 0) {
		gd_error_ex(GD_WARNING,
					"one parameter to a memory allocation multiplication is "
					"negative or zero, failing operation gracefully\n");
		return 1;
	}
	if(a > INT_MAX / b) {
		gd_error_ex(GD_WARNING,
					"product of memory allocation multiplication would exceed "
					"INT_MAX, failing operation gracefully\n");
		return 1;
	}
	return 0;
}

int overflowMul3(int a, int b, int c) {
	if (a < 0 || b < 0 || c < 0) {
		return 1;
	}
	if (a == 0 || b == 0 || c == 0) {
		return 0;
	}
	/* check a*b fits in int first */
	if (a > INT_MAX / b) {
		return 1;
	}
#ifdef HAVE_INT64_T
	/* check a*b*c fits in int64 */
	if ((int64_t)a * b > INT64_MAX / c) {
		return 1;
	}
#else
	/* no 64-bit type available, check against INT_MAX */
	if (a > INT_MAX / b / c) {
		return 1;
	}
#endif
	return 0;
}
