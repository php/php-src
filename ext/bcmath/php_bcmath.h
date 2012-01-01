/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andi Gutmans <andi@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_BCMATH_H
#define PHP_BCMATH_H

#include "libbcmath/src/bcmath.h"

extern zend_module_entry bcmath_module_entry;
#define phpext_bcmath_ptr &bcmath_module_entry

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
	long bc_precision;
ZEND_END_MODULE_GLOBALS(bcmath)
	
#ifdef ZTS
# define BCG(v) TSRMG(bcmath_globals_id, zend_bcmath_globals *, v)
#else
# define BCG(v) (bcmath_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(bcmath)

#endif /* PHP_BCMATH_H */
