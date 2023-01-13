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

#include "zend_portability.h"

typedef struct _zend_refcounted_h zend_refcounted_h;

extern ZEND_API bool zend_rc_debug;

BEGIN_EXTERN_C()

ZEND_API void ZEND_RC_MOD_CHECK(const zend_refcounted_h *p);

END_EXTERN_C()

#else
# define ZEND_RC_MOD_CHECK(p) \
	do { } while (0)
#endif

#endif /* ZEND_RC_DEBUG_H */
