/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andi Gutmans <andi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_BCMATH_H
#define PHP_BCMATH_H

#include "libbcmath/src/bcmath.h"

extern zend_module_entry bcmath_module_entry;
#define phpext_bcmath_ptr &bcmath_module_entry

#include "php_version.h"
#define PHP_BCMATH_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(bcmath);
PHP_MSHUTDOWN_FUNCTION(bcmath);
PHP_MINFO_FUNCTION(bcmath);

PHP_FUNCTION(bcadd);
PHP_FUNCTION(bcsub);
PHP_FUNCTION(bcmul);
PHP_FUNCTION(bcdiv);
PHP_FUNCTION(bcmod);
PHP_FUNCTION(bcpow);
PHP_FUNCTION(bcsqrt);
PHP_FUNCTION(bccomp);
PHP_FUNCTION(bcscale);
PHP_FUNCTION(bcpowmod);

ZEND_BEGIN_MODULE_GLOBALS(bcmath)
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
	zend_long bc_precision;
ZEND_END_MODULE_GLOBALS(bcmath)

#if defined(ZTS) && defined(COMPILE_DL_BCMATH)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(bcmath)
#define BCG(v) ZEND_MODULE_GLOBALS_ACCESSOR(bcmath, v)

#endif /* PHP_BCMATH_H */
