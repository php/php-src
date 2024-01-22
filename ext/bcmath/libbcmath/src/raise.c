/* raise.c: bcmath library file. */
/*
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.
    Copyright (C) 2000 Philip A. Nelson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.  (LICENSE)

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/

#include "bcmath.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>


/* Raise NUM1 to the NUM2 power.  The result is placed in RESULT.
   Maximum exponent is LONG_MAX.  If a NUM2 is not an integer,
   only the integer part is used.  */

void bc_raise(bc_num num1, long exponent, bc_num *result, size_t scale)
{
	bc_num temp, power;
	size_t rscale;
	size_t pwrscale;
	size_t calcscale;
	bool is_neg;

	/* Special case if exponent is a zero. */
	if (exponent == 0) {
		bc_free_num (result);
		*result = bc_copy_num(BCG(_one_));
		return;
	}

	/* Other initializations. */
	if (exponent < 0) {
		is_neg = true;
		exponent = -exponent;
		rscale = scale;
	} else {
		is_neg = false;
		rscale = MIN (num1->n_scale * exponent, MAX(scale, num1->n_scale));
	}

	/* Set initial value of temp. */
	power = bc_copy_num(num1);
	pwrscale = num1->n_scale;
	while ((exponent & 1) == 0) {
		pwrscale = 2 * pwrscale;
		bc_multiply(power, power, &power, pwrscale);
		exponent = exponent >> 1;
	}
	temp = bc_copy_num(power);
	calcscale = pwrscale;
	exponent = exponent >> 1;

	/* Do the calculation. */
	while (exponent > 0) {
		pwrscale = 2 * pwrscale;
		bc_multiply(power, power, &power, pwrscale);
		if ((exponent & 1) == 1) {
			calcscale = pwrscale + calcscale;
			bc_multiply(temp, power, &temp, calcscale);
		}
		exponent = exponent >> 1;
	}

	/* Assign the value. */
	if (is_neg) {
		bc_divide(BCG(_one_), temp, result, rscale);
		bc_free_num (&temp);
	} else {
		bc_free_num (result);
		*result = temp;
		if ((*result)->n_scale > rscale) {
			(*result)->n_scale = rscale;
		}
	}
	bc_free_num (&power);
}

/* This is used internally by BCMath */
void bc_raise_bc_exponent(bc_num base, bc_num expo, bc_num *result, size_t scale)
{
	/* Exponent must not have fractional part */
	assert(expo->n_scale == 0);

	long exponent = bc_num2long(expo);
	/* Exponent must be properly convertable to long */
	if (exponent == 0 && (expo->n_len > 1 || expo->n_value[0] != 0)) {
		assert(false && "Exponent is not well formed in internal call");
		//assert(exponent != 0 || (expo->n_len == 0 && expo->n_value[0] == 0));
	}
	//assert(exponent != 0 || (expo->n_len == 0 && expo->n_value[0] == 0));
	bc_raise(base, exponent, result, scale);
}

