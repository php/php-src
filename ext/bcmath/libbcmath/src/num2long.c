/* num2long.c: bcmath library file. */
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

/* Convert a number NUM to a long.  The function returns only the integer
   part of the number.  For numbers that are too large to represent as
   a long, this function returns a zero.  This can be detected by checking
   the NUM for zero after having a zero returned. */

long
bc_num2long (num)
     bc_num num;
{
  long val;
  char *nptr;
  int  index;

  /* Extract the int value, ignore the fraction. */
  val = 0;
  nptr = num->n_value;
  for (index=num->n_len; (index>0) && (val<=(LONG_MAX/BASE)); index--)
    val = val*BASE + *nptr++;

  /* Check for overflow.  If overflow, return zero. */
  if (index>0) val = 0;
  if (val < 0) val = 0;

  /* Return the value. */
  if (num->n_sign == PLUS)
    return (val);
  else
    return (-val);
}

