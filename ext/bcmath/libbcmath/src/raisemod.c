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


/* Truncate a number to zero scale.  To avoid sharing issues (refcount and
   shared n_value) the number is copied, this copy is truncated, and the
   original number is "freed". */

static void
_bc_truncate (bc_num *num)
{
  bc_num temp;

  temp = bc_new_num ((*num)->n_len, 0);
  temp->n_sign = (*num)->n_sign;
  memcpy (temp->n_value, (*num)->n_value, (*num)->n_len);
  bc_free_num (num);
  *num = temp;
}


/* Raise BASE to the EXPO power, reduced modulo MOD.  The result is
   placed in RESULT.  If a EXPO is not an integer,
   only the integer part is used.  */

int
bc_raisemod (bc_num base, bc_num expo, bc_num mod, bc_num *result, int scale)
{
  bc_num power, exponent, modulus, parity, temp;
  int rscale;

  /* Check for correct numbers. */
  if (bc_is_zero(mod)) return -1;
  if (bc_is_neg(expo)) return -1;

  /* Set initial values.  */
  power = bc_copy_num (base);
  exponent = bc_copy_num (expo);
  modulus = bc_copy_num (mod);
  temp = bc_copy_num (BCG(_one_));
  bc_init_num(&parity);

  /* Check the base for scale digits. */
  if (power->n_scale != 0)
    {
      php_error_docref (NULL, E_WARNING, "non-zero scale in base");
      _bc_truncate (&power);
    }

  /* Check the exponent for scale digits. */
  if (exponent->n_scale != 0)
    {
      php_error_docref (NULL, E_WARNING, "non-zero scale in exponent");
      _bc_truncate (&exponent);
    }

  /* Check the modulus for scale digits. */
  if (modulus->n_scale != 0)
    {
      php_error_docref (NULL, E_WARNING, "non-zero scale in modulus");
      _bc_truncate (&modulus);
    }

  /* Do the calculation. */
  rscale = MAX(scale, power->n_scale);
  if ( !bc_compare(modulus, BCG(_one_)) )
    {
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
  return 0;	/* Everything is OK. */
}
