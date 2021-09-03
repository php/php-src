/* raisemod.c: bcmath library file. */
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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "bcmath.h"
#include "private.h"
#include "zend_exceptions.h"

/* Raise BASE to the EXPO power, reduced modulo MOD.  The result is placed in RESULT. */
zend_result bc_raisemod (bc_num base, bc_num expo, bc_num mod, bc_num *result, int scale)
{
  bc_num power, exponent, modulus, parity, temp;
  int rscale;

	/* Check the base for scale digits. */
	if (base->n_scale != 0) {
		/* 1st argument from PHP_FUNCTION(bcpowmod) */
		zend_argument_value_error(1, "cannot have a fractional part");
		return FAILURE;
    }
	/* Check the exponent for scale digits. */
	if (expo->n_scale != 0) {
		/* 2nd argument from PHP_FUNCTION(bcpowmod) */
		zend_argument_value_error(2, "cannot have a fractional part");
		return FAILURE;
    }
	if (bc_is_neg(expo)) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		return FAILURE;
	}
	/* Check the modulus for scale digits. */
	if (mod->n_scale != 0) {
		/* 3rd argument from PHP_FUNCTION(bcpowmod) */
		zend_argument_value_error(3, "cannot have a fractional part");
		return FAILURE;
    }
    /* Modulus cannot be 0 */
	if (bc_is_zero(mod)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		return FAILURE;
	}

  /* Set initial values.  */
  power = bc_copy_num (base);
  exponent = bc_copy_num (expo);
  modulus = bc_copy_num (mod);
  temp = bc_copy_num (BCG(_one_));
  bc_init_num(&parity);

  /* Do the calculation. */
  rscale = MAX(scale, power->n_scale);
  if ( !bc_compare(modulus, BCG(_one_)) )
    {
      bc_free_num (&temp);
      temp = bc_new_num (1, scale);
    }
  else
    {
      while ( !bc_is_zero(exponent) )
	{
	  (void) bc_divmod (exponent, BCG(_two_), &exponent, &parity, 0);
	  if ( !bc_is_zero(parity) )
	    {
	      bc_multiply (temp, power, &temp, rscale);
	      (void) bc_modulo (temp, modulus, &temp, scale);
	    }

	  bc_multiply (power, power, &power, rscale);
	  (void) bc_modulo (power, modulus, &power, scale);
	}
    }

  /* Assign the value. */
  bc_free_num (&power);
  bc_free_num (&exponent);
  bc_free_num (&modulus);
  bc_free_num (result);
  bc_free_num (&parity);
  *result = temp;
  return SUCCESS;	/* Everything is OK. */
}
