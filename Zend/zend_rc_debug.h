/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_RC_DEBUG_H
#define ZEND_RC_DEBUG_H

#ifndef ZEND_RC_DEBUG
# define ZEND_RC_DEBUG 0
#endif

#if ZEND_RC_DEBUG

#ifdef PHP_WIN32
# include "zend_config.w32.h"
#else
# include "zend_config.h"
#endif

#include <stdbool.h>
#include <stdint.h>

extern ZEND_API bool zend_rc_debug;

/* The GC_PERSISTENT flag is reused for IS_OBJ_WEAKLY_REFERENCED on objects.
 * Skip checks for OBJECT/NULL type to avoid interpreting the flag incorrectly. */
# define ZEND_RC_MOD_CHECK(p) do { \
		if (zend_rc_debug) { \
			uint8_t type = zval_gc_type((p)->u.type_info); \
			if (type != IS_OBJECT && type != IS_NULL) { \
				ZEND_ASSERT(!(zval_gc_flags((p)->u.type_info) & GC_IMMUTABLE)); \
				ZEND_ASSERT((zval_gc_flags((p)->u.type_info) & (GC_PERSISTENT|GC_PERSISTENT_LOCAL)) != GC_PERSISTENT); \
			} \
		} \
	} while (0)
#else
# define ZEND_RC_MOD_CHECK(p) \
	do { } while (0)
#endif

#endif /* ZEND_RC_DEBUG_H */
