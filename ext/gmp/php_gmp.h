/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

#if HAVE_GMP

#include <gmp.h>

extern zend_module_entry gmp_module_entry;
#define phpext_gmp_ptr &gmp_module_entry

ZEND_MODULE_STARTUP_D(gmp);
ZEND_MODULE_DEACTIVATE_D(gmp);
ZEND_MODULE_INFO_D(gmp);

ZEND_FUNCTION(gmp_init);
ZEND_FUNCTION(gmp_import);
ZEND_FUNCTION(gmp_export);
ZEND_FUNCTION(gmp_intval);
ZEND_FUNCTION(gmp_strval);
ZEND_FUNCTION(gmp_add);
ZEND_FUNCTION(gmp_sub);
ZEND_FUNCTION(gmp_mul);
ZEND_FUNCTION(gmp_div_qr);
ZEND_FUNCTION(gmp_div_q);
ZEND_FUNCTION(gmp_div_r);
ZEND_FUNCTION(gmp_mod);
ZEND_FUNCTION(gmp_divexact);
ZEND_FUNCTION(gmp_neg);
ZEND_FUNCTION(gmp_abs);
ZEND_FUNCTION(gmp_fact);
ZEND_FUNCTION(gmp_sqrt);
ZEND_FUNCTION(gmp_sqrtrem);
ZEND_FUNCTION(gmp_root);
ZEND_FUNCTION(gmp_rootrem);
ZEND_FUNCTION(gmp_pow);
ZEND_FUNCTION(gmp_powm);
ZEND_FUNCTION(gmp_perfect_square);
ZEND_FUNCTION(gmp_prob_prime);
ZEND_FUNCTION(gmp_gcd);
ZEND_FUNCTION(gmp_gcdext);
ZEND_FUNCTION(gmp_invert);
ZEND_FUNCTION(gmp_jacobi);
ZEND_FUNCTION(gmp_legendre);
ZEND_FUNCTION(gmp_cmp);
ZEND_FUNCTION(gmp_sign);
ZEND_FUNCTION(gmp_and);
ZEND_FUNCTION(gmp_or);
ZEND_FUNCTION(gmp_com);
ZEND_FUNCTION(gmp_xor);
ZEND_FUNCTION(gmp_random);
ZEND_FUNCTION(gmp_random_seed);
ZEND_FUNCTION(gmp_random_bits);
ZEND_FUNCTION(gmp_random_range);
ZEND_FUNCTION(gmp_setbit);
ZEND_FUNCTION(gmp_clrbit);
ZEND_FUNCTION(gmp_scan0);
ZEND_FUNCTION(gmp_scan1);
ZEND_FUNCTION(gmp_testbit);
ZEND_FUNCTION(gmp_popcount);
ZEND_FUNCTION(gmp_hamdist);
ZEND_FUNCTION(gmp_nextprime);

/* GMP and MPIR use different datatypes on different platforms */
#ifdef PHP_WIN32
typedef zend_long gmp_long;
typedef zend_ulong gmp_ulong;
#else
typedef long gmp_long;
typedef unsigned long gmp_ulong;
#endif

ZEND_BEGIN_MODULE_GLOBALS(gmp)
	zend_bool rand_initialized;
	gmp_randstate_t rand_state;
ZEND_END_MODULE_GLOBALS(gmp)

#ifdef ZTS
#define GMPG(v) ZEND_TSRMG(gmp_globals_id, zend_gmp_globals *, v)
#ifdef COMPILE_DL_GMP
ZEND_TSRMLS_CACHE_EXTERN;
#endif
#else
#define GMPG(v) (gmp_globals.v)
#endif

#else

#define phpext_gmp_ptr NULL

#endif

#endif	/* PHP_GMP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
