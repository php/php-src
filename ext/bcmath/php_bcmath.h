/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#if WITH_BCMATH

#include "libbcmath/src/bcmath.h"

ZEND_BEGIN_MODULE_GLOBALS(bcmath)
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
ZEND_END_MODULE_GLOBALS(bcmath)
	
#if ZTS
# define BCG(v) TSRMG(bcmath_globals_id, zend_bcmath_globals *, v)
extern int bcmath_globals_id;
#else
# define BCG(v) (bcmath_globals.v)
extern zend_bcmath_globals bcmath_globals;
#endif

#define BC

extern zend_module_entry bcmath_module_entry;
#define phpext_bcmath_ptr &bcmath_module_entry

#if ZTS
PHP_MINIT_FUNCTION(bcmath);
#endif
PHP_RINIT_FUNCTION(bcmath);
PHP_RSHUTDOWN_FUNCTION(bcmath);
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
PHP_FUNCTION(bc_powmod);

#else

#define phpext_bcmath_ptr NULL

#endif

#endif /* PHP_BCMATH_H */
