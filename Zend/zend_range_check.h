/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
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
#ifdef ZEND_LONG_CAN_OVFL_INT
# define ZEND_LONG_INT_OVFL(zlong) UNEXPECTED((zlong) > (zend_long)INT_MAX)
# define ZEND_LONG_INT_UDFL(zlong) UNEXPECTED((zlong) < (zend_long)INT_MIN)
# define ZEND_LONG_EXCEEDS_INT(zlong) UNEXPECTED(ZEND_LONG_INT_OVFL(zlong) || ZEND_LONG_INT_UDFL(zlong))
# define ZEND_LONG_UINT_OVFL(zlong) UNEXPECTED((zlong) < 0 || (zlong) > (zend_long)UINT_MAX)
#else
# define ZEND_LONG_INT_OVFL(zl) (0)
# define ZEND_LONG_INT_UDFL(zl) (0)
# define ZEND_LONG_EXCEEDS_INT(zlong) (0)
# define ZEND_LONG_UINT_OVFL(zl) (0)
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
