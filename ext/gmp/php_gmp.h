/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_GMP_H
#define PHP_GMP_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_GMP

extern zend_module_entry gmp_module_entry;
#define phpext_gmp_ptr &gmp_module_entry

#ifdef ZEND_WIN32
#define GMP_API __declspec(dllexport)
#else
#define GMP_API
#endif

ZEND_MINIT_FUNCTION(gmp);
ZEND_MSHUTDOWN_FUNCTION(gmp);
ZEND_MINFO_FUNCTION(gmp);

ZEND_FUNCTION(gmp_init);
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
ZEND_FUNCTION(gmp_pow);
ZEND_FUNCTION(gmp_powm);
ZEND_FUNCTION(gmp_sqrtrem);
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
ZEND_FUNCTION(gmp_setbit);
ZEND_FUNCTION(gmp_clrbit);
ZEND_FUNCTION(gmp_scan0);
ZEND_FUNCTION(gmp_scan1);
ZEND_FUNCTION(gmp_popcount);
ZEND_FUNCTION(gmp_hamdist);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(gmp)
	int global_variable;
ZEND_END_MODULE_GLOBALS(gmp)
*/

/* In every function that needs to use variables in php_gmp_globals,
   do call GMPLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as GMPG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define GMPG(v) (gmp_globals->v)
#define GMPLS_FETCH() php_gmp_globals *gmp_globals = ts_resource(gmp_globals_id)
#else
#define GMPG(v) (gmp_globals.v)
#define GMPLS_FETCH()
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
