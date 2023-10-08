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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_libsodium.h"
#include "ext/standard/php_password.h"

#include <sodium.h>

#if SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6)

/**
 * MEMLIMIT is normalized to KB even though sodium uses Bytes in order to
 * present a consistent user-facing API.
 *
 * Threads are fixed at 1 by libsodium.
 *
 * When updating these values, synchronize ext/standard/php_password.h values.
 */
#define PHP_SODIUM_PWHASH_MEMLIMIT (64 << 10)
#define PHP_SODIUM_PWHASH_OPSLIMIT 4
#define PHP_SODIUM_PWHASH_THREADS 1

static inline int get_options(zend_array *options, size_t *memlimit, size_t *opslimit) {
	zval *opt;

	*opslimit = PHP_SODIUM_PWHASH_OPSLIMIT;
	*memlimit = PHP_SODIUM_PWHASH_MEMLIMIT << 10;
	if (!options) {
		return SUCCESS;
	}
	if ((opt = zend_hash_str_find(options, "memory_cost", strlen("memory_cost")))) {
		zend_long smemlimit = zval_get_long(opt);

		if ((smemlimit < 0) || (smemlimit < crypto_pwhash_MEMLIMIT_MIN >> 10) || (smemlimit > (crypto_pwhash_MEMLIMIT_MAX >> 10))) {
			zend_value_error("Memory cost is outside of allowed memory range");
			return FAILURE;
		}
		*memlimit = smemlimit << 10;
	}
	if ((opt = zend_hash_str_find(options, "time_cost", strlen("time_cost")))) {
		*opslimit = zval_get_long(opt);
		if ((*opslimit < crypto_pwhash_OPSLIMIT_MIN) || (*opslimit > crypto_pwhash_OPSLIMIT_MAX)) {
			zend_value_error("Time cost is outside of allowed time range");
			return FAILURE;
		}
	}
	if ((opt = zend_hash_str_find(options, "threads", strlen("threads"))) && (zval_get_long(opt) != 1)) {
		zend_value_error("A thread value other than 1 is not supported by this implementation");
		return FAILURE;
	}
	return SUCCESS;
}

static zend_string *php_sodium_argon2_hash(const zend_string *password, zend_array *options, int alg) {
	size_t opslimit, memlimit;
	zend_string *ret;

	if ((ZSTR_LEN(password) >= 0xffffffff)) {
		zend_value_error("Password is too long");
		return NULL;
	}

	if (get_options(options, &memlimit, &opslimit) == FAILURE) {
		return NULL;
	}

	ret = zend_string_alloc(crypto_pwhash_STRBYTES - 1, 0);
	if (crypto_pwhash_str_alg(ZSTR_VAL(ret), ZSTR_VAL(password), ZSTR_LEN(password), opslimit, memlimit, alg)) {
		zend_value_error("Unexpected failure hashing password");
		zend_string_release(ret);
		return NULL;
	}

	ZSTR_LEN(ret) = strlen(ZSTR_VAL(ret));
	ZSTR_VAL(ret)[ZSTR_LEN(ret)] = 0;

	return ret;
}

static bool php_sodium_argon2_verify(const zend_string *password, const zend_string *hash) {
	if ((ZSTR_LEN(password) >= 0xffffffff) || (ZSTR_LEN(hash) >= 0xffffffff)) {
		return 0;
	}
	return crypto_pwhash_str_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password)) == 0;
}

static bool php_sodium_argon2_needs_rehash(const zend_string *hash, zend_array *options) {
	size_t opslimit, memlimit;

	if (get_options(options, &memlimit, &opslimit) == FAILURE) {
		return 1;
	}
	return crypto_pwhash_str_needs_rehash(ZSTR_VAL(hash), opslimit, memlimit);
}

static int php_sodium_argon2_get_info(zval *return_value, const zend_string *hash) {
	const char* p = NULL;
	zend_long v = 0, threads = 1;
	zend_long memory_cost = PHP_SODIUM_PWHASH_MEMLIMIT;
	zend_long time_cost = PHP_SODIUM_PWHASH_OPSLIMIT;

	if (!hash || (ZSTR_LEN(hash) < sizeof("$argon2id$"))) {
		return FAILURE;
	}

	p = ZSTR_VAL(hash);
	if (!memcmp(p, "$argon2i$", strlen("$argon2i$"))) {
		p += strlen("$argon2i$");
	} else if (!memcmp(p, "$argon2id$", strlen("$argon2id$"))) {
		p += strlen("$argon2id$");
	} else {
		return FAILURE;
	}

	sscanf(p, "v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT,
	       &v, &memory_cost, &time_cost, &threads);

	add_assoc_long(return_value, "memory_cost", memory_cost);
	add_assoc_long(return_value, "time_cost", time_cost);
	add_assoc_long(return_value, "threads", threads);

	return SUCCESS;
}

/* argon2i specific methods */

static zend_string *php_sodium_argon2i_hash(const zend_string *password, zend_array *options) {
	return php_sodium_argon2_hash(password, options, crypto_pwhash_ALG_ARGON2I13);
}

static const php_password_algo sodium_algo_argon2i = {
	"argon2i",
	php_sodium_argon2i_hash,
	php_sodium_argon2_verify,
	php_sodium_argon2_needs_rehash,
	php_sodium_argon2_get_info,
	NULL,
};

/* argon2id specific methods */

static zend_string *php_sodium_argon2id_hash(const zend_string *password, zend_array *options) {
	return php_sodium_argon2_hash(password, options, crypto_pwhash_ALG_ARGON2ID13);
}

static const php_password_algo sodium_algo_argon2id = {
	"argon2id",
	php_sodium_argon2id_hash,
	php_sodium_argon2_verify,
	php_sodium_argon2_needs_rehash,
	php_sodium_argon2_get_info,
	NULL,
};

PHP_MINIT_FUNCTION(sodium_password_hash) /* {{{ */ {
	zend_string *argon2i = ZSTR_INIT_LITERAL("argon2i", 1);

	if (php_password_algo_find(argon2i)) {
		/* Nothing to do. Core has registered these algorithms for us. */
		zend_string_release(argon2i);
		return SUCCESS;
	}
	zend_string_release(argon2i);

	if (FAILURE == php_password_algo_register("argon2i", &sodium_algo_argon2i)) {
		return FAILURE;
	}
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2I", "argon2i", CONST_PERSISTENT);

	if (FAILURE == php_password_algo_register("argon2id", &sodium_algo_argon2id)) {
		return FAILURE;
	}
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2ID", "argon2id", CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_MEMORY_COST", PHP_SODIUM_PWHASH_MEMLIMIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_TIME_COST", PHP_SODIUM_PWHASH_OPSLIMIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_THREADS", PHP_SODIUM_PWHASH_THREADS, CONST_PERSISTENT);

	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2_PROVIDER", "sodium", CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */


#endif /* HAVE_SODIUM_PASSWORD_HASH */
