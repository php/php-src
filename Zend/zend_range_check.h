/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   |          Marc Bennewitz <marc@mabe.berlin>                           |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_RANGE_CHECK_H
#define ZEND_RANGE_CHECK_H

#include "zend_long.h"

/* _ZEND_INT_TYPE_CHECK(T): the integer constant 0,
 * or a compile error unless T is an integer type not wider than intmax_t. */
#ifndef __cplusplus
/* C: a static_assert() cannot appear in an expression itself,
 * but it can as a member of a struct of which only the size is taken.
 * The "% 1" only compiles for an integer operand. */
# include <assert.h>

# define _ZEND_INT_TYPE_CHECK(T) \
	((int)(0 * sizeof(struct { \
		static_assert( \
			sizeof((T)0 % 1) && sizeof(T) <= sizeof(intmax_t), \
			"T must be an integer type not wider than intmax_t" \
		); \
		char zend_dummy; \
	})))
#else
/* C++ does not allow to define a struct within sizeof(),
 * so the static_assert() lives in a class template instead,
 * which taking the size of instantiates. */
extern "C++" {
# include <cstdint>
# include <type_traits>
	template<typename T> struct _zend_int_type_check {
		static_assert(std::is_integral<T>::value && sizeof(T) <= sizeof(intmax_t),
			"T must be an integer type not wider than intmax_t");
	};
}

# define _ZEND_INT_TYPE_CHECK(T) ((int)(0 * sizeof(_zend_int_type_check<T>)))
#endif

/* _ZEND_TYPE_IS_SIGNED(T): whether the integer type T is signed, as an integer constant expression. */
#define _ZEND_TYPE_IS_SIGNED(T) (!((T)-1 > (T)0))

/* ZEND_TYPE_MAX(T): the largest value of the integer type T, as an integer constant expression,
 * for types without a constant of their own such as time_t.
 * T may be any signed or unsigned integer type not wider than intmax_t. */
#define ZEND_TYPE_MAX(T) (_ZEND_INT_TYPE_CHECK(T) + (_ZEND_TYPE_IS_SIGNED(T) \
	? (((((T)1 << (sizeof(T) * CHAR_BIT - 2)) - 1) * 2) + 1) \
	: (T)-1 \
))

/* ZEND_TYPE_MIN(T): the smallest value of the integer type T, as an integer constant expression.
 * T may be any signed or unsigned integer type not wider than intmax_t. */
#define ZEND_TYPE_MIN(T) (_ZEND_TYPE_IS_SIGNED(T) ? -ZEND_TYPE_MAX(T) - 1 : (T)0)

/* ZEND_IN_RANGE_OF */
#ifndef __cplusplus
/* C: _Generic dispatches on the promoted type of x, without evaluating it,
 * to a comparison done in intmax_t for a signed x or in uintmax_t for an unsigned one,
 * against ZEND_TYPE_MIN(T) and ZEND_TYPE_MAX(T). */
static inline bool _zend_in_range_signed(intmax_t v, intmax_t min, uintmax_t max)
{
	if (max <= INTMAX_MAX) {
		return v >= min && v <= (intmax_t)max;
	}
	return v >= 0; /* T is a uintmax_t-wide unsigned type */
}

static inline bool _zend_in_range_unsigned(uintmax_t v, uintmax_t max)
{
	return v <= max;
}

# define _ZEND_IN_RANGE_SIGNED(x, T) _zend_in_range_signed((x), ZEND_TYPE_MIN(T), ZEND_TYPE_MAX(T))
# define _ZEND_IN_RANGE_UNSIGNED(x, T) _zend_in_range_unsigned((x), ZEND_TYPE_MAX(T))

/* ZEND_IN_RANGE_OF(x, T): whether the value of the integer expression x is representable in the integer type T.
 * x and T may be any signed or unsigned integer types not wider than intmax_t.
 * x is evaluated exactly once. */
# define ZEND_IN_RANGE_OF(x, T) EXPECTED(_Generic((x) + 0, \
	int:                _ZEND_IN_RANGE_SIGNED(x, T), \
	long:               _ZEND_IN_RANGE_SIGNED(x, T), \
	long long:          _ZEND_IN_RANGE_SIGNED(x, T), \
	unsigned int:       _ZEND_IN_RANGE_UNSIGNED(x, T), \
	unsigned long:      _ZEND_IN_RANGE_UNSIGNED(x, T), \
	unsigned long long: _ZEND_IN_RANGE_UNSIGNED(x, T) \
))
#else
/* C++ has no _Generic (g++ and MSVC reject it).
 * A function template deduces the type of x instead.
 * The unary + applies the integer promotions, as (x) + 0 does for C. */
extern "C++" {
	template<typename T, typename X>
	constexpr bool _zend_in_range_of(X x) {
		static_assert(std::is_integral<X>::value && sizeof(X) <= sizeof(intmax_t), "x must be an integer not wider than intmax_t");

		return std::is_signed<X>::value
			? (intmax_t)x >= (intmax_t)ZEND_TYPE_MIN(T)
				&& ((uintmax_t)ZEND_TYPE_MAX(T) > (uintmax_t)INTMAX_MAX
					|| (intmax_t)x <= (intmax_t)ZEND_TYPE_MAX(T))
			: (uintmax_t)x <= (uintmax_t)ZEND_TYPE_MAX(T);
	}
}

/* ZEND_IN_RANGE_OF(x, T): whether the value of the integer expression x is representable in the integer type T.
 * x and T may be any signed or unsigned integer types not wider than intmax_t.
 * x is evaluated exactly once. */
# define ZEND_IN_RANGE_OF(x, T) EXPECTED(_zend_in_range_of<T>(+(x)))
#endif

/* Comparison zend_long vs size_t */
#define ZEND_SIZE_T_GT_ZEND_LONG(size, zlong)  ((zlong) < 0 || (size) > (size_t)(zlong))
#define ZEND_SIZE_T_GTE_ZEND_LONG(size, zlong) ((zlong) < 0 || (size) >= (size_t)(zlong))
#define ZEND_SIZE_T_LT_ZEND_LONG(size, zlong)  ((zlong) >= 0 && (size) < (size_t)(zlong))
#define ZEND_SIZE_T_LTE_ZEND_LONG(size, zlong) ((zlong) >= 0 && (size) <= (size_t)(zlong))

/* -- DEPRECATED -- */

/* The zend_long and size_t vs. (unsigned) int checks below are deprecated, use ZEND_IN_RANGE_OF() instead. */
static inline ZEND_ATTRIBUTE_DEPRECATED void _zend_deprecated_use_ZEND_IN_RANGE_OF_instead(void) {}

#if SIZEOF_INT < SIZEOF_ZEND_LONG
# define ZEND_LONG_CAN_OVFL_INT 1     /* Deprecated */
# define ZEND_LONG_CAN_OVFL_UINT 1    /* Deprecated */
# define ZEND_LONG_INT_OVFL(zlong)    (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((zlong) > (zend_long)INT_MAX))
# define ZEND_LONG_INT_UDFL(zlong)    (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((zlong) < (zend_long)INT_MIN))
# define ZEND_LONG_EXCEEDS_INT(zlong) (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((zlong) > (zend_long)INT_MAX || (zlong) < (zend_long)INT_MIN))
# define ZEND_LONG_UINT_OVFL(zlong)   (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((zlong) < 0 || (zlong) > (zend_long)UINT_MAX))
#else
# define ZEND_LONG_INT_OVFL(zl)       (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), 0)
# define ZEND_LONG_INT_UDFL(zl)       (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), 0)
# define ZEND_LONG_EXCEEDS_INT(zlong) (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), 0)
# define ZEND_LONG_UINT_OVFL(zl)      (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), 0)
#endif

#define ZEND_SIZE_T_INT_OVFL(size) (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((size) > (size_t)INT_MAX))
#if SIZEOF_INT < SIZEOF_SIZE_T
# define ZEND_SIZE_T_CAN_OVFL_UINT 1 /* Deprecated */
# define ZEND_SIZE_T_UINT_OVFL(size) (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), UNEXPECTED((size) > (size_t)UINT_MAX))
#else
# define ZEND_SIZE_T_UINT_OVFL(size) (_zend_deprecated_use_ZEND_IN_RANGE_OF_instead(), 0)
#endif

#endif /* ZEND_RANGE_CHECK_H */
