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
    Lesser General Public License for more details.  (COPYING.LIB)

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
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "bcmath.h"
#include "private.h"

/* Raise BASE to the EXPO power, reduced modulo MOD.  The result is
   placed in RESULT.  If a EXPO is not an integer,
   only the integer part is used.  */

int
bc_raisemod (bc_num base, bc_num expo, bc_num mod, bc_num *result, int scale)
{
  bc_num power, exponent, parity, temp;
  int rscale;

  /* Check for correct numbers. */
  if (bc_is_zero(mod)) return -1;
  if (bc_is_neg(expo)) return -1;

  /* Set initial values.  */
  power = bc_copy_num (base);
  exponent = bc_copy_num (expo);
  temp = bc_copy_num (BCG(_one_));
  bc_init_num(&parity);

  /* Check the base for scale digits. */
  if (base->n_scale != 0)
      bc_rt_warn ("non-zero scale in base");

  /* Check the exponent for scale digits. */
  if (exponent->n_scale != 0)
    {
      bc_rt_warn ("non-zero scale in exponent");
      bc_divide (exponent, BCG(_one_), &exponent, 0); /*truncate */
    }

  /* Check the modulus for scale digits. */
  if (mod->n_scale != 0)
      bc_rt_warn ("non-zero scale in modulus");

  /* Do the calculation. */
  rscale = MAX(scale, base->n_scale);
  while ( !bc_is_zero(exponent) )
    {
      (void) bc_divmod (exponent, BCG(_two_), &exponent, &parity, 0);
      if ( !bc_is_zero(parity) )
	{
	  bc_multiply (temp, power, &temp, rscale);
	  (void) bc_modulo (temp, mod, &temp, scale);
	}

      bc_multiply (power, power, &power, rscale);
      (void) bc_modulo (power, mod, &power, scale);
    }

  /* Assign the value. */
  bc_free_num (&power);
  bc_free_num (&exponent);
  bc_free_num (result);
  bc_free_num (&parity);
  *result = temp;
  return 0;	/* Everything is OK. */
}
