/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_OVERLOAD_H
#define PHP_OVERLOAD_H

#if HAVE_OVERLOAD

extern zend_module_entry overload_module_entry;
#define phpext_overload_ptr &overload_module_entry

#ifdef PHP_WIN32
#define PHP_OVERLOAD_API __declspec(dllexport)
#else
#define PHP_OVERLOAD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(overload);
PHP_MINFO_FUNCTION(overload);

PHP_FUNCTION(overload);

ZEND_BEGIN_MODULE_GLOBALS(overload)
	HashTable overloaded_classes;
ZEND_END_MODULE_GLOBALS(overload)

#ifdef ZTS
#define OOG(v) TSRMG(overload_globals_id, zend_overload_globals *, v)
#else
#define OOG(v) (overload_globals.v)
#endif

#endif /* HAVE_OVERLOAD */

#endif /* PHP_OVERLOAD_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
