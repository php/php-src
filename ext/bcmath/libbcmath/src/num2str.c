/* num2str.c: bcmath library file. */
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

/* Convert a numbers to a string.  Base 10 only.*/

char
*bc_num2str (num)
      bc_num num;
{
  char *str, *sptr;
  char *nptr;
  int  index, signch;

  /* Allocate the string memory. */
  signch = ( num->n_sign == PLUS ? 0 : 1 );  /* Number of sign chars. */
  if (num->n_scale > 0)
    str = (char *) safe_emalloc (1, num->n_len + num->n_scale, 2 + signch);
  else
    str = (char *) safe_emalloc (1, num->n_len, 1 + signch);
  if (str == NULL) bc_out_of_memory();

  /* The negative sign if needed. */
  sptr = str;
  if (signch) *sptr++ = '-';

  /* Load the whole number. */
  nptr = num->n_value;
  for (index=num->n_len; index>0; index--)
    *sptr++ = BCD_CHAR(*nptr++);

  /* Now the fraction. */
  if (num->n_scale > 0)
    {
      *sptr++ = '.';
      for (index=0; index<num->n_scale; index++)
	*sptr++ = BCD_CHAR(*nptr++);
    }

  /* Terminate the string and return it! */
  *sptr = '\0';
  return (str);
}
