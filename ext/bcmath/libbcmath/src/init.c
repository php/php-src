/* init.c: bcmath library file. */
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

 bc_num _bc_Free_list = NULL;

/* new_num allocates a number and sets fields to known values. */

bc_num
bc_new_num (length, scale)
     int length, scale;
{
  bc_num temp;

  /* PHP Change:  malloc() -> emalloc(), removed free_list code */
  temp = (bc_num) emalloc (sizeof(bc_struct)+length+scale);
#if 0
  if (_bc_Free_list != NULL) {
    temp = _bc_Free_list;
    _bc_Free_list = temp->n_next;
  } else {
    temp = (bc_num) emalloc (sizeof(bc_struct));
    if (temp == NULL) bc_out_of_memory ();
  }
#endif
  temp->n_sign = PLUS;
  temp->n_len = length;
  temp->n_scale = scale;
  temp->n_refs = 1;
  /* PHP Change:  malloc() -> emalloc() */
  temp->n_ptr = (char *) emalloc (length+scale);
  if (temp->n_ptr == NULL) bc_out_of_memory();
  temp->n_value = temp->n_ptr;
  memset (temp->n_ptr, 0, length+scale);
  return temp;
}


/* "Frees" a bc_num NUM.  Actually decreases reference count and only
   frees the storage if reference count is zero. */

void
bc_free_num (num)
    bc_num *num;
{
  if (*num == NULL) return;
  (*num)->n_refs--;
  if ((*num)->n_refs == 0) {
    if ((*num)->n_ptr)
		/* PHP Change:  free() -> efree(), removed free_list code */
      efree ((*num)->n_ptr);
	efree(*num);
#if 0
    (*num)->n_next = _bc_Free_list;
    _bc_Free_list = *num;
#endif
  }
  *num = NULL;
}


/* Intitialize the number package! */

void
bc_init_numbers (TSRMLS_D)
{
  BCG(_zero_) = bc_new_num (1,0);
  BCG(_one_)  = bc_new_num (1,0);
  BCG(_one_)->n_value[0] = 1;
  BCG(_two_)  = bc_new_num (1,0);
  BCG(_two_)->n_value[0] = 2;
}


/* Make a copy of a number!  Just increments the reference count! */

bc_num
bc_copy_num (bc_num num)
{
  num->n_refs++;
  return num;
}


/* Initialize a number NUM by making it a copy of zero. */

void
bc_init_num (bc_num *num TSRMLS_DC)
{
  *num = bc_copy_num (BCG(_zero_));
}

