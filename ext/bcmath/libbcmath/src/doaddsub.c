/* doaddsub.c: bcmath library file. */
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


/* Perform addition: N1 is added to N2 and the value is
   returned.  The signs of N1 and N2 are ignored.
   SCALE_MIN is to set the minimum scale of the result. */

 bc_num
_bc_do_add (n1, n2, scale_min)
     bc_num n1, n2;
     int scale_min;
{
  bc_num sum;
  int sum_scale, sum_digits;
  char *n1ptr, *n2ptr, *sumptr;
  int carry, n1bytes, n2bytes;
  int count;

  /* Prepare sum. */
  sum_scale = MAX (n1->n_scale, n2->n_scale);
  sum_digits = MAX (n1->n_len, n2->n_len) + 1;
  sum = bc_new_num (sum_digits, MAX(sum_scale, scale_min));

  /* Zero extra digits made by scale_min. */
  if (scale_min > sum_scale)
    {
      sumptr = (char *) (sum->n_value + sum_scale + sum_digits);
      for (count = scale_min - sum_scale; count > 0; count--)
	*sumptr++ = 0;
    }

  /* Start with the fraction part.  Initialize the pointers. */
  n1bytes = n1->n_scale;
  n2bytes = n2->n_scale;
  n1ptr = (char *) (n1->n_value + n1->n_len + n1bytes - 1);
  n2ptr = (char *) (n2->n_value + n2->n_len + n2bytes - 1);
  sumptr = (char *) (sum->n_value + sum_scale + sum_digits - 1);

  /* Add the fraction part.  First copy the longer fraction.*/
  if (n1bytes != n2bytes)
    {
      if (n1bytes > n2bytes)
	while (n1bytes>n2bytes)
	  { *sumptr-- = *n1ptr--; n1bytes--;}
      else
	while (n2bytes>n1bytes)
	  { *sumptr-- = *n2ptr--; n2bytes--;}
    }

  /* Now add the remaining fraction part and equal size integer parts. */
  n1bytes += n1->n_len;
  n2bytes += n2->n_len;
  carry = 0;
  while ((n1bytes > 0) && (n2bytes > 0))
    {
      *sumptr = *n1ptr-- + *n2ptr-- + carry;
      if (*sumptr > (BASE-1))
	{
	   carry = 1;
	   *sumptr -= BASE;
	}
      else
	carry = 0;
      sumptr--;
      n1bytes--;
      n2bytes--;
    }

  /* Now add carry the longer integer part. */
  if (n1bytes == 0)
    { n1bytes = n2bytes; n1ptr = n2ptr; }
  while (n1bytes-- > 0)
    {
      *sumptr = *n1ptr-- + carry;
      if (*sumptr > (BASE-1))
	{
	   carry = 1;
	   *sumptr -= BASE;
	 }
      else
	carry = 0;
      sumptr--;
    }

  /* Set final carry. */
  if (carry == 1)
    *sumptr += 1;

  /* Adjust sum and return. */
  _bc_rm_leading_zeros (sum);
  return sum;
}


/* Perform subtraction: N2 is subtracted from N1 and the value is
   returned.  The signs of N1 and N2 are ignored.  Also, N1 is
   assumed to be larger than N2.  SCALE_MIN is the minimum scale
   of the result. */

 bc_num
_bc_do_sub (n1, n2, scale_min)
     bc_num n1, n2;
     int scale_min;
{
  bc_num diff;
  int diff_scale, diff_len;
  int min_scale, min_len;
  char *n1ptr, *n2ptr, *diffptr;
  int borrow, count, val;

  /* Allocate temporary storage. */
  diff_len = MAX (n1->n_len, n2->n_len);
  diff_scale = MAX (n1->n_scale, n2->n_scale);
  min_len = MIN  (n1->n_len, n2->n_len);
  min_scale = MIN (n1->n_scale, n2->n_scale);
  diff = bc_new_num (diff_len, MAX(diff_scale, scale_min));

  /* Zero extra digits made by scale_min. */
  if (scale_min > diff_scale)
    {
      diffptr = (char *) (diff->n_value + diff_len + diff_scale);
      for (count = scale_min - diff_scale; count > 0; count--)
	*diffptr++ = 0;
    }

  /* Initialize the subtract. */
  n1ptr = (char *) (n1->n_value + n1->n_len + n1->n_scale -1);
  n2ptr = (char *) (n2->n_value + n2->n_len + n2->n_scale -1);
  diffptr = (char *) (diff->n_value + diff_len + diff_scale -1);

  /* Subtract the numbers. */
  borrow = 0;

  /* Take care of the longer scaled number. */
  if (n1->n_scale != min_scale)
    {
      /* n1 has the longer scale */
      for (count = n1->n_scale - min_scale; count > 0; count--)
	*diffptr-- = *n1ptr--;
    }
  else
    {
      /* n2 has the longer scale */
      for (count = n2->n_scale - min_scale; count > 0; count--)
	{
	  val = - *n2ptr-- - borrow;
	  if (val < 0)
	    {
	      val += BASE;
	      borrow = 1;
	    }
	  else
	    borrow = 0;
	  *diffptr-- = val;
	}
    }

  /* Now do the equal length scale and integer parts. */

  for (count = 0; count < min_len + min_scale; count++)
    {
      val = *n1ptr-- - *n2ptr-- - borrow;
      if (val < 0)
	{
	  val += BASE;
	  borrow = 1;
	}
      else
	borrow = 0;
      *diffptr-- = val;
    }

  /* If n1 has more digits then n2, we now do that subtract. */
  if (diff_len != min_len)
    {
      for (count = diff_len - min_len; count > 0; count--)
	{
	  val = *n1ptr-- - borrow;
	  if (val < 0)
	    {
	      val += BASE;
	      borrow = 1;
	    }
	  else
	    borrow = 0;
	  *diffptr-- = val;
	}
    }

  /* Clean up and return. */
  _bc_rm_leading_zeros (diff);
  return diff;
}

