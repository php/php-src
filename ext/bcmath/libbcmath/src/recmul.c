/* recmul.c: bcmath library file. */
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
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include "private.h" /* For _bc_rm_leading_zeros() */
#include "zend_alloc.h"

/* Recursive vs non-recursive multiply crossover ranges. */
#if defined(MULDIGITS)
#include "muldigits.h"
#else
#define MUL_BASE_DIGITS 80
#endif

int mul_base_digits = MUL_BASE_DIGITS;
#define MUL_SMALL_DIGITS mul_base_digits/4

/* Multiply utility routines */

static bc_num new_sub_num(size_t length, size_t scale, char *value)
{
	bc_num temp = (bc_num) emalloc(sizeof(bc_struct));

	temp->n_sign = PLUS;
	temp->n_len = length;
	temp->n_scale = scale;
	temp->n_refs = 1;
	temp->n_ptr = NULL;
	temp->n_value = value;
	return temp;
}

static void _bc_simp_mul(bc_num n1, size_t n1len, bc_num n2, int n2len, bc_num *prod)
{
	char *n1ptr, *n2ptr, *pvptr;
	char *n1end, *n2end;        /* To the end of n1 and n2. */
	int sum = 0;

	int prodlen = n1len + n2len + 1;

	*prod = bc_new_num (prodlen, 0);

	n1end = (char *) (n1->n_value + n1len - 1);
	n2end = (char *) (n2->n_value + n2len - 1);
	pvptr = (char *) ((*prod)->n_value + prodlen - 1);

	/* Here is the loop... */
	for (int index = 0; index < prodlen - 1; index++) {
		n1ptr = (char *) (n1end - MAX(0, index - n2len + 1));
		n2ptr = (char *) (n2end - MIN(index, n2len - 1));
		while ((n1ptr >= n1->n_value) && (n2ptr <= n2end)) {
			sum += *n1ptr * *n2ptr;
			n1ptr--;
			n2ptr++;
		}
		*pvptr-- = sum % BASE;
		sum = sum / BASE;
	}
	*pvptr = sum;
}


/* A special adder/subtractor for the recursive divide and conquer
   multiply algorithm.  Note: if sub is called, accum must
   be larger that what is being subtracted.  Also, accum and val
   must have n_scale = 0.  (e.g. they must look like integers. *) */
static void _bc_shift_addsub(bc_num accum, bc_num val, int shift, bool sub)
{
	signed char *accp, *valp;
	unsigned int carry = 0;
	size_t count = val->n_len;

	if (val->n_value[0] == 0) {
		count--;
	}
	assert(accum->n_len + accum->n_scale >= shift + count);

	/* Set up pointers and others */
	accp = (signed char *) (accum->n_value + accum->n_len + accum->n_scale - shift - 1);
	valp = (signed char *) (val->n_value + val->n_len - 1);

	if (sub) {
		/* Subtraction, carry is really borrow. */
		while (count--) {
			*accp -= *valp-- + carry;
			if (*accp < 0) {
				carry = 1;
				*accp-- += BASE;
			} else {
				carry = 0;
				accp--;
			}
		}
		while (carry) {
			*accp -= carry;
			if (*accp < 0) {
				*accp-- += BASE;
			} else {
				carry = 0;
			}
		}
	} else {
		/* Addition */
		while (count--) {
			*accp += *valp-- + carry;
			if (*accp > (BASE - 1)) {
				carry = 1;
				*accp-- -= BASE;
			} else {
				carry = 0;
				accp--;
			}
		}
		while (carry) {
			*accp += carry;
			if (*accp > (BASE - 1)) {
				*accp-- -= BASE;
			} else {
				carry = 0;
			}
		}
	}
}

/* Recursive divide and conquer multiply algorithm.
   Based on
   Let u = u0 + u1*(b^n)
   Let v = v0 + v1*(b^n)
   Then uv = (B^2n+B^n)*u1*v1 + B^n*(u1-u0)*(v0-v1) + (B^n+1)*u0*v0

   B is the base of storage, number of digits in u1,u0 close to equal.
*/
static void _bc_rec_mul(bc_num u, size_t ulen, bc_num v, size_t vlen, bc_num *prod)
{
	bc_num u0, u1, v0, v1;
	bc_num m1, m2, m3, d1, d2;
	size_t n;
	bool m1zero;

	/* Base case? */
	if ((ulen + vlen) < mul_base_digits
		|| ulen < MUL_SMALL_DIGITS
		|| vlen < MUL_SMALL_DIGITS
	) {
		_bc_simp_mul(u, ulen, v, vlen, prod);
		return;
	}

	/* Calculate n -- the u and v split point in digits. */
	n = (MAX(ulen, vlen) + 1) / 2;

	/* Split u and v. */
	if (ulen < n) {
		u1 = bc_copy_num(BCG(_zero_));
		u0 = new_sub_num(ulen, 0, u->n_value);
	} else {
		u1 = new_sub_num(ulen - n, 0, u->n_value);
		u0 = new_sub_num(n, 0, u->n_value + ulen - n);
	}
	if (vlen < n) {
		v1 = bc_copy_num(BCG(_zero_));
		v0 = new_sub_num(vlen, 0, v->n_value);
	} else {
		v1 = new_sub_num(vlen - n, 0, v->n_value);
		v0 = new_sub_num(n, 0, v->n_value + vlen - n);
	}
	_bc_rm_leading_zeros(u1);
	_bc_rm_leading_zeros(u0);
	_bc_rm_leading_zeros(v1);
	_bc_rm_leading_zeros(v0);

	m1zero = bc_is_zero(u1) || bc_is_zero(v1);

	/* Calculate sub results ... */

	bc_init_num(&d1);
	bc_init_num(&d2);
	bc_sub(u1, u0, &d1, 0);
	bc_sub(v0, v1, &d2, 0);


	/* Do recursive multiplies and shifted adds. */
	if (m1zero) {
		m1 = bc_copy_num(BCG(_zero_));
	} else {
		_bc_rec_mul(u1, u1->n_len, v1, v1->n_len, &m1);
	}

	if (bc_is_zero(d1) || bc_is_zero(d2)) {
		m2 = bc_copy_num(BCG(_zero_));
	} else {
		_bc_rec_mul(d1, d1->n_len, d2, d2->n_len, &m2);
	}

	if (bc_is_zero(u0) || bc_is_zero(v0)) {
		m3 = bc_copy_num(BCG(_zero_));
	} else {
		_bc_rec_mul(u0, u0->n_len, v0, v0->n_len, &m3);
	}

	/* Initialize product */
	*prod = bc_new_num(ulen + vlen + 1, 0);

	if (!m1zero) {
		_bc_shift_addsub(*prod, m1, 2 * n, false);
		_bc_shift_addsub(*prod, m1, n, false);
	}
	_bc_shift_addsub(*prod, m3, n, false);
	_bc_shift_addsub(*prod, m3, 0, false);
	_bc_shift_addsub(*prod, m2, n, d1->n_sign != d2->n_sign);

	/* Now clean up! */
	bc_free_num (&u1);
	bc_free_num (&u0);
	bc_free_num (&v1);
	bc_free_num (&m1);
	bc_free_num (&v0);
	bc_free_num (&m2);
	bc_free_num (&m3);
	bc_free_num (&d1);
	bc_free_num (&d2);
}

/* The multiply routine.  N2 times N1 is put int PROD with the scale of
   the result being MIN(N2 scale+N1 scale, MAX (SCALE, N2 scale, N1 scale)).
   */

void bc_multiply(bc_num n1, bc_num n2, bc_num *prod, size_t scale)
{
	bc_num pval;
	size_t len1, len2;
	size_t full_scale, prod_scale;

	/* Initialize things. */
	len1 = n1->n_len + n1->n_scale;
	len2 = n2->n_len + n2->n_scale;
	full_scale = n1->n_scale + n2->n_scale;
	prod_scale = MIN(full_scale, MAX(scale, MAX(n1->n_scale, n2->n_scale)));

	/* Do the multiply */
	_bc_rec_mul(n1, len1, n2, len2, &pval);

	/* Assign to prod and clean up the number. */
	pval->n_sign = (n1->n_sign == n2->n_sign ? PLUS : MINUS);
	pval->n_value = pval->n_ptr;
	pval->n_len = len2 + len1 + 1 - full_scale;
	pval->n_scale = prod_scale;
	_bc_rm_leading_zeros(pval);
	if (bc_is_zero(pval)) {
		pval->n_sign = PLUS;
	}
	bc_free_num(prod);
	*prod = pval;
}
