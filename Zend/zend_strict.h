/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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
 This file implements the '--enable-maintainer-strict-api' configure option.

 When this option is set, the names of the protected structure fields (those
 defined using using the _ZSTRICT_FIELD() macro) are modified, causing
 a compile failure when code attempts a direct access to the structure
 fields. This allows to detect violations of the published API.

 -------------------------------- WARNING ------------------------------------
 Including this file is reserved to source files located in the Zend
 subdirectory. Outside the Zend directory, this file can be included indirectly,
 through another zend_xxx include, but not directly. The macros defined below
 should never be used in any piece of code outside the Zend subdirectory.
============================================================================*/

#ifndef ZEND_STRICT_H
#define ZEND_STRICT_H

#include "php_config.h"

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
#	define _ZSTRICT_NAME(_type,_prefix,_elt) _zstrict_ ## _type ## _ ## _prefix ## _ ## _elt
#	define _ZSTRICT_FIELD(_prefix,_elt) _ZSTRICT_NAME(field,_prefix,_elt)
#	define ZEND_STRICT_API
#else
#	define _ZSTRICT_FIELD(_prefix,_elt) _elt
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
