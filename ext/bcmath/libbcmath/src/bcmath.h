/* bcmath.h: bcmath library header.    	*/
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

#ifndef _BCMATH_H_
#define _BCMATH_H_

typedef enum {PLUS, MINUS} sign;

typedef struct bc_struct *bc_num;

typedef struct bc_struct
    {
      sign  n_sign;
      int   n_len;	/* The number of digits before the decimal point. */
      int   n_scale;	/* The number of digits after the decimal point. */
      int   n_refs;     /* The number of pointers to this number. */
      bc_num n_next;	/* Linked list for available list. */
      char *n_ptr;	/* The pointer to the actual storage.
			   If NULL, n_value points to the inside of
			   another number (bc_multiply...) and should
			   not be "freed." */
      char *n_value;	/* The number. Not zero char terminated.
			   May not point to the same place as n_ptr as
			   in the case of leading zeros generated. */
    } bc_struct;


/* The base used in storing the numbers in n_value above.
   Currently this MUST be 10. */

#define BASE 10

/*  Some useful macros and constants. */

#define CH_VAL(c)     (c - '0')
#define BCD_CHAR(d)   (d + '0')

#ifdef MIN
#undef MIN
#undef MAX
#endif
#define MAX(a, b)      ((a)>(b)?(a):(b))
#define MIN(a, b)      ((a)>(b)?(b):(a))
#define ODD(a)        ((a)&1)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef LONG_MAX
#define LONG_MAX 0x7ffffff
#endif


/* Global numbers. */
extern bc_num _zero_;
extern bc_num _one_;
extern bc_num _two_;


/* Function Prototypes */

/* Define the _PROTOTYPE macro if it is needed. */

#ifndef _PROTOTYPE
#ifdef __STDC__
#define _PROTOTYPE(func, args) func args
#else
#define _PROTOTYPE(func, args) func()
#endif
#endif

_PROTOTYPE(void bc_init_numbers, (void));

_PROTOTYPE(bc_num bc_new_num, (int length, int scale));

_PROTOTYPE(void bc_free_num, (bc_num *num));

_PROTOTYPE(bc_num bc_copy_num, (bc_num num));

_PROTOTYPE(void bc_init_num, (bc_num *num));

_PROTOTYPE(void bc_str2num, (bc_num *num, char *str, int scale));

_PROTOTYPE(char *bc_num2str, (bc_num num));

_PROTOTYPE(void bc_int2num, (bc_num *num, int val));

_PROTOTYPE(long bc_num2long, (bc_num num));

_PROTOTYPE(int bc_compare, (bc_num n1, bc_num n2));

_PROTOTYPE(char bc_is_zero, (bc_num num));

_PROTOTYPE(char bc_is_near_zero, (bc_num num, int scale));

_PROTOTYPE(char bc_is_neg, (bc_num num));

_PROTOTYPE(void bc_add, (bc_num n1, bc_num n2, bc_num *result, int scale_min));

_PROTOTYPE(void bc_sub, (bc_num n1, bc_num n2, bc_num *result, int scale_min));

_PROTOTYPE(void bc_multiply, (bc_num n1, bc_num n2, bc_num *prod, int scale));

_PROTOTYPE(int bc_divide, (bc_num n1, bc_num n2, bc_num *quot, int scale));

_PROTOTYPE(int bc_modulo, (bc_num num1, bc_num num2, bc_num *result,
			   int scale));

_PROTOTYPE(int bc_divmod, (bc_num num1, bc_num num2, bc_num *quot,
			   bc_num *rem, int scale));

_PROTOTYPE(int bc_raisemod, (bc_num base, bc_num expo, bc_num mod,
			     bc_num *result, int scale));

_PROTOTYPE(void bc_raise, (bc_num num1, bc_num num2, bc_num *result,
			   int scale));

_PROTOTYPE(int bc_sqrt, (bc_num *num, int scale));

_PROTOTYPE(void bc_out_num, (bc_num num, int o_base, void (* out_char)(int),
			     int leading_zero));

/* Prototypes needed for external utility routines. */

_PROTOTYPE(void bc_rt_warn, (char *mesg ,...));
_PROTOTYPE(void bc_rt_error, (char *mesg ,...));
_PROTOTYPE(void bc_out_of_memory, (void));
#endif
