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
