/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_lcg.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef ZTS
int lcg_globals_id;
#else
static php_lcg_globals lcg_globals;
#endif

static int php_lcg_initialized = 0;

#ifdef PHP_WIN32
#include <process.h>
#endif

/*
 * combinedLCG() returns a pseudo random number in the range of (0,1).
 * The function combines two CGs with periods of 
 * 2^31 - 85 and 2^31 - 249. The period of this function
 * is equal to the product of both primes.
 */

#define MODMULT(a,b,c,m,s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m

double php_combined_lcg(TSRMLS_D)
{
	php_int32 q;
	php_int32 z;

	MODMULT(53668,40014,12211,2147483563L, LCG(s1));
	MODMULT(52774,40692,3791, 2147483399L, LCG(s2));

	z = LCG(s1) - LCG(s2);
	if(z < 1) {
		z += 2147483562;
	}

	return z * 4.656613e-10;
}

static void lcg_init_globals(php_lcg_globals *lcg_globals_p TSRMLS_DC)
{
	LCG(s1) = 1;
#ifdef ZTS
	LCG(s2) = (long) tsrm_thread_id();
#else
	LCG(s2) = (long) getpid();
#endif
}

#ifdef ZTS
PHP_MINIT_FUNCTION(lcg)
{
	ts_allocate_id(&lcg_globals_id, sizeof(php_lcg_globals), (ts_allocate_ctor) lcg_init_globals, NULL);
	return SUCCESS;
}
#else 
PHP_RINIT_FUNCTION(lcg)
{
	if (!php_lcg_initialized) {
		lcg_init_globals(&lcg_globals TSRMLS_CC);
		php_lcg_initialized = 1;
	}
	return SUCCESS;
}
#endif

/* {{{ proto double lcg_value()
   Returns a value from the combined linear congruential generator */
PHP_FUNCTION(lcg_value)
{
	RETURN_DOUBLE(php_combined_lcg(TSRMLS_C));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
