/* output.c: bcmath library file. */
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


/* The following routines provide output for bcd numbers package
   using the rules of POSIX bc for output. */

/* This structure is used for saving digits in the conversion process. */
typedef struct stk_rec {
	long  digit;
	struct stk_rec *next;
} stk_rec;

/* The reference string for digits. */
static char ref_str[] = "0123456789ABCDEF";


/* A special output routine for "multi-character digits."  Exactly
   SIZE characters must be output for the value VAL.  If SPACE is
   non-zero, we must output one space before the number.  OUT_CHAR
   is the actual routine for writing the characters. */

void
bc_out_long (val, size, space, out_char)
     long val;
     int size, space;
#ifdef __STDC__
     void (*out_char)(int);
#else
     void (*out_char)();
#endif
{
  char digits[40];
  int len, ix;

  if (space) (*out_char) (' ');
  snprintf(digits, sizeof(digits), "%ld", val);
  len = strlen (digits);
  while (size > len)
    {
      (*out_char) ('0');
      size--;
    }
  for (ix=0; ix < len; ix++)
    (*out_char) (digits[ix]);
}

/* Output of a bcd number.  NUM is written in base O_BASE using OUT_CHAR
   as the routine to do the actual output of the characters. */

void
#ifdef __STDC__
bc_out_num (bc_num num, int o_base, void (*out_char)(int), int leading_zero)
#else
bc_out_num (bc_num num, int o_base, void (*out_char)(), int leading_zero)
#endif
{
  char *nptr;
  int  index, fdigit, pre_space;
  stk_rec *digits, *temp;
  bc_num int_part, frac_part, base, cur_dig, t_num, max_o_digit;

  /* The negative sign if needed. */
  if (num->n_sign == MINUS) (*out_char) ('-');

  /* Output the number. */
  if (bc_is_zero (num))
    (*out_char) ('0');
  else
    if (o_base == 10)
      {
	/* The number is in base 10, do it the fast way. */
	nptr = num->n_value;
	if (num->n_len > 1 || *nptr != 0)
	  for (index=num->n_len; index>0; index--)
	    (*out_char) (BCD_CHAR(*nptr++));
	else
	  nptr++;

	if (leading_zero && bc_is_zero (num))
	  (*out_char) ('0');

	/* Now the fraction. */
	if (num->n_scale > 0)
	  {
	    (*out_char) ('.');
	    for (index=0; index<num->n_scale; index++)
	      (*out_char) (BCD_CHAR(*nptr++));
	  }
      }
    else
      {
	/* special case ... */
	if (leading_zero && bc_is_zero (num))
	  (*out_char) ('0');

	/* The number is some other base. */
	digits = NULL;
	bc_init_num (&int_part);
	bc_divide (num, BCG(_one_), &int_part, 0);
	bc_init_num (&frac_part);
	bc_init_num (&cur_dig);
	bc_init_num (&base);
	bc_sub (num, int_part, &frac_part, 0);
	/* Make the INT_PART and FRAC_PART positive. */
	int_part->n_sign = PLUS;
	frac_part->n_sign = PLUS;
	bc_int2num (&base, o_base);
	bc_init_num (&max_o_digit);
	bc_int2num (&max_o_digit, o_base-1);


	/* Get the digits of the integer part and push them on a stack. */
	while (!bc_is_zero (int_part))
	  {
	    bc_modulo (int_part, base, &cur_dig, 0);
		/* PHP Change:  malloc() -> emalloc() */
	    temp = (stk_rec *) emalloc (sizeof(stk_rec));
	    temp->digit = bc_num2long (cur_dig);
	    temp->next = digits;
	    digits = temp;
	    bc_divide (int_part, base, &int_part, 0);
	  }

	/* Print the digits on the stack. */
	if (digits != NULL)
	  {
	    /* Output the digits. */
	    while (digits != NULL)
	      {
		temp = digits;
		digits = digits->next;
		if (o_base <= 16)
		  (*out_char) (ref_str[ (int) temp->digit]);
		else
		  bc_out_long (temp->digit, max_o_digit->n_len, 1, out_char);
		efree (temp);
	      }
	  }

	/* Get and print the digits of the fraction part. */
	if (num->n_scale > 0)
	  {
	    (*out_char) ('.');
	    pre_space = 0;
	    t_num = bc_copy_num (BCG(_one_));
	    while (t_num->n_len <= num->n_scale) {
	      bc_multiply (frac_part, base, &frac_part, num->n_scale);
	      fdigit = bc_num2long (frac_part);
	      bc_int2num (&int_part, fdigit);
	      bc_sub (frac_part, int_part, &frac_part, 0);
	      if (o_base <= 16)
		(*out_char) (ref_str[fdigit]);
	      else {
		bc_out_long (fdigit, max_o_digit->n_len, pre_space, out_char);
		pre_space = 1;
	      }
	      bc_multiply (t_num, base, &t_num, 0);
	    }
	    bc_free_num (&t_num);
	  }

	/* Clean up. */
	bc_free_num (&int_part);
	bc_free_num (&frac_part);
	bc_free_num (&base);
	bc_free_num (&cur_dig);
	bc_free_num (&max_o_digit);
      }
}
