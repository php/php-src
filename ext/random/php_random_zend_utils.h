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
   | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
   |          Tim Düsterhus <timwolla@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_RANDOM_ZEND_UTILS_H
# define PHP_RANDOM_ZEND_UTILS_H

# include "php.h"
# include "php_random.h"
# include "zend.h"

typedef struct _php_random_bytes_insecure_state_for_zend {
	bool initialized;
	php_random_status_state_xoshiro256starstar xoshiro256starstar_state;
} php_random_bytes_insecure_state_for_zend;

ZEND_STATIC_ASSERT(sizeof(zend_random_bytes_insecure_state) >= sizeof(php_random_bytes_insecure_state_for_zend), "");

ZEND_ATTRIBUTE_NONNULL PHPAPI void php_random_bytes_insecure_for_zend(
		zend_random_bytes_insecure_state *state, void *bytes, size_t size);

#endif /* PHP_RANDOM_ZEND_UTILS_H */
