/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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

static php_random_result generate(void *state)
{
	zend_ulong r = 0;

	php_random_bytes_throw(&r, sizeof(r));

	return (php_random_result){
		.size = sizeof(zend_ulong),
		.result = r,
	};
}

static zend_long range(void *state, zend_long min, zend_long max)
{
	zend_long result = 0;

	php_random_int_throw(min, max, &result);

	return result;
}

PHPAPI const php_random_algo php_random_algo_secure = {
	0,
	generate,
	range,
	NULL,
	NULL
};
