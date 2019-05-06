/* debug.c: bcmath library file. */
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

/* pn prints the number NUM in base 10. */

static void
out_char (int c)
{
  putchar(c);
}


void
pn (bc_num num)
{
  bc_out_num (num, 10, out_char, 0);
  out_char ('\n');
}


/* pv prints a character array as if it was a string of bcd digits. */
void
pv (name, num, len)
     char *name;
     unsigned char *num;
     int len;
{
  int i;
  printf ("%s=", name);
  for (i=0; i<len; i++) printf ("%c",BCD_CHAR(num[i]));
  printf ("\n");
}
