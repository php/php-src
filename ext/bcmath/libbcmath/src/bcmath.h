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

#ifndef _BCMATH_H_
#define _BCMATH_H_

#include <stddef.h>

typedef enum {PLUS, MINUS} sign;

typedef struct bc_struct *bc_num;

typedef struct bc_struct {
	size_t        n_len;   /* The number of digits before the decimal point. */
	size_t        n_scale; /* The number of digits after the decimal point. */
	char         *n_value; /* The number. Not zero char terminated. */
	unsigned int  n_refs;  /* The number of pointers to this number. */
	sign          n_sign;
} bc_struct;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zend.h"
#include <stdbool.h>
#include "zend_string.h"

/* Needed for BCG() macro and PHP_ROUND_XXX */
#include "../../php_bcmath.h"

/* The base used in storing the numbers in n_value above.
   Currently, this MUST be 10. */

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

#ifndef LONG_MAX
#define LONG_MAX 0x7fffffff
#endif


/* Function Prototypes */

void bc_init_numbers(void);

bc_num _bc_new_num_ex(size_t length, size_t scale, bool persistent);

bc_num _bc_new_num_nonzeroed_ex(size_t length, size_t scale, bool persistent);

void _bc_free_num_ex(bc_num *num, bool persistent);

/* Make a copy of a number! Just increments the reference count! */
static inline bc_num bc_copy_num(bc_num num)
{
	num->n_refs++;
	return num;
}

void bc_init_num(bc_num *num);

bool bc_str2num(bc_num *num, const char *str, const char *end, size_t scale, size_t *full_scale, bool auto_scale);

bc_num bc_long2num(zend_long lval);

zend_string *bc_num2str_ex(bc_num num, size_t scale);

void bc_int2num(bc_num *num, int val);

long bc_num2long(bc_num num);

typedef enum {
	BCMATH_EQUAL = 0,
	BCMATH_LEFT_GREATER = 1,
	BCMATH_RIGHT_GREATER = -1
} bcmath_compare_result;

bcmath_compare_result bc_compare(bc_num n1, bc_num n2, size_t scale);

bool bc_is_zero(bc_num num);

bool bc_is_zero_for_scale(bc_num num, size_t scale);

bool bc_is_near_zero(bc_num num, size_t scale);

bool bc_is_neg(bc_num num);

void bc_rm_trailing_zeros(bc_num num);

bc_num bc_add(bc_num n1, bc_num n2, size_t scale_min);

#define bc_add_ex(n1, n2, result, scale_min) do {	\
	bc_num add_ex = bc_add(n1, n2, scale_min);		\
	bc_free_num (result);                           \
	*(result) = add_ex;                             \
} while (0)

bc_num bc_sub(bc_num n1, bc_num n2, size_t scale_min);

#define bc_sub_ex(n1, n2, result, scale_min) do {	\
	bc_num sub_ex = bc_sub(n1, n2, scale_min);		\
	bc_free_num (result);                           \
	*(result) = sub_ex;                             \
} while (0)

bc_num bc_multiply(bc_num n1, bc_num n2, size_t scale);

#define bc_multiply_ex(n1, n2, result, scale_min) do {	\
	bc_num mul_ex = bc_multiply(n1, n2, scale_min); \
	bc_free_num (result);                           \
	*(result) = mul_ex;                             \
} while (0)

bc_num bc_square(bc_num n1, size_t scale);

bool bc_divide(bc_num n1, bc_num n2, bc_num *quot, size_t scale);

bool bc_modulo(bc_num num1, bc_num num2, bc_num *resul, size_t scale);

bool bc_divmod(bc_num num1, bc_num num2, bc_num *quo, bc_num *rem, size_t scale);

bc_num bc_floor_or_ceil(bc_num num, bool is_floor);

void bc_round(bc_num num, zend_long places, zend_long mode, bc_num *result);

typedef enum {
	OK,
	BASE_HAS_FRACTIONAL,
	EXPO_HAS_FRACTIONAL,
	EXPO_IS_NEGATIVE,
	MOD_HAS_FRACTIONAL,
	MOD_IS_ZERO
} raise_mod_status;

raise_mod_status bc_raisemod(bc_num base, bc_num exponent, bc_num mod, bc_num *result, size_t scale);

bool bc_raise(bc_num base, long exponent, bc_num *result, size_t scale);

void bc_raise_bc_exponent(bc_num base, bc_num exponent, bc_num *resul, size_t scale);

bool bc_sqrt(bc_num *num, size_t scale);

/* Prototypes needed for external utility routines. */
#define bc_new_num(length, scale)			_bc_new_num_ex((length), (scale), 0)
#define bc_new_num_nonzeroed(length, scale)	_bc_new_num_nonzeroed_ex((length), (scale), 0)
#define bc_free_num(num)					_bc_free_num_ex((num), 0)
#define bc_num2str(num)						bc_num2str_ex((num), (num->n_scale))

#endif
