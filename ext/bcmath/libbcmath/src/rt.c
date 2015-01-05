/* rt.c: bcmath library file. */
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


void bc_rt_warn (char *mesg ,...)
{
  va_list args;
  char error_mesg [255];

  va_start (args, mesg);
  vsnprintf (error_mesg, sizeof(error_mesg), mesg, args);
  va_end (args);

  fprintf (stderr, "bc math warning: %s\n", error_mesg);
}


void bc_rt_error (char *mesg ,...)
{
  va_list args;
  char error_mesg [255];

  va_start (args, mesg);
  vsnprintf (error_mesg, sizeof(error_mesg), mesg, args);
  va_end (args);

  fprintf (stderr, "bc math error: %s\n", error_mesg);
}
