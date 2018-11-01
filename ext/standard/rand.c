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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_rand.h"
#include "php_mt_rand.h"

/* {{{ php_srand
 */
PHPAPI void php_srand(zend_long seed)
{
	php_mt_srand(seed);
}
/* }}} */

/* {{{ php_rand
 */
PHPAPI zend_long php_rand(void)
{
	return php_mt_rand();
}
/* }}} */

/* {{{ proto int mt_rand([int min, int max])
   Returns a random number from Mersenne Twister */
PHP_FUNCTION(rand)
{
	zend_long min;
	zend_long max;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		RETURN_LONG(php_mt_rand() >> 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (max < min) {
		RETURN_LONG(php_mt_rand_common(max, min));
	}

	RETURN_LONG(php_mt_rand_common(min, max));
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
