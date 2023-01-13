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

#include "zend_rc_debug.h"

#if ZEND_RC_DEBUG

#include "zend_types.h"

ZEND_API bool zend_rc_debug = false;

ZEND_API void ZEND_RC_MOD_CHECK(const zend_refcounted_h *p)
{
	if (!zend_rc_debug) {
		return;
	}

	uint8_t type = zval_gc_type(p->u.type_info);

	/* Skip checks for OBJECT/NULL type to avoid interpreting the flag incorrectly. */
	if (type != IS_OBJECT && type != IS_NULL) {
		ZEND_ASSERT(!(zval_gc_flags(p->u.type_info) & GC_IMMUTABLE));

		/* The GC_PERSISTENT flag is reused for IS_OBJ_WEAKLY_REFERENCED on objects. */
		ZEND_ASSERT((zval_gc_flags(p->u.type_info) & (GC_PERSISTENT|GC_PERSISTENT_LOCAL)) != GC_PERSISTENT);
	}
}

#endif
