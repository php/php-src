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
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdlib.h>

#include "php.h"
#include "php_math.h"
#include "php_rand.h"
#include "php_ini.h"

#include "zend_execute.h"

#include "basic_functions.h"

/* See php_rand.h for information about layout */

#define SRAND_A_RANDOM_SEED (time(0) * getpid() * (php_combined_lcg(TSRMLS_C) * 10000.0)) /* something with microtime? */

/* TODO: check that this function is called on the start of each script
 * execution: not more often, not less often.
 *
 * Note that system rand is inherently thread-unsafe: A different thread can
 * always eat up some rand()'s, and thus nuking your expected sequence.
 * Another reason to use MT...
 */
PHP_RINIT_FUNCTION(rand)
{
	/* seed all number-generators */
	/* FIXME: or seed relevant numgen on init/update ini-entry? */
	php_srand_sys(SRAND_A_RANDOM_SEED);
	php_srand_mt(SRAND_A_RANDOM_SEED);
}

/* INI */
static int randgen_str_to_int(char *str, int strlen)
{
	/* manually check all cases, or some loop to automate this
	 * kind of stuff, so that a new random number generator
	 * can be added more easily?
	 *
	 * --jeroen
	 */
	if (!strcasecmp(str,RAND_SYS_STR)) {
		return RAND_SYS;
	} else if (!strcasecmp(str,RAND_MT_STR)) {
		return RAND_MT;
	} else if (!strcasecmp(str,RAND_LCG_STR)) {
		return RAND_LCG;
	}
	return 0; /* FIXME: include that f*** .h that has FALSE */
}
	
/* FIXME: check that this is called on initial ini-parsing too */
/* FIXME: what if no ini-entry was present? */
static PHP_INI_MH(OnUpdateRandGen)
{
	/* Set BG(rand_generator) to the correct integer value indicating
	 * ini-setting */
	BG(rand_generator) = randgen_str_to_int(new_value, new_value_length);
	if (!BG(rand_generator)) {
		/* FIXME: is this possible? What happens if this occurs during
		 * ini-parsing at startup? */
		php_error(E_WARNING,"Invalid value for random_number_generator: \"%s\"", new_value);
		/* Fallback: */
		BG(rand_generator) = RAND_DEFAULT;
	}
#ifdef DEBUG_RAND
	printf("\nRAND-INI updated: %d\n",BG(rand_generator));
#endif
	return SUCCESS;
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY("random_number_generator",	RAND_DEFAULT_STR,	PHP_INI_ALL,		OnUpdateRandGen)
PHP_INI_END()

/* srand */

/* {{{ PHPAPI void php_srand(void) */
PHPAPI void php_srand(void)
{
	/* php_srand_2( 1e6d * microtime() , protocol-specified-in-php.ini ) */
	php_error(E_ERROR,"Not yet implemented");
}
/* }}} */

/* {{{ void php_srand2(long seed, int alg) */
static inline void php_srand2(long seed, int alg)
{
	switch (alg) {
		case RAND_SYS:
			php_srand_sys(seed);
			return;
		case RAND_MT:
			php_srand_mt(seed TSRMLS_CC);
			return;
		default:
			php_error(E_ERROR,"Bug, please report (php_srand2-%d)",alg);
			return;
	}
}
/* }}} */

/* {{{ [mt_]srand common */
#define pim_srand_common(name,type) 				\
PHP_FUNCTION(name)							\
{											\
	zval **arg;								\
											\
	if (ZEND_NUM_ARGS() != 1) {				\
		WRONG_PARAM_COUNT;					\
	}										\
	zend_get_parameters_ex(1, &arg);		\
	convert_to_long_ex(arg);				\
											\
	php_srand2((*arg)->value.lval, type); 	\
}
/* }}} */

/* {{{ proto void srand(int seed)
   Seeds random number generator */
pim_srand_common(srand,RAND_SYS)
/* }}} */

/* {{{ proto void mt_srand(int seed)
   Seeds random number generator */
pim_srand_common(mt_srand,RAND_MT)
/* }}} */

/* rand */

/* {{{ PHPAPI long php_rand(void) */
PHPAPI long php_rand(void)
{
	/* algorithm = BG(current_alg) */
	return php_rand_sys();
}
/* }}} */

/* {{{ macro: php_map_a_range */
#define php_map_a_range(number,min,max,MAX) {  \
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
     */                            \
	if ((max) < (min)) {		\
		php_error(E_WARNING, "%s():  Invalid range:  %ld..%ld (minimum can't be larger than maximum)", \
			get_active_function_name(TSRMLS_C), (min), (max)); \
	} else if ( (max) - (min) > (MAX) ) { \
		/* TODO: this can done better, get two numbers and combine... */ \
		php_error(E_WARNING, "%s():  Invalid range:  %ld..%ld (can't give that much randomness)",  \
			get_active_function_name(TSRMLS_C), (min), (max)); \
	} \
	(number) = (min) + (int) ((double)((max)-(min)+1) * (number)/(MAX+1.0)); \
}
/* }}} */

/* {{{ long php_rand_range2(long min, long max, int alg) 
 *  Temporary hack function */
static inline long php_rand_range2(long min, long max, int alg)
{
	long number;
	
	switch (alg) {
		case RAND_SYS:
			number = php_rand_sys();
			php_map_a_range(number,min,max,php_randmax_sys());
			return number;
		case RAND_MT:
			number = php_rand_mt();
			php_map_a_range(number,min,max,php_randmax_mt());
			return number;
		default:
			php_error(E_ERROR,"Bug, please report (php_rand_range2-%d)",alg);
			return;
	}
}
/* }}} */

/* {{{ PHPAPI long php_rand_range(long min, long max) */
PHPAPI long php_rand_range(long min, long max)
{
	/* will be php.ini & srand aware */
	return php_rand_range2(min,max,RAND_SYS);
}
/* }}} */

/* {{{ [mt_]rand common */
#define PHP_FUNCTION_RAND(name,type,lowertype)   					\
PHP_FUNCTION(name)													\
{																	\
	zval **min, **max;												\
																	\
	switch (ZEND_NUM_ARGS()) {										\
		case 0:														\
			RETURN_LONG(php_rand_##lowertype());					\
		case 2:														\
			if (zend_get_parameters_ex(2, &min, &max)==FAILURE) {	\
				RETURN_FALSE;										\
			}														\
			convert_to_long_ex(min);								\
			convert_to_long_ex(max);								\
			RETURN_LONG(php_rand_range2(Z_LVAL_PP(min), 			\
						Z_LVAL_PP(max), type));				\
		default:													\
			WRONG_PARAM_COUNT;										\
			break;													\
	}																\
}
/* }}} */

/* {{{ proto int rand([int min, int max]) 
   Returns a random number */
PHP_FUNCTION_RAND(rand,RAND_SYS,sys)
/* }}} */

/* {{{ proto int mt_rand([int min, int max]) 
   Returns a random number by means of Mersenne Twister */
PHP_FUNCTION_RAND(mt_rand,RAND_MT,mt)
/* }}} */

/* getrandmax */

/* {{{ PHPAPI long php_randmax(void)
   Returns the maximum value a random number can have */
PHPAPI long php_randmax(void)
{
	/* Get current algorithm */
	return php_randmax_sys();
}
/* }}} */

/* {{{ proto int getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(getrandmax)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG( php_randmax_sys() );
}
/* }}} */

/* {{{ proto int mt_getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(mt_getrandmax)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG( php_randmax_mt() );
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
