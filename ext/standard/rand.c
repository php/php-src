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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Jeroen van Wolffelaar <jeroen@php.net>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdlib.h>

#include "php.h"
#include "php_math.h"
#include "php_rand.h"
#include "php_lcg.h"
#include "php_ini.h"

#include "zend_execute.h"

#include "basic_functions.h"

/* See php_rand.h for information about layout */

/* an ARRAY of POINTERS, not vice versa */
php_randgen_entry *php_randgen_entries[PHP_RAND_NUMRANDS];

#define PHP_HAS_SRAND(which)	(php_randgen_entries[which] && php_randgen_entries[which]->srand)
#define PHP_SRAND(which,seed)	((*(php_randgen_entries[which]->srand))(seed TSRMLS_CC))
#define PHP_RAND(which)			((*(php_randgen_entries[which]->rand))(TSRMLS_C))
#define PHP_RANDMAX(which)		(php_randgen_entries[which]->randmax)
#define PHP_RAND_INISTR(which)	(php_randgen_entries[which]->ini_str)

#define CURR_GEN BG(rand_generator_current)

PHP_MINIT_FUNCTION(rand)
{
	PHP_MINIT(rand_sys)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(rand_mt)(INIT_FUNC_ARGS_PASSTHRU);
	/* lcg not yet implemented */
	php_randgen_entries[PHP_RAND_LCG] = NULL;

	return SUCCESS;
}

/* TODO: check that this function is called on the start of each script
 * execution: not more often, not less often.
 *
 * Note that system rand is inherently thread-unsafe: A different thread can
 * always eat up some rand()'s, and thus nuking your expected sequence.
 * Another reason to use MT...
 */
PHP_RINIT_FUNCTION(rand)
{
	register int i;

	/* seed all number-generators */
	/* FIXME: or seed relevant numgen on init/update ini-entry? */
	for (i = 0 ; i < PHP_RAND_NUMRANDS ; i++) {
		if (PHP_HAS_SRAND(i)) {
#define SRAND_A_RANDOM_SEED ((long)time(0) * (long)getpid() * (long)(php_combined_lcg(TSRMLS_C) * 10000.0)) /* something with microtime? */
			PHP_SRAND(i,SRAND_A_RANDOM_SEED);
		}
	}

	return SUCCESS;
}

/* INI */
static int randgen_str_to_int(char *str, int strlen)
{
	register int i;

	for (i = 0 ; i < PHP_RAND_NUMRANDS ; i++) {
		if (!strcasecmp(str, PHP_RAND_INISTR(i)))
			return i;
	}
	return -1;
}
	
/* FIXME: check that this is called on initial ini-parsing too */
/* FIXME: what if no ini-entry was present? */
static PHP_INI_MH(OnUpdateRandGen)
{
	/* Set BG(rand_generator) to the correct integer value indicating
	 * ini-setting */
	BG(rand_generator) = randgen_str_to_int(new_value, new_value_length);
	if (BG(rand_generator) == -1) {
		/* FIXME: is this possible? What happens if this occurs during
		 * ini-parsing at startup? */
		php_error(E_WARNING,"Invalid value for random_number_generator: \"%s\"", new_value);
		/* Fallback: */
		BG(rand_generator) = PHP_RAND_DEFAULT;
	}
#ifdef DEBUG_RAND
	printf("\nRAND-INI updated: %d\n",BG(rand_generator));
#endif
	return SUCCESS;
}

PHP_INI_BEGIN()
	/* FIXME: default is hardcoded here, this is the second place */
	PHP_INI_ENTRY("random_number_generator", "mt", PHP_INI_ALL, OnUpdateRandGen)
PHP_INI_END()

/* srand */

/* FIXME: isn't used? */
/* {{{ void php_srand(void) */
static void php_srand(TSRMLS_D)
{
	CURR_GEN = BG(rand_generator);
	PHP_SRAND(BG(rand_generator), SRAND_A_RANDOM_SEED);
}
/* }}} */

/* {{{ [mt_]srand common */
#define pim_srand_common(name,type) 				\
PHP_FUNCTION(name)							\
{											\
	zval **seed;								\
	zval **alg;								\
											\
	switch (ZEND_NUM_ARGS()) {						\
		case 0:										\
			CURR_GEN = BG(rand_generator);	\
			PHP_SRAND(BG(rand_generator), SRAND_A_RANDOM_SEED);	\
			RETURN_TRUE;							\
		case 1:										\
			zend_get_parameters_ex(1, &seed);		\
			convert_to_long_ex(seed);				\
			CURR_GEN = type;		\
			PHP_SRAND(type, Z_LVAL_PP(seed)); 		\
			RETURN_TRUE;							\
		case 2:										\
			/* algorithm, seed is most logic, though it is the other way
			 * around than current way... */		\
			zend_get_parameters_ex(2, &alg, &seed);	\
			convert_to_long_ex(seed);				\
			convert_to_long_ex(alg);				\
			if (0 > Z_LVAL_PP(alg) || Z_LVAL_PP(alg) >= PHP_RAND_NUMRANDS) {	\
				php_error(E_WARNING, "%s(): There is no algorithm %d.", get_active_function_name(TSRMLS_C), Z_LVAL_PP(alg));		\
				RETURN_FALSE;						\
			}										\
			if (!PHP_HAS_SRAND(Z_LVAL_PP(alg))) {	\
				php_error(E_WARNING, "%s(): Algorithm %d does not support reproducable results.", get_active_function_name(TSRMLS_C), Z_LVAL_PP(alg));	\
				RETURN_FALSE;						\
			}										\
			CURR_GEN = Z_LVAL_PP(alg);		\
			PHP_SRAND(Z_LVAL_PP(alg), Z_LVAL_PP(seed)); 		\
			RETURN_TRUE;							\
		default:									\
			WRONG_PARAM_COUNT;						\
	}												\
}
/* }}} */

/* {{{ proto bool srand(int seed)
   Seeds random number generator */
pim_srand_common(srand,PHP_RAND_SYS)
/* }}} */

/* {{{ proto bool mt_srand(int seed)
   Seeds random number generator */
pim_srand_common(mt_srand,PHP_RAND_MT)
/* }}} */

/* rand */

/* {{{ PHPAPI long php_rand(void) */
PHPAPI long php_rand(TSRMLS_D)
{
	return PHP_RAND(CURR_GEN);
}
/* }}} */

/* {{{ PHPAPI double php_drand(void) 
 *      returns a double in the range [0,1) */
PHPAPI double php_drand(TSRMLS_D)
{
	return  (double)php_rand(TSRMLS_C) /
			(double)(PHP_RANDMAX(CURR_GEN)+1.0);
}
/* }}} */

/* {{{ macro: PHP_RAND_RANGE */
#define PHP_RAND_RANGE(which,min,max,result) {  \
    /*
     * A bit of tricky math here.  We want to avoid using a modulus because
     * that simply tosses the high-order bits and might skew the distribution
     * of random values over the range.  Instead we map the range directly.
     *
     * We need to map the range from 0...M evenly to the range a...b
	 * Expressed in real numbers, this becomes:
	 *
	 * 				[0,M+1[ mapped to [a,b+1[
	 * 
     * Let n = the random number and n' = the mapped random number
	 * So the formula needs to be:
	 *
     *               n' = a + n((b+1)-a)/(m+1) 
     *
	 *  This isn't perfect, because n only takes integer values. So when a..b
	 *  spans a significant portion of 0..M, some numbers have nearly twice as
	 *  much chance. But since twice a very small chance is still a very small
	 *  chance, it's ignored.
	 *
	 *  --Rasmus and Jeroen
     */								\
	(result) = PHP_RAND(which);		\
	if ((max) < (min)) {			\
		php_error(E_WARNING, "%s():  Invalid range:  %ld..%ld (minimum can't be larger than maximum)", \
			get_active_function_name(TSRMLS_C), (min), (max)); \
	} else if ( (max) - (min) > PHP_RANDMAX(which) ) { \
		/* TODO: this can done better, get two numbers and combine... */ \
		php_error(E_WARNING, "%s():  Invalid range:  %ld..%ld (can't give that much randomness)",  \
			get_active_function_name(TSRMLS_C), (min), (max)); \
	} \
	(result) = (min) + (long) ((double)((max)-(min)+1) * (result)/(PHP_RANDMAX(which)+1.0)); \
}
/* }}} */

/* {{{ PHPAPI long php_rand_range(long min, long max) */
PHPAPI long php_rand_range(long min, long max TSRMLS_DC)
{
	register long result;

	PHP_RAND_RANGE(CURR_GEN, min, max, result);
	return result;
}
/* }}} */

/* {{{ [mt_]rand common */
#define PHP_FUNCTION_RAND(name,which)								\
PHP_FUNCTION(name)													\
{																	\
	zval **min, **max;												\
																	\
	switch (ZEND_NUM_ARGS()) {										\
		case 0:														\
			RETURN_LONG(PHP_RAND(which));							\
		case 2:														\
			if (zend_get_parameters_ex(2, &min, &max)==FAILURE) {	\
				RETURN_FALSE;										\
			}														\
			convert_to_long_ex(min);								\
			convert_to_long_ex(max);								\
			Z_TYPE_P(return_value) = IS_LONG;						\
			PHP_RAND_RANGE(which, Z_LVAL_PP(min), 					\
					Z_LVAL_PP(max), Z_LVAL_P(return_value));		\
			return;													\
		default:													\
			WRONG_PARAM_COUNT;										\
			break;													\
	}																\
}
/* }}} */

/* {{{ proto int rand([int min, int max]) 
   Returns a random number */
PHP_FUNCTION_RAND(rand,PHP_RAND_SYS)
/* }}} */

/* {{{ proto int mt_rand([int min, int max]) 
   Returns a random number by means of Mersenne Twister */
PHP_FUNCTION_RAND(mt_rand,PHP_RAND_MT)
/* }}} */

/* getrandmax */

/* {{{ PHPAPI long php_randmax(void)
   Returns the maximum value a random number can have */
PHPAPI long php_randmax(TSRMLS_D)
{
	return PHP_RANDMAX(CURR_GEN);
}
/* }}} */

/* {{{ proto int getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(getrandmax)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG( php_randmax(TSRMLS_C));
}
/* }}} */

/* {{{ proto int mt_getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(mt_getrandmax)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG( php_randmax(TSRMLS_C) );
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim: sw=4 ts=4 tw=78
 */
