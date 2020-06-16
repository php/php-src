/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stanislav Malyshev <stas@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_GMP_H
#define PHP_GMP_H

#include <gmp.h>

extern zend_module_entry gmp_module_entry;
#define phpext_gmp_ptr &gmp_module_entry

#include "php_version.h"
#define PHP_GMP_VERSION PHP_VERSION

ZEND_MODULE_STARTUP_D(gmp);
ZEND_MODULE_DEACTIVATE_D(gmp);
ZEND_MODULE_INFO_D(gmp);

ZEND_BEGIN_MODULE_GLOBALS(gmp)
	zend_bool rand_initialized;
	gmp_randstate_t rand_state;
ZEND_END_MODULE_GLOBALS(gmp)

#define GMPG(v) ZEND_MODULE_GLOBALS_ACCESSOR(gmp, v)

#if defined(ZTS) && defined(COMPILE_DL_GMP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_GMP_H */
