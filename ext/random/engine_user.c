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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"

static uint64_t generate(php_random_status *status)
{
	php_random_status_state_user *s = status->state;
	uint64_t result = 0;
	size_t size;
	zval retval;

	zend_call_known_instance_method_with_0_params(s->generate_method, s->object, &retval);

	if (EG(exception)) {
		return 0;
	}

	/* Store generated size in a state */
	size = Z_STRLEN(retval);

	/* Guard for over 64-bit results */
	if (size > sizeof(uint64_t)) {
		size = sizeof(uint64_t);
	}
	status->last_generated_size = size;

	if (size > 0) {
		/* Endianness safe copy */
		for (size_t i = 0; i < size; i++) {
			result += ((uint64_t) (unsigned char) Z_STRVAL(retval)[i]) << (8 * i);
		}
	} else {
		zend_throw_error(random_ce_Random_BrokenRandomEngineError, "A random engine must return a non-empty string");
		return 0;
	}

	zval_ptr_dtor(&retval);

	return result;
}

static zend_long range(php_random_status *status, zend_long min, zend_long max)
{
	return php_random_range(&php_random_algo_user, status, min, max);
}

const php_random_algo php_random_algo_user = {
	0,
	sizeof(php_random_status_state_user),
	NULL,
	generate,
	range,
	NULL,
	NULL,
};
