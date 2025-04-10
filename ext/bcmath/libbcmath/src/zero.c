/* zero.c: bcmath library file. */
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
#include <stdbool.h>
#include <stddef.h>

/* In some places we need to check if the number NUM is zero. */

bool bc_is_zero_for_scale(bc_num num, size_t scale)
{
	size_t count;
	char *nptr;

	/* Quick check. */
	if (num == BCG(_zero_)) {
		return true;
	}

	/* Initialize */
	count = num->n_len + scale;
	nptr = num->n_value;

	/* The check */
	while ((count > 0) && (*nptr++ == 0)) count--;

	return count == 0;
}

bool bc_is_zero(bc_num num)
{
	return bc_is_zero_for_scale(num, num->n_scale);
}
