/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Seiler <chris_se@gmx.net>                         |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_compile.h"
#include "zend_float.h"

ZEND_API void zend_init_fpu(void) /* {{{ */
{
#if XPFPA_HAVE_CW
	XPFPA_DECLARE

	if (!EG(saved_fpu_cw_ptr)) {
		EG(saved_fpu_cw_ptr) = (void*)&EG(saved_fpu_cw);
	}
	XPFPA_STORE_CW(EG(saved_fpu_cw_ptr));
	XPFPA_SWITCH_DOUBLE();
#else
	EG(saved_fpu_cw_ptr) = NULL;
#endif
}
/* }}} */

ZEND_API void zend_shutdown_fpu(void) /* {{{ */
{
#if XPFPA_HAVE_CW
	if (EG(saved_fpu_cw_ptr)) {
		XPFPA_RESTORE_CW(EG(saved_fpu_cw_ptr));
	}
#endif
	EG(saved_fpu_cw_ptr) = NULL;
}
/* }}} */

ZEND_API void zend_ensure_fpu_mode(void) /* {{{ */
{
	XPFPA_DECLARE

	XPFPA_SWITCH_DOUBLE();
}
/* }}} */
