/* doaddsub.c: bcmath library file. */
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
#include "private.h"
#include <stddef.h>


/* Perform addition: N1 is added to N2 and the value is
   returned.  The signs of N1 and N2 are ignored.
   SCALE_MIN is to set the minimum scale of the result. */

bc_num _bc_do_add(bc_num n1, bc_num n2, size_t scale_min)
{
	bc_num sum;
	size_t sum_scale, sum_digits;
	char *n1ptr, *n2ptr, *sumptr;
	size_t n1bytes, n2bytes;
	bool carry;

	/* Prepare sum. */
	sum_scale = MAX (n1->n_scale, n2->n_scale);
	sum_digits = MAX (n1->n_len, n2->n_len) + 1;
	sum = bc_new_num (sum_digits, MAX(sum_scale, scale_min));

	/* Start with the fraction part.  Initialize the pointers. */
	n1bytes = n1->n_scale;
	n2bytes = n2->n_scale;
	n1ptr = (char *) (n1->n_value + n1->n_len + n1bytes - 1);
	n2ptr = (char *) (n2->n_value + n2->n_len + n2bytes - 1);
	sumptr = (char *) (sum->n_value + sum_scale + sum_digits - 1);

	/* Add the fraction part.  First copy the longer fraction.*/
	if (n1bytes != n2bytes) {
		if (n1bytes > n2bytes) {
			while (n1bytes > n2bytes) {
				*sumptr-- = *n1ptr--;
				n1bytes--;
			}
		} else {
			while (n2bytes > n1bytes) {
				*sumptr-- = *n2ptr--;
				n2bytes--;
			}
		}
	}

	/* Now add the remaining fraction part and equal size integer parts. */
	n1bytes += n1->n_len;
	n2bytes += n2->n_len;
	carry = 0;
	while ((n1bytes > 0) && (n2bytes > 0)) {
		*sumptr = *n1ptr-- + *n2ptr-- + carry;
		if (*sumptr > (BASE - 1)) {
			carry = 1;
			*sumptr -= BASE;
		} else {
			carry = 0;
		}
		sumptr--;
		n1bytes--;
		n2bytes--;
	}

	/* Now add carry the longer integer part. */
	if (n1bytes == 0) {
		n1bytes = n2bytes;
		n1ptr = n2ptr;
	}
	while (n1bytes-- > 0) {
		*sumptr = *n1ptr-- + carry;
		if (*sumptr > (BASE - 1)) {
			carry = true;
			*sumptr -= BASE;
		} else {
			carry = false;
		}
		sumptr--;
	}

	/* Set final carry. */
	if (carry) {
		*sumptr += 1;
	}

	/* Adjust sum and return. */
	_bc_rm_leading_zeros(sum);
	return sum;
}


/* Perform subtraction: N2 is subtracted from N1 and the value is
   returned.  The signs of N1 and N2 are ignored.  Also, N1 is
   assumed to be larger than N2.  SCALE_MIN is the minimum scale
   of the result. */
bc_num _bc_do_sub(bc_num n1, bc_num n2, size_t scale_min)
{
	bc_num diff;
	/* The caller is guaranteed that n1 is always large. */
	size_t diff_len = EXPECTED(n1->n_len >= n2->n_len) ? n1->n_len : n2->n_len;
	size_t diff_scale = MAX(n1->n_scale, n2->n_scale);
	/* Same condition as EXPECTED before, but using EXPECTED again will make it slower. */
	size_t min_len = n1->n_len >= n2->n_len ? n2->n_len : n1->n_len;
	size_t min_scale = MIN(n1->n_scale, n2->n_scale);
	size_t min_bytes = min_len + min_scale;
	size_t borrow = 0;
	size_t count;
	int val;
	char *n1ptr, *n2ptr, *diffptr;

	/* Allocate temporary storage. */
	diff = bc_new_num (diff_len, MAX(diff_scale, scale_min));

	/* Initialize the subtract. */
	n1ptr = (char *) (n1->n_value + n1->n_len + n1->n_scale - 1);
	n2ptr = (char *) (n2->n_value + n2->n_len + n2->n_scale - 1);
	diffptr = (char *) (diff->n_value + diff_len + diff_scale - 1);

	/* Take care of the longer scaled number. */
	if (n1->n_scale != min_scale) {
		/* n1 has the longer scale */
		for (count = n1->n_scale - min_scale; count > 0; count--) {
			*diffptr-- = *n1ptr--;
		}
	} else {
		/* n2 has the longer scale */
		for (count = n2->n_scale - min_scale; count > 0; count--) {
			val = -*n2ptr-- - borrow;
			if (val < 0) {
				val += BASE;
				borrow = 1;
			} else {
				borrow = 0;
			}
			*diffptr-- = val;
		}
	}

	/* Now do the equal length scale and integer parts. */
	count = 0;
	/* Uses SIMD to perform calculations at high speed. */
	if (min_bytes >= sizeof(BC_UINT_T)) {
		diffptr++;
		n1ptr++;
		n2ptr++;
		while (count + sizeof(BC_UINT_T) <= min_bytes) {
			diffptr -= sizeof(BC_UINT_T);
			n1ptr -= sizeof(BC_UINT_T);
			n2ptr -= sizeof(BC_UINT_T);

			BC_UINT_T n1bytes;
			BC_UINT_T n2bytes;
			memcpy(&n1bytes, n1ptr, sizeof(n1bytes));
			memcpy(&n2bytes, n2ptr, sizeof(n2bytes));

#if BC_LITTLE_ENDIAN
			/* Little endian requires changing the order of bytes. */
			n1bytes = BC_BSWAP(n1bytes);
			n2bytes = BC_BSWAP(n2bytes);
#endif

			n1bytes -= n2bytes + borrow;
			/* If the most significant bit is 1, a carry down has occurred. */
			bool tmp_borrow = n1bytes & ((BC_UINT_T) 1 << (8 * sizeof(BC_UINT_T) - 1));

			/*
			 * Check the most significant bit of each of the bytes, and if it is 1, a carry down has
			 * occurred. When carrying down occurs, due to the difference between decimal and hexadecimal
			 * numbers, an extra 6 is added to the lower 4 bits.
			 * Therefore, for a byte that has been carried down, set all the upper 4 bits to 0 and subtract
			 * 6 from the lower 4 bits to adjust it to the correct value as a decimal number.
			 */
			BC_UINT_T borrow_mask = ((n1bytes & SWAR_REPEAT(0x80)) >> 7) * 0x06;
			n1bytes = (n1bytes & SWAR_REPEAT(0x0F)) - borrow_mask;

#if BC_LITTLE_ENDIAN
			/* Little endian requires changing the order of bytes back. */
			n1bytes = BC_BSWAP(n1bytes);
#endif

			memcpy(diffptr, &n1bytes, sizeof(n1bytes));

			borrow = tmp_borrow;
			count += sizeof(BC_UINT_T);
		}
		diffptr--;
		n1ptr--;
		n2ptr--;
	}

	/* Calculate the remaining bytes that are less than the size of BC_UINT_T using a normal loop. */
	for (; count < min_bytes; count++) {
		val = *n1ptr-- - *n2ptr-- - borrow;
		if (val < 0) {
			val += BASE;
			borrow = 1;
		} else {
			borrow = 0;
		}
		*diffptr-- = val;
	}

	/* If n1 has more digits than n2, we now do that subtract. */
	if (diff_len != min_len) {
		for (count = diff_len - min_len; count > 0; count--) {
			val = *n1ptr-- - borrow;
			if (val < 0) {
				val += BASE;
				borrow = 1;
			} else {
				borrow = 0;
			}
			*diffptr-- = val;
		}
	}

	/* Clean up and return. */
	_bc_rm_leading_zeros(diff);
	return diff;
}
