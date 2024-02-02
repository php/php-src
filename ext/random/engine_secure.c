/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sammy Kaye Powers <me@sammyk.me>                            |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"
#include "php_random_csprng.h"

#include "Zend/zend_exceptions.h"

static php_random_result generate(php_random_status *status)
{
	zend_ulong r = 0;

	php_random_bytes_throw(&r, sizeof(zend_ulong));

	return (php_random_result){
		.size = sizeof(zend_ulong),
		.result = r,
	};
}

static zend_long range(php_random_status *status, zend_long min, zend_long max)
{
	zend_long result = 0;

	php_random_int_throw(min, max, &result);

	return result;
}

const php_random_algo php_random_algo_secure = {
	0,
	NULL,
	generate,
	range,
	NULL,
	NULL
};
