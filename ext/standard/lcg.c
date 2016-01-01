/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_lcg.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef PHP_WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#ifdef ZTS
int lcg_globals_id;
#else
static php_lcg_globals lcg_globals;
#endif

#ifdef PHP_WIN32
#include <process.h>
#endif

/*
 * combinedLCG() returns a pseudo random number in the range of (0, 1).
 * The function combines two CGs with periods of
 * 2^31 - 85 and 2^31 - 249. The period of this function
 * is equal to the product of both primes.
 */

#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m

static void lcg_seed(TSRMLS_D);

PHPAPI double php_combined_lcg(TSRMLS_D) /* {{{ */
{
	php_int32 q;
	php_int32 z;

	if (!LCG(seeded)) {
		lcg_seed(TSRMLS_C);
	}

	MODMULT(53668, 40014, 12211, 2147483563L, LCG(s1));
	MODMULT(52774, 40692, 3791, 2147483399L, LCG(s2));

	z = LCG(s1) - LCG(s2);
	if (z < 1) {
		z += 2147483562;
	}

	return z * 4.656613e-10;
}
/* }}} */

static void lcg_seed(TSRMLS_D) /* {{{ */
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == 0) {
		LCG(s1) = tv.tv_sec ^ (tv.tv_usec<<11);
	} else {
		LCG(s1) = 1;
	}
#ifdef ZTS
	LCG(s2) = (long) tsrm_thread_id();
#else
	LCG(s2) = (long) getpid();
#endif

	/* Add entropy to s2 by calling gettimeofday() again */
	if (gettimeofday(&tv, NULL) == 0) {
		LCG(s2) ^= (tv.tv_usec<<11);
	}

	LCG(seeded) = 1;
}
/* }}} */

static void lcg_init_globals(php_lcg_globals *lcg_globals_p TSRMLS_DC) /* {{{ */
{
	LCG(seeded) = 0;
}
/* }}} */

PHP_MINIT_FUNCTION(lcg) /* {{{ */
{
#ifdef ZTS
	ts_allocate_id(&lcg_globals_id, sizeof(php_lcg_globals), (ts_allocate_ctor) lcg_init_globals, NULL);
#else
	lcg_init_globals(&lcg_globals);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ proto float lcg_value()
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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
