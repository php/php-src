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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php_random_zend_utils.h"

ZEND_ATTRIBUTE_NONNULL PHPAPI void php_random_bytes_insecure_for_zend(
		zend_random_bytes_insecure_state *opaque_state, void *bytes, size_t size)
{
	php_random_bytes_insecure_state_for_zend *state = (php_random_bytes_insecure_state_for_zend*) opaque_state;

	if (UNEXPECTED(!state->initialized)) {
		uint64_t t[4];
		php_random_fallback_seed_state fallback_state;
		fallback_state.initialized = false;

		do {
			/* Skip the CSPRNG if it has already failed */
			if (!fallback_state.initialized) {
				char errstr[128];
				if (php_random_bytes_ex(&t, sizeof(t), errstr, sizeof(errstr)) == FAILURE) {
#if ZEND_DEBUG
					fprintf(stderr, "php_random_bytes_ex: Failed to generate a random seed: %s\n", errstr);
#endif
					goto fallback;
				}
			} else {
fallback:
				t[0] = php_random_generate_fallback_seed_ex(&fallback_state);
				t[1] = php_random_generate_fallback_seed_ex(&fallback_state);
				t[2] = php_random_generate_fallback_seed_ex(&fallback_state);
				t[3] = php_random_generate_fallback_seed_ex(&fallback_state);
			}
		} while (UNEXPECTED(t[0] == 0 && t[1] == 0 && t[2] == 0 && t[3] == 0));

		php_random_xoshiro256starstar_seed256(&state->xoshiro256starstar_state, t[0], t[1], t[2], t[3]);
		state->initialized = true;
	}

	while (size > 0) {
		php_random_result result = php_random_algo_xoshiro256starstar.generate(&state->xoshiro256starstar_state);
		ZEND_ASSERT(result.size == 8 && sizeof(result.result) == 8);
		size_t chunk_size = MIN(size, 8);
		bytes = zend_mempcpy(bytes, &result.result, chunk_size);
		size -= chunk_size;
	}
}
