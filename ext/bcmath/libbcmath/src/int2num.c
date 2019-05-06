/* int2num.c: bcmath library file. */
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


/* Convert an integer VAL to a bc number NUM. */

void
bc_int2num (num, val)
     bc_num *num;
     int val;
{
  char buffer[30];
  char *bptr, *vptr;
  int  ix = 1;
  char neg = 0;

  /* Sign. */
  if (val < 0)
    {
      neg = 1;
      val = -val;
    }

  /* Get things going. */
  bptr = buffer;
  *bptr++ = val % BASE;
  val = val / BASE;

  /* Extract remaining digits. */
  while (val != 0)
    {
      *bptr++ = val % BASE;
      val = val / BASE;
      ix++; 		/* Count the digits. */
    }

  /* Make the number. */
  bc_free_num (num);
  *num = bc_new_num (ix, 0);
  if (neg) (*num)->n_sign = MINUS;

  /* Assign the digits. */
  vptr = (*num)->n_value;
  while (ix-- > 0)
    *vptr++ = *--bptr;
}
