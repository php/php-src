/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_rand.h"

/* SYSTEM RAND FUNCTIONS */

/* {{{ php_srand
 */
PHPAPI void php_srand(zend_long seed)
{
#ifdef ZTS
	BG(rand_seed) = (unsigned int) seed;
#else
# if defined(HAVE_SRANDOM)
	srandom((unsigned int) seed);
# elif defined(HAVE_SRAND48)
	srand48(seed);
# else
	srand((unsigned int) seed);
# endif
#endif

	/* Seed only once */
	BG(rand_is_seeded) = 1;
}
/* }}} */

/* {{{ php_rand
 */
PHPAPI zend_long php_rand(void)
{
	zend_long ret;

	if (!BG(rand_is_seeded)) {
		php_srand(GENERATE_SEED());
	}

#ifdef ZTS
	ret = php_rand_r(&BG(rand_seed));
#else
# if defined(HAVE_RANDOM)
	ret = random();
# elif defined(HAVE_LRAND48)
	ret = lrand48();
# else
	ret = rand();
# endif
#endif

	return ret;
}
/* }}} */

/* {{{ proto void srand([int seed])
   Seeds random number generator */
PHP_FUNCTION(srand)
{
	zend_long seed = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &seed) == FAILURE)
		return;

	if (ZEND_NUM_ARGS() == 0)
		seed = GENERATE_SEED();

	php_srand(seed);
}
/* }}} */

/* {{{ proto int rand([int min, int max])
   Returns a random number */
PHP_FUNCTION(rand)
{
	zend_long min;
	zend_long max;
	zend_long number;
	int  argc = ZEND_NUM_ARGS();

	if (argc != 0 && zend_parse_parameters(argc, "ll", &min, &max) == FAILURE)
		return;

	number = php_rand();
	if (argc == 2) {
		RAND_RANGE(number, min, max, PHP_RAND_MAX);
	}

	RETURN_LONG(number);
}
/* }}} */

/* {{{ proto int getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(getrandmax)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(PHP_RAND_MAX);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
