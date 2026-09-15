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
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_RANGE_CHECK_H
#define ZEND_RANGE_CHECK_H

#include "zend_long.h"

/* Flag macros for basic range recognition. Notable is that
   always sizeof(signed) == sizeof(unsigned), so no need to
   overcomplicate things. */
#if SIZEOF_INT < SIZEOF_ZEND_LONG
# define ZEND_LONG_CAN_OVFL_INT 1
# define ZEND_LONG_CAN_OVFL_UINT 1
#endif

#if SIZEOF_INT < SIZEOF_SIZE_T
/* size_t can always overflow signed int on the same platform.
   Furthermore, by the current design, size_t can always
   overflow zend_long. */
# define ZEND_SIZE_T_CAN_OVFL_UINT 1
#endif


/* zend_long vs. (unsigned) int checks. */
#define ZEND_LONG_UINT_UDFL(zlong) UNEXPECTED((zlong) < 0)
#ifdef ZEND_LONG_CAN_OVFL_INT
# define ZEND_LONG_INT_OVFL(zlong) UNEXPECTED((zlong) > (zend_long)INT_MAX)
# define ZEND_LONG_INT_UDFL(zlong) UNEXPECTED((zlong) < (zend_long)INT_MIN)
# define ZEND_LONG_EXCEEDS_INT(zlong) UNEXPECTED((zlong) < (zend_long)INT_MIN || (zlong) > (zend_long)INT_MAX)
# define ZEND_LONG_UINT_OVFL(zlong) UNEXPECTED((zlong) > (zend_long)UINT_MAX)
# define ZEND_LONG_EXCEEDS_UINT(zlong) UNEXPECTED((zlong) < 0 || (zlong) > (zend_long)UINT_MAX)
#else
# define ZEND_LONG_INT_OVFL(zlong) (0)
# define ZEND_LONG_INT_UDFL(zlong) (0)
# define ZEND_LONG_EXCEEDS_INT(zlong) (0)
# define ZEND_LONG_UINT_OVFL(zlong) (0)
# define ZEND_LONG_EXCEEDS_UINT(zlong) UNEXPECTED((zlong) < 0)
#endif

/* zend_long vs. (unsigned) long checks. A long is not of the same width as a
 * zend_long everywhere, LLP64 being the common case. */
#define ZEND_LONG_ULONG_UDFL(zlong) UNEXPECTED((zlong) < 0)
#if SIZEOF_LONG < SIZEOF_ZEND_LONG
# define ZEND_LONG_LONG_OVFL(zlong) UNEXPECTED((zlong) > (zend_long)LONG_MAX)
# define ZEND_LONG_LONG_UDFL(zlong) UNEXPECTED((zlong) < (zend_long)LONG_MIN)
# define ZEND_LONG_EXCEEDS_LONG(zlong) UNEXPECTED((zlong) < (zend_long)LONG_MIN || (zlong) > (zend_long)LONG_MAX)
# define ZEND_LONG_ULONG_OVFL(zlong) UNEXPECTED((zlong) > (zend_long)ULONG_MAX)
# define ZEND_LONG_EXCEEDS_ULONG(zlong) UNEXPECTED((zlong) < 0 || (zlong) > (zend_long)ULONG_MAX)
#else
# define ZEND_LONG_LONG_OVFL(zlong) (0)
# define ZEND_LONG_LONG_UDFL(zlong) (0)
# define ZEND_LONG_EXCEEDS_LONG(zlong) (0)
# define ZEND_LONG_ULONG_OVFL(zlong) (0)
# define ZEND_LONG_EXCEEDS_ULONG(zlong) UNEXPECTED((zlong) < 0)
#endif

/* size_t vs (unsigned) int checks. */
#define ZEND_SIZE_T_INT_OVFL(size) 	UNEXPECTED((size) > (size_t)INT_MAX)
#ifdef ZEND_SIZE_T_CAN_OVFL_UINT
# define ZEND_SIZE_T_UINT_OVFL(size) UNEXPECTED((size) > (size_t)UINT_MAX)
#else
# define ZEND_SIZE_T_UINT_OVFL(size) (0)
#endif

/* Comparison zend_long vs size_t */
#define ZEND_SIZE_T_GT_ZEND_LONG(size, zlong) ((zlong) < 0 || (size) > (size_t)(zlong))
#define ZEND_SIZE_T_GTE_ZEND_LONG(size, zlong) ((zlong) < 0 || (size) >= (size_t)(zlong))
#define ZEND_SIZE_T_LT_ZEND_LONG(size, zlong) ((zlong) >= 0 && (size) < (size_t)(zlong))
#define ZEND_SIZE_T_LTE_ZEND_LONG(size, zlong) ((zlong) >= 0 && (size) <= (size_t)(zlong))

#endif /* ZEND_RANGE_CHECK_H */
