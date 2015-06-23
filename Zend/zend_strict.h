/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Francois Laupretre <francois@php.net>                        |
   +----------------------------------------------------------------------+
*/
/* $Id$ */
/*============================================================================
 This file implements '--enable-api-checks' and '--enable-strict-api'
 configure otpions.

 In 'api-check' mode the names of the protected structure elements are modified,
 causing compilation to fail when some code attempts a direct access to the
 element using its 'normal' name. This allows to detect and fixes places where
 code does not respect the published API.

 In 'strict-api' mode, the API becommes more restrictive, allowing developers
 to check their code against future restrictions of the API.
 
 -------------------------------- WARNING ------------------------------------
 Including this file is reserved to source files located in the Zend
 subdirectory (outside the Zend directory, it can be included indirectly,
 through another zend_xxx include, but not directly). The macros defined below
 should never be used in any piece of code outside the Zend subdirectory.
============================================================================*/

#ifndef ZEND_STRICT_H
#define ZEND_STRICT_H

#include "php_config.h"

#ifdef ZEND_EXT_CHECK_API
#	define __ZEND_CHECK_API ZEND_EXT_CHECK_API
#else
#	ifdef ZEND_CORE_CHECK_API
#		define __ZEND_CHECK_API ZEND_CORE_CHECK_API
#	else
#		define __ZEND_CHECK_API 0
#	endif
#endif
#if __ZEND_CHECK_API
#	define _ZEND_PROTECTED(_prefix,_elt) _z_check_ ## _prefix ## _ ## _elt
#	define ZEND_CHECK_API
#else
#	define _ZEND_PROTECTED(_prefix,_elt) _elt
#endif
#undef __ZEND_CHECK_API

#ifdef ZEND_EXT_STRICT_API
#	define __ZEND_STRICT_API ZEND_EXT_STRICT_API
#else
#	ifdef ZEND_CORE_STRICT_API
#		define __ZEND_STRICT_API ZEND_CORE_STRICT_API
#	else
#		define __ZEND_STRICT_API 0
#	endif
#endif
#if __ZEND_STRICT_API
#	define _ZEND_PROTECTED_STRICT(_prefix,_elt) _ZEND_PROTECTED(_prefix ## _strict,_elt)
#	define ZEND_STRICT_API
#else
#	define _ZEND_PROTECTED_STRICT(_prefix,_elt) _elt
#endif
#undef __ZEND_STRICT_API

/*-------------------------------------------------------------------------*/
#endif /* ZEND_STRICT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
