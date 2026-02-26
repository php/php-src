/* str2num.c: bcmath library file. */
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
#include "convert.h"
#include "private.h"
#include "xsse.h"
#include <stdbool.h>
#include <stddef.h>

/* Convert strings to bc numbers.  Base 10 only.*/
static inline const char *bc_count_digits(const char *str, const char *end)
{
	/* Process in bulk */
#ifdef XSSE2
	const __m128i offset = _mm_set1_epi8((signed char) (SCHAR_MIN - '0'));
	/* we use the less than comparator, so add 1 */
	const __m128i threshold = _mm_set1_epi8(SCHAR_MIN + ('9' + 1 - '0'));

	while (str + sizeof(__m128i) <= end) {
		__m128i bytes = _mm_loadu_si128((const __m128i *) str);
		/* Wrapping-add the offset to the bytes, such that all bytes below '0' are positive and others are negative.
		 * More specifically, '0' will be -128 and '9' will be -119. */
		bytes = _mm_add_epi8(bytes, offset);
		/* Now mark all bytes that are <= '9', i.e. <= -119, i.e. < -118, i.e. the threshold. */
		bytes = _mm_cmplt_epi8(bytes, threshold);

		int mask = _mm_movemask_epi8(bytes);
		if (mask != 0xffff) {
			/* At least one of the bytes is not within range. Move to the first offending byte. */
#ifdef PHP_HAVE_BUILTIN_CTZL
			return str + __builtin_ctz(~mask);
#else
			break;
#endif
		}

		str += sizeof(__m128i);
	}
#endif

	while (*str >= '0' && *str <= '9') {
		str++;
	}

	return str;
}

static inline const char *bc_skip_zero_reverse(const char *scanner, const char *stop)
{
	/* Check in bulk */
#ifdef XSSE2
	const __m128i c_zero_repeat = _mm_set1_epi8('0');
	while (scanner - sizeof(__m128i) >= stop) {
		scanner -= sizeof(__m128i);
		__m128i bytes = _mm_loadu_si128((const __m128i *) scanner);
		/* Checks if all numeric strings are equal to '0'. */
		bytes = _mm_cmpeq_epi8(bytes, c_zero_repeat);

		int mask = _mm_movemask_epi8(bytes);
		/* The probability of having 16 trailing 0s in a row is very low, so we use EXPECTED. */
		if (EXPECTED(mask != 0xffff)) {
			/* Move the pointer back and check each character in loop. */
			scanner += sizeof(__m128i);
			break;
		}
	}
#endif

	/* Exclude trailing zeros. */
	while (scanner - 1 >= stop && scanner[-1] == '0') {
		scanner--;
	}

	return scanner;
}

static bool bc_scientific_notation_str2num(
	bc_num *num, const char *str, const char *end, const char *integer_ptr, const char *fractional_ptr, const char *exponent_ptr,
	size_t digits, size_t *full_scale)
{
	const char *fractional_end = fractional_ptr != NULL ? exponent_ptr : NULL;

	if (UNEXPECTED(*exponent_ptr != 'e' && *exponent_ptr != 'E')) {
		goto fail;
	}
	exponent_ptr++;

	sign exponent_sign = PLUS;
	if (*exponent_ptr == '+') {
		/* Skip Sign */
		exponent_ptr++;
	} else if (*exponent_ptr == '-') {
		exponent_sign = MINUS;
		exponent_ptr++;
	}

	/* Skip exponent leading zeros. This is rare, so don't do bulk processing. */
	while (*exponent_ptr == '0') {
		exponent_ptr++;
	}

	const char *exponent_end = bc_count_digits(exponent_ptr, end);
	if (UNEXPECTED(*exponent_end != '\0')) {
		/* invalid num */
		goto fail;
	}

	size_t exponent = 0;
	while (exponent_ptr < exponent_end) {
		exponent = exponent * 10 + (*exponent_ptr - '0'); /* TODO: check overflow */
		exponent_ptr++;
	}

	if (fractional_end != NULL) {
		/* Skip fraction trailing zeros. This is rare, so don't do bulk processing. */
		while (fractional_end[-1] == '0') {
			fractional_end--;
		}
	}

	const char *integer_end = integer_ptr + digits;

	size_t str_scale = fractional_end - fractional_ptr;
	size_t str_full_len = digits + str_scale;
	size_t leading_zero_paddings = 0;

	if (exponent_sign == PLUS) {
		digits += exponent;
		if (digits == 0) {
			leading_zero_paddings = 1;
		}
		str_scale = str_scale > exponent ? str_scale - exponent : 0;
	} else {
		str_scale += exponent;
		if (digits > exponent) {
			digits -= exponent;
		} else {
			leading_zero_paddings = exponent - digits + 1; /* 1 is for interger part */
			digits = 0;
		}
	}

	*num = bc_new_num_nonzeroed(digits > 0 ? digits : 1, str_scale); /* 1 is for 0 */
	(*num)->n_sign = *str == '-' ? MINUS : PLUS;
	char *nptr = (*num)->n_value;

	for (size_t i = 0; i < leading_zero_paddings; i++) {
		*nptr++ = 0;
	}

	nptr = bc_copy_and_toggle_bcd(nptr, integer_ptr, integer_end);
	if (fractional_ptr != NULL) {
		nptr = bc_copy_and_toggle_bcd(nptr, fractional_ptr, fractional_end);
	}

	if (digits > str_full_len) {
		/* Fill the rest integer part with zeros */
		for (size_t i = 0; i < digits - str_full_len; i++) {
			*nptr++ = 0;
		}
	}

	if (full_scale) {
		*full_scale = str_scale;
	}

	return true;

fail:
	*num = bc_copy_num(BCG(_zero_));
	return false;
}

/* Assumes `num` points to NULL, i.e. does yet not hold a number. */
bool bc_str2num(bc_num *num, const char *str, const char *end, size_t scale, size_t *full_scale, bool auto_scale)
{
	size_t str_scale = 0;
	const char *ptr = str;
	const char *fractional_ptr = NULL;
	const char *fractional_end = NULL;
	bool zero_int = false;

	ZEND_ASSERT(*num == NULL);

	/* Check for valid number and count digits. */
	if ((*ptr == '+') || (*ptr == '-')) {
		/* Skip Sign */
		ptr++;
	}
	/* Skip leading zeros. */
	while (*ptr == '0') {
		ptr++;
	}
	const char *integer_ptr = ptr;
	/* digits before the decimal point */
	ptr = bc_count_digits(ptr, end);
	size_t digits = ptr - integer_ptr;
	/* decimal point */
	const char *decimal_point = (*ptr == '.') ? ptr : NULL;

	/* If a non-digit and non-decimal-point indicator is in the string, i.e. an invalid character */
	if (!decimal_point && *ptr != '\0') {
		return bc_scientific_notation_str2num(num, str, end, integer_ptr, fractional_ptr, ptr, digits, full_scale);
	}

	/* search and validate fractional end if exists */
	if (decimal_point) {
		/* search */
		fractional_ptr = fractional_end = decimal_point + 1;
		/* For strings that end with a decimal point, such as "012." */
		if (UNEXPECTED(*fractional_ptr == '\0')) {
			if (full_scale) {
				*full_scale = 0;
			}
			goto after_fractional;
		}

		/* validate */
		fractional_end = bc_count_digits(fractional_ptr, end);
		if (*fractional_end != '\0') {
			return bc_scientific_notation_str2num(num, str, end, integer_ptr, fractional_ptr, fractional_end, digits, full_scale);
		}

		if (full_scale) {
			*full_scale = fractional_end - fractional_ptr;
		}

		/* Exclude trailing zeros. */
		fractional_end = bc_skip_zero_reverse(fractional_end, fractional_ptr);

		/* Calculate the length of the fraction excluding trailing zero. */
		str_scale = fractional_end - fractional_ptr;

		/*
		 * If set the scale manually and it is smaller than the automatically calculated scale,
		 * adjust it to match the manual setting.
		 */
		if (str_scale > scale && !auto_scale) {
			fractional_end -= str_scale - scale;
			str_scale = scale;

			/*
			 * e.g. 123.0001 with scale 2 -> 123.00
			 * So, remove the trailing 0 again.
			 */
			if (str_scale > 0) {
				const char *fractional_new_end = bc_skip_zero_reverse(fractional_end, fractional_ptr);
				str_scale -= fractional_end - fractional_new_end; /* fractional_end >= fractional_new_end */
			}
		}
	} else {
		if (full_scale) {
			*full_scale = 0;
		}
	}

after_fractional:

	if (digits + str_scale == 0) {
		goto zero;
	}

	/* Adjust numbers and allocate storage and initialize fields. */
	if (digits == 0) {
		zero_int = true;
		digits = 1;
	}
	*num = bc_new_num_nonzeroed(digits, str_scale);
	(*num)->n_sign = *str == '-' ? MINUS : PLUS;
	char *nptr = (*num)->n_value;

	if (zero_int) {
		*nptr++ = 0;
		/*
		 * If zero_int is true and the str_scale is 0, there is an early return,
		 * so here str_scale is always greater than 0.
		 */
		nptr = bc_copy_and_toggle_bcd(nptr, fractional_ptr, fractional_end);
	} else {
		const char *integer_end = integer_ptr + digits;
		nptr = bc_copy_and_toggle_bcd(nptr, integer_ptr, integer_end);
		if (str_scale > 0) {
			nptr = bc_copy_and_toggle_bcd(nptr, fractional_ptr, fractional_end);
		}
	}

	return true;

zero:
	*num = bc_copy_num(BCG(_zero_));
	return true;
}
