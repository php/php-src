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
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   |          Charles R. Portwood II <charlesportwoodii@erianna.com>      |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>

#include "php.h"

#include "fcntl.h"
#include "php_password.h"
#include "php_crypt.h"
#include "base64.h"
#include "zend_interfaces.h"
#include "info.h"
#include "ext/random/php_random_csprng.h"
#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha3.h"
#include "password_arginfo.h"
#ifdef HAVE_ARGON2LIB
#include "argon2.h"
#endif

#ifdef PHP_WIN32
#include "win32/winutil.h"
#endif

static zend_array php_password_algos;

int php_password_algo_register(const char *ident, const php_password_algo *algo) {
	zend_string *key = zend_string_init_interned(ident, strlen(ident), 1);
	return zend_hash_add_ptr(&php_password_algos, key, (void *) algo) ? SUCCESS : FAILURE;
}

void php_password_algo_unregister(const char *ident) {
	zend_hash_str_del(&php_password_algos, ident, strlen(ident));
}

static bool php_password_make_salt(unsigned char *salt, size_t length) /* {{{ */
{
	/* 64 ought to be enough for anybody */
	if (length > 64) {
		zend_value_error("Length is too large to safely generate");
		return false;
	}

	if (FAILURE == php_random_bytes_throw(salt, length)) {
		zend_value_error("Unable to generate salt");
		return false;
	}

	return true;
}
/* }}} */

/* bcrypt implementation */
#define PHP_BCRYPT_SALT_SIZE 16

static bool php_password_b64_from_bcrypt(const char *in, char *out, const size_t len) /* }}} */
{
	/* If it doesn't match bcrypt hash size, something's wrong.
	 * Salt cannot be converted back, because bcrypt discards the 128th bit */
	if (len != 31) {
		zend_value_error("Unexpected bcrypt size encountered: " ZEND_LONG_FMT, len);
		return false;
	}

	for (size_t i = 0; i < len; i++) {
		char c = in[i];
		/* bcrypt: ./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
		 * phcb64: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
		 * This is rather cryptic, but simpler than having tables. */
		if (c == '.' || c == '/') {
			out[i] = c + 19;
		} else if ((c >= 'A' && c <= 'X') || (c >= 'a' && c <= 'x') || (c >= '0' && c <= '7')) {
			out[i] = c + 2;
		} else if (c == 'Y' || c == 'Z') {
			out[i] = c + 8;
		} else if (c == 'y' || c == 'z') {
			out[i] = c - 73;
		} else if (c == '8') {
			out[i] = '+';
		} else if (c == '9') {
			out[i] = '/';
		} else {
			zend_value_error("Invalid bcrypt base64 character encountered: %c", c);
			return false;
		}
	}

	out[len] = 0;
	return true;
}
/* }}} */

static bool php_password_bcrypt_from_b64(const char *in, char *out, const size_t len) /* }}} */
{
	/* If it doesn't match bcrypt salt/hash size, something's wrong */
	if (len != 22 && len != 31) {
		zend_value_error("Unexpected bcrypt size encountered: " ZEND_LONG_FMT, len);
		return false;
	}

	for (size_t i = 0; i < len; i++) {
		char c = in[i];
		/* phcb64: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
		 * bcrypt: ./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
		 * This is rather cryptic, but simpler than having tables. */
		if (c == 'A' || c == 'B') {
			out[i] = c - 19;
		} else if ((c >= 'C' && c <= 'Z') || (c >= 'c' && c <= 'z') || (c >= '2' && c <= '9')) {
			out[i] = c - 2;
		} else if (c == 'a' || c == 'b') {
			out[i] = c - 8;
		} else if (c == '0' || c == '1') {
			out[i] = c + 73;
		} else if (c == '+') {
			out[i] = '8';
		} else if (c == '/') {
			out[i] = '9';
		} else {
			zend_value_error("Invalid base64 character encountered: %c", c);
			return false;
		}
	}

	/* The size boundaries work out in such a way that the hash is fully packed, but
	 * the salt is clamped by bcrypt and needs post-processing. */
	if (len == 22) {
		char c = out[21];
		if (c == '/' || (c >= 'A' && c <= 'N')) {
			out[21] = '.';
		} else if (c >= 'P' && c <= 'd') {
			out[21] = 'O';
		} else if (c >= 'f' && c <= 't') {
			out[21] = 'e';
		} else if (c >= 'v' || ZEND_IS_DIGIT(c)) {
			out[21] = 'u';
		}
	}

	out[len] = 0;
	return true;
}
/* }}} */

static bool php_password_bcrypt_get_cost(zend_array *options, zend_long *cost) {
	zval *zcost;
	if (options && (zcost = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
		*cost = zval_get_long(zcost);
	}

	return *cost >= 4 && *cost <= 31;
}

static bool php_password_bcrypt_parse(const zend_string *hash, zend_long *cost) {
	unsigned int c = 0;
	int r = 0;
	const char *h = ZSTR_VAL(hash);

	if (ZSTR_LEN(hash) == 60 && ZEND_IS_DIGIT(h[4]) && ZEND_IS_DIGIT(h[5]) &&
		1 == sscanf(ZSTR_VAL(hash), "$2y$%2u$%*21[./A-Za-z0-9]%*1[.Oeu]%*31[./A-Za-z0-9]%n", &c, &r) &&
		r == 60 && c >= 4 && c <= 31
	) {
		if (cost != NULL) {
			*cost = (zend_long) c;
		}
		return true;
	}

	return false;
}

static bool php_password_bcrypt_valid(const zend_string *hash) {
	return php_password_bcrypt_parse(hash, NULL);
}

static int php_password_bcrypt_get_info(zval *return_value, const zend_string *hash) {
	zend_long cost;

	if (!php_password_bcrypt_parse(hash, &cost)) {
		/* Should never get called this way. */
		return FAILURE;
	}
	add_assoc_long(return_value, "cost", cost);

	return SUCCESS;
}

static bool php_password_bcrypt_needs_rehash(const zend_string *hash, zend_array *options) {
	zend_long old_cost;
	zend_long new_cost = PHP_PASSWORD_BCRYPT_COST;

	if (!php_password_bcrypt_parse(hash, &old_cost)) {
		/* Should never get called this way. */
		return 1;
	}

	php_password_bcrypt_get_cost(options, &new_cost);

	return old_cost != new_cost;
}

static bool php_password_bcrypt_verify(const zend_string *password, const zend_string *hash) {
	int status = 0;
	zend_string *ret = php_crypt(ZSTR_VAL(password), (int)ZSTR_LEN(password), ZSTR_VAL(hash), (int)ZSTR_LEN(hash), 1);

	if (!ret) {
		return false;
	}

	if (ZSTR_LEN(hash) < 13) {
		zend_string_free(ret);
		return false;
	}

	/* We're using this method instead of == in order to provide
	 * resistance towards timing attacks. This is a constant time
	 * equality check that will always check every byte of both
	 * values. */
	status = php_safe_bcmp(ret, hash);

	zend_string_free(ret);
	return status == 0;
}

/* bcrypt-wrap needs the salt during prehash, so yes the parameter here is necessary */
static zend_string* php_password_bcrypt_hash_internal(const zend_string *password, const unsigned char (*salt)[PHP_BCRYPT_SALT_SIZE], const uint8_t cost) /* {{{ */
{
	char hash_prefix[30];
	zend_string *hash, *salt64;

	/* It's already validated */
	ZEND_ASSERT(cost >= 4 && cost <= 31);

	salt64 = php_base64_encode_ex((const unsigned char*) salt, PHP_BCRYPT_SALT_SIZE, PHP_BASE64_NO_PADDING);
	ZEND_ASSERT(salt64 != NULL && ZSTR_LEN(salt64) == 22);

	if (!php_password_bcrypt_from_b64(ZSTR_VAL(salt64), ZSTR_VAL(salt64), 22)) {
		zend_string_free(salt64);
		return NULL;
	}

	snprintf(hash_prefix, sizeof(hash_prefix), "$2y$%02u$%.22s", cost, ZSTR_VAL(salt64));
	ZEND_ASSERT(hash_prefix[28] == ZSTR_VAL(salt64)[21] && hash_prefix[29] == 0);
	zend_string_free(salt64);

	/* This cast is safe, since both values are defined here in code and cannot overflow */
	hash = php_crypt(ZSTR_VAL(password), (int)ZSTR_LEN(password), hash_prefix, (int)sizeof(hash_prefix)-1, 1);
	if (!hash) {
		return NULL;
	}

	if (ZSTR_LEN(hash) != 60) {
		zend_string_free(hash);
		return NULL;
	}

	return hash;
}
/* }}} */

static zend_string* php_password_bcrypt_hash(const zend_string *password, zend_array *options) /* {{{ */
{
	unsigned char salt[PHP_BCRYPT_SALT_SIZE];
	zend_long cost = PHP_PASSWORD_BCRYPT_COST;

	if (memchr(ZSTR_VAL(password), '\0', ZSTR_LEN(password))) {
		zend_value_error("Bcrypt password must not contain null character");
		return NULL;
	}

	if (!php_password_bcrypt_get_cost(options, &cost)) {
		zend_value_error("Invalid bcrypt cost parameter specified: " ZEND_LONG_FMT, cost);
		return NULL;
	}

	if (!php_password_make_salt(salt, PHP_BCRYPT_SALT_SIZE)) {
		return NULL;
	}

	return php_password_bcrypt_hash_internal(password, &salt, cost);
}
/* }}} */

const php_password_algo php_password_algo_bcrypt = {
	"bcrypt",
	php_password_bcrypt_hash,
	php_password_bcrypt_verify,
	php_password_bcrypt_needs_rehash,
	php_password_bcrypt_get_info,
	php_password_bcrypt_valid,
};

#ifdef HAVE_ARGON2LIB
/* argon2i/argon2id shared implementation */

static int extract_argon2_parameters(const zend_string *hash,
									  zend_long *v, zend_long *memory_cost,
									  zend_long *time_cost, zend_long *threads) /* {{{ */
{
	const char *p = ZSTR_VAL(hash);
	if (!hash || (ZSTR_LEN(hash) < sizeof("$argon2id$"))) {
		return FAILURE;
	}
	if (!memcmp(p, "$argon2i$", sizeof("$argon2i$") - 1)) {
		p += sizeof("$argon2i$") - 1;
	} else if (!memcmp(p, "$argon2id$", sizeof("$argon2id$") - 1)) {
		p += sizeof("$argon2id$") - 1;
	} else {
		return FAILURE;
	}

	sscanf(p, "v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT,
	       v, memory_cost, time_cost, threads);

	return SUCCESS;
}
/* }}} */

static int php_password_argon2_get_info(zval *return_value, const zend_string *hash) {
	zend_long v = 0;
	zend_long memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
	zend_long time_cost = PHP_PASSWORD_ARGON2_TIME_COST;
	zend_long threads = PHP_PASSWORD_ARGON2_THREADS;

	extract_argon2_parameters(hash, &v, &memory_cost, &time_cost, &threads);

	add_assoc_long(return_value, "memory_cost", memory_cost);
	add_assoc_long(return_value, "time_cost", time_cost);
	add_assoc_long(return_value, "threads", threads);

	return SUCCESS;
}

static bool php_password_argon2_needs_rehash(const zend_string *hash, zend_array *options) {
	zend_long v = 0;
	zend_long new_memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST, memory_cost = 0;
	zend_long new_time_cost = PHP_PASSWORD_ARGON2_TIME_COST, time_cost = 0;
	zend_long new_threads = PHP_PASSWORD_ARGON2_THREADS, threads = 0;
	zval *option_buffer;

	if (options && (option_buffer = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
		new_memory_cost = zval_get_long(option_buffer);
	}

	if (options && (option_buffer = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
		new_time_cost = zval_get_long(option_buffer);
	}

	if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
		new_threads = zval_get_long(option_buffer);
	}

	extract_argon2_parameters(hash, &v, &memory_cost, &time_cost, &threads);

	return (new_time_cost != time_cost) ||
			(new_memory_cost != memory_cost) ||
			(new_threads != threads);
}

static zend_string *php_password_argon2_hash(const zend_string *password, zend_array *options, argon2_type type) {
	zval *option_buffer;
	zend_string *out, *encoded;
	size_t salt_len = 16;
	unsigned char salt[salt_len];
	size_t time_cost = PHP_PASSWORD_ARGON2_TIME_COST;
	size_t memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
	size_t threads = PHP_PASSWORD_ARGON2_THREADS;
	size_t encoded_len;
	int status = 0;

	if (options && (option_buffer = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
		memory_cost = zval_get_long(option_buffer);
	}

	if (memory_cost > ARGON2_MAX_MEMORY || memory_cost < ARGON2_MIN_MEMORY) {
		zend_value_error("Memory cost is outside of allowed memory range");
		return NULL;
	}

	if (options && (option_buffer = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
		time_cost = zval_get_long(option_buffer);
	}

	if (time_cost > ARGON2_MAX_TIME || time_cost < ARGON2_MIN_TIME) {
		zend_value_error("Time cost is outside of allowed time range");
		return NULL;
	}

	if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
		threads = zval_get_long(option_buffer);
	}

	if (threads > ARGON2_MAX_LANES || threads == 0) {
		zend_value_error("Invalid number of threads");
		return NULL;
	}

	if (!php_password_make_salt(salt, salt_len)) {
		return NULL;
	}

	out = zend_string_alloc(32, 0);
	encoded_len = argon2_encodedlen(
		time_cost,
		memory_cost,
		threads,
		(uint32_t)salt_len,
		ZSTR_LEN(out),
		type
	);

	encoded = zend_string_alloc(encoded_len - 1, 0);
	status = argon2_hash(
		time_cost,
		memory_cost,
		threads,
		ZSTR_VAL(password),
		ZSTR_LEN(password),
		salt,
		salt_len,
		ZSTR_VAL(out),
		ZSTR_LEN(out),
		ZSTR_VAL(encoded),
		encoded_len,
		type,
		ARGON2_VERSION_NUMBER
	);

	zend_string_release_ex(out, 0);

	if (status != ARGON2_OK) {
		zend_string_efree(encoded);
		zend_value_error("%s", argon2_error_message(status));
		return NULL;
	}

	ZSTR_VAL(encoded)[ZSTR_LEN(encoded)] = 0;
	return encoded;
}

/* argon2i specific methods */

static bool php_password_argon2i_verify(const zend_string *password, const zend_string *hash) {
	return ARGON2_OK == argon2_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password), Argon2_i);
}

static zend_string *php_password_argon2i_hash(const zend_string *password, zend_array *options) {
	return php_password_argon2_hash(password, options, Argon2_i);
}

const php_password_algo php_password_algo_argon2i = {
	"argon2i",
	php_password_argon2i_hash,
	php_password_argon2i_verify,
	php_password_argon2_needs_rehash,
	php_password_argon2_get_info,
	NULL,
};

/* argon2id specific methods */

static bool php_password_argon2id_verify(const zend_string *password, const zend_string *hash) {
	return ARGON2_OK == argon2_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password), Argon2_id);
}

static zend_string *php_password_argon2id_hash(const zend_string *password, zend_array *options) {
	return php_password_argon2_hash(password, options, Argon2_id);
}

const php_password_algo php_password_algo_argon2id = {
	"argon2id",
	php_password_argon2id_hash,
	php_password_argon2id_verify,
	php_password_argon2_needs_rehash,
	php_password_argon2_get_info,
	NULL,
};
#endif

PHP_MINIT_FUNCTION(password) /* {{{ */
{
	zend_hash_init(&php_password_algos, 4, NULL, ZVAL_PTR_DTOR, 1);

	register_password_symbols(module_number);

	if (FAILURE == php_password_algo_register("2y", &php_password_algo_bcrypt)) {
		return FAILURE;
	}

#ifdef HAVE_ARGON2LIB
	if (FAILURE == php_password_algo_register("argon2i", &php_password_algo_argon2i)) {
		return FAILURE;
	}
	if (FAILURE == php_password_algo_register("argon2id", &php_password_algo_argon2id)) {
		return FAILURE;
	}
#endif

	if (FAILURE == php_password_algo_register("bcrypt-wrap", &php_password_algo_bcrypt_wrap)) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(password) /* {{{ */
{
#ifdef ZTS
	if (!tsrm_is_main_thread()) {
		return SUCCESS;
	}
#endif
	zend_hash_destroy(&php_password_algos);
	return SUCCESS;
}
/* }}} */

const php_password_algo* php_password_algo_default(void) {
	return &php_password_algo_bcrypt;
}

const php_password_algo* php_password_algo_find(const zend_string *ident) {
	zval *tmp;

	if (!ident) {
		return NULL;
	}

	tmp = zend_hash_find(&php_password_algos, (zend_string*)ident);
	if (!tmp || (Z_TYPE_P(tmp) != IS_PTR)) {
		return NULL;
	}

	return Z_PTR_P(tmp);
}

static const php_password_algo* php_password_algo_find_zval(zend_string *arg_str, zend_long arg_long, bool arg_is_null) {
	if (arg_is_null) {
		return php_password_algo_default();
	}

	if (arg_str) {
		return php_password_algo_find(arg_str);
	}

	switch (arg_long) {
		case 0: return php_password_algo_default();
		case 1: return &php_password_algo_bcrypt;
#ifdef HAVE_ARGON2LIB
		case 2: return &php_password_algo_argon2i;
		case 3: return &php_password_algo_argon2id;
#else
		case 2:
			{
			zend_string *n = ZSTR_INIT_LITERAL("argon2i", 0);
			const php_password_algo* ret = php_password_algo_find(n);
			zend_string_release(n);
			return ret;
			}
		case 3:
			{
			zend_string *n = ZSTR_INIT_LITERAL("argon2id", 0);
			const php_password_algo* ret = php_password_algo_find(n);
			zend_string_release(n);
			return ret;
			}
#endif
		case 4: return &php_password_algo_bcrypt_wrap;
	}

	return NULL;
}

zend_string *php_password_algo_extract_ident(const zend_string* hash) {
	const char *ident, *ident_end;

	if (!hash || ZSTR_LEN(hash) < 3) {
		/* Minimum prefix: "$x$" */
		return NULL;
	}

	ident = ZSTR_VAL(hash) + 1;
	ident_end = strchr(ident, '$');
	if (!ident_end) {
		/* No terminating '$' */
		return NULL;
	}

	return zend_string_init(ident, ident_end - ident, 0);
}

const php_password_algo* php_password_algo_identify_ex(const zend_string* hash, const php_password_algo *default_algo) {
	const php_password_algo *algo;
	zend_string *ident = php_password_algo_extract_ident(hash);

	if (!ident) {
		return default_algo;
	}

	algo = php_password_algo_find(ident);
	zend_string_release(ident);
	return (!algo || (algo->valid && !algo->valid(hash))) ? default_algo : algo;
}

/* {{{ Retrieves information about a given hash */
PHP_FUNCTION(password_get_info)
{
	const php_password_algo *algo;
	zend_string *hash, *ident;
	zval options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(hash)
	ZEND_PARSE_PARAMETERS_END();

	array_init(return_value);
	array_init(&options);

	ident = php_password_algo_extract_ident(hash);
	algo = php_password_algo_find(ident);
	if (!algo || (algo->valid && !algo->valid(hash))) {
		if (ident) {
			zend_string_release(ident);
		}
		add_assoc_null(return_value, "algo");
		add_assoc_string(return_value, "algoName", "unknown");
		add_assoc_zval(return_value, "options", &options);
		return;
	}

	add_assoc_str(return_value, "algo", php_password_algo_extract_ident(hash));
	zend_string_release(ident);

	add_assoc_string(return_value, "algoName", algo->name);
	if (algo->get_info) {
		algo->get_info(&options, hash);
	}
	add_assoc_zval(return_value, "options", &options);
}
/** }}} */

/* {{{ Determines if a given hash requires re-hashing based upon parameters */
PHP_FUNCTION(password_needs_rehash)
{
	const php_password_algo *old_algo, *new_algo;
	zend_string *hash;
	zend_string *new_algo_str;
	zend_long new_algo_long = 0;
	bool new_algo_is_null;
	zend_array *options = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(hash)
		Z_PARAM_STR_OR_LONG_OR_NULL(new_algo_str, new_algo_long, new_algo_is_null)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	new_algo = php_password_algo_find_zval(new_algo_str, new_algo_long, new_algo_is_null);
	if (!new_algo) {
		/* Unknown new algorithm, never prompt to rehash. */
		RETURN_FALSE;
	}

	old_algo = php_password_algo_identify_ex(hash, NULL);
	if (old_algo != new_algo) {
		/* Different algorithm preferred, always rehash. */
		RETURN_TRUE;
	}

	RETURN_BOOL(old_algo->needs_rehash(hash, options));
}
/* }}} */

/* {{{ Verify a hash created using crypt() or password_hash() */
PHP_FUNCTION(password_verify)
{
	zend_string *password, *hash;
	const php_password_algo *algo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(password)
		Z_PARAM_STR(hash)
	ZEND_PARSE_PARAMETERS_END();

	algo = php_password_algo_identify(hash);
	RETURN_BOOL(algo && (!algo->verify || algo->verify(password, hash)));
}
/* }}} */

/* {{{ Hash a password */
PHP_FUNCTION(password_hash)
{
	zend_string *password, *digest = NULL;
	zend_string *algo_str;
	zend_long algo_long = 0;
	bool algo_is_null;
	const php_password_algo *algo;
	zend_array *options = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(password)
		Z_PARAM_STR_OR_LONG_OR_NULL(algo_str, algo_long, algo_is_null)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	algo = php_password_algo_find_zval(algo_str, algo_long, algo_is_null);
	if (!algo) {
		zend_argument_value_error(2, "must be a valid password hashing algorithm");
		RETURN_THROWS();
	}

	if (options && zend_hash_str_exists(options, "salt", sizeof("salt") - 1)) {
		php_error_docref(NULL, E_WARNING, "The \"salt\" option has been ignored, since providing a custom salt is no longer supported");
	}

	digest = algo->hash(password, options);
	if (!digest) {
		if (!EG(exception)) {
			zend_throw_error(NULL, "Password hashing failed for unknown reason");
		}
		RETURN_THROWS();
	}

	RETURN_NEW_STR(digest);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(password_algos) {
	zend_string *algo;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	ZEND_HASH_MAP_FOREACH_STR_KEY(&php_password_algos, algo) {
		add_next_index_str(return_value, zend_string_copy(algo));
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* bcrypt-wrap */

#define PHP_BCRYPT_WRAP_VERSION 0

typedef struct bcrypt_wrap_ops {
	uint8_t version;
	uint8_t cost;
	char prehash[10];
} bcrypt_wrap_ops;

static bool php_password_bcrypt_wrap_get_options(zend_array *options, bcrypt_wrap_ops *ops) /* {{{ */
{
	zval *zv;
	zend_long cost = PHP_PASSWORD_BCRYPT_COST;

	if (!php_password_bcrypt_get_cost(options, &cost)) {
		zend_value_error("Invalid bcrypt cost parameter specified: " ZEND_LONG_FMT, cost);
		return false;
	}

	*ops = (bcrypt_wrap_ops) {
		.version = PHP_BCRYPT_WRAP_VERSION,
		.cost = cost,
		.prehash = {0}
	};

	if (options && (zv = zend_hash_str_find(options, "prehash", sizeof("prehash")-1)) != NULL) {
		zend_string *prehash = zval_get_string(zv);

		if (zend_string_equals_literal(prehash, "cshake256")) {
			snprintf(ops->prehash, sizeof(ops->prehash), "cshake256");
		} else if (zend_string_equals_literal(prehash, "sha512")) {
			snprintf(ops->prehash, sizeof(ops->prehash), "sha512");
		} else if (!zend_string_equals_literal(prehash, "cshake256")) {
			zend_value_error("Invalid bcrypt-wrap prehash parameter specified: %s", ZSTR_VAL(prehash));
			zend_string_release(prehash);
			return false;
		}

		zend_string_release(prehash);
	} else {
		snprintf(ops->prehash, sizeof(ops->prehash), "cshake256");
	}

	return true;
}
/* }}} */

static bool php_password_bcrypt_wrap_parse(const zend_string *hash, bcrypt_wrap_ops *ops) /* {{{ */
{
	unsigned int v, cost;
	char prehash[10];
	int r = 0;
	const char *h = ZSTR_VAL(hash);

	if (ZSTR_LEN(hash) < 93 || ZSTR_LEN(hash) > 97 || !ZEND_IS_DIGIT(h[15]) || !ZEND_IS_DIGIT(h[22]) || h[22] == '0') {
		return false;
	}

	if (3 != sscanf(h, "$bcrypt-wrap$v=%1u$cost=%2u,prehash=%9[a-z0-9]$%*21[A-Za-z0-9+/]%*1[AQgw]$%*31[A-Za-z0-9+/]%n",
		&v, &cost, prehash, &r) || (size_t)r != ZSTR_LEN(hash) || cost < 4 || cost > 31
	) {
		return false;
	}

	if (v > PHP_BCRYPT_WRAP_VERSION) {
		return false;
	}

	if (strcmp(prehash, "cshake256") != 0 && strcmp(prehash, "sha512") != 0) {
		return false;
	}

	if (ops != NULL) {
		*ops = (bcrypt_wrap_ops) {
			.version = v,
			.cost = cost,
			.prehash = {0}
		};
		snprintf(ops->prehash, sizeof(ops->prehash), "%.9s", prehash);
	}

	return true;
}
/* }}} */

static zend_string* php_password_bcrypt_wrap_prehash(const zend_string *password, const bcrypt_wrap_ops *ops, const unsigned char *salt, size_t salt_len) /* {{{ */
{
	 /* 72 bytes after base64 encoding */
	const size_t raw_len = 54;
	unsigned char raw[raw_len];
	/* Dynamic sizes not needed anytime soon, if at all, but still */
	static const char prefix[] = "bcrypt-wrap:%u:";
	const size_t prefix_len = snprintf(NULL, 0, prefix, ops->version);
	unsigned char psalt[prefix_len + salt_len];

	/* Prehash salt is technically not bound by Bcrypt's limit.
	 * Highly unlikely, but a future version could increase its size. */
	ZEND_ASSERT(salt_len >= PHP_BCRYPT_SALT_SIZE);
	snprintf((char *) psalt, sizeof(psalt), prefix, ops->version);
	memcpy(psalt + prefix_len, salt, salt_len);

#if ZEND_DEBUG
	/* Official test vectors from NIST, here for PoC purposes, as this is our first
	 * use of cSHAKE256. Can probably be dropped from final patch after review.
	 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/cSHAKE_samples.pdf
	 * (tail of page 8 until end of document) */
	const unsigned char cstr[15] = "Email Signature";
	const unsigned char data1[4] = { 0x00, 0x01, 0x02, 0x03 };
	const unsigned char out1[64] = {
		0xD0, 0x08, 0x82, 0x8E, 0x2B, 0x80, 0xAC, 0x9D, 0x22, 0x18, 0xFF, 0xEE, 0x1D, 0x07, 0x0C, 0x48,
		0xB8, 0xE4, 0xC8, 0x7B, 0xFF, 0x32, 0xC9, 0x69, 0x9D, 0x5B, 0x68, 0x96, 0xEE, 0xE0, 0xED, 0xD1,
		0x64, 0x02, 0x0E, 0x2B, 0xE0, 0x56, 0x08, 0x58, 0xD9, 0xC0, 0x0C, 0x03, 0x7E, 0x34, 0xA9, 0x69,
		0x37, 0xC5, 0x61, 0xA7, 0x4C, 0x41, 0x2B, 0xB4, 0xC7, 0x46, 0x46, 0x95, 0x27, 0x28, 0x1C, 0x8C
	};
	unsigned char data2[200];
	for (size_t i = 0; i < sizeof(data2); i++) {
		data2[i] = i;
	}
	const unsigned char out2[64] = {
		0x07, 0xDC, 0x27, 0xB1, 0x1E, 0x51, 0xFB, 0xAC, 0x75, 0xBC, 0x7B, 0x3C, 0x1D, 0x98, 0x3E, 0x8B,
		0x4B, 0x85, 0xFB, 0x1D, 0xEF, 0xAF, 0x21, 0x89, 0x12, 0xAC, 0x86, 0x43, 0x02, 0x73, 0x09, 0x17,
		0x27, 0xF4, 0x2B, 0x17, 0xED, 0x1D, 0xF6, 0x3E, 0x8E, 0xC1, 0x18, 0xF0, 0x4B, 0x23, 0x63, 0x3C,
		0x1D, 0xFB, 0x15, 0x74, 0xC8, 0xFB, 0x55, 0xCB, 0x45, 0xDA, 0x8E, 0x25, 0xAF, 0xB0, 0x92, 0xBB
	};

	unsigned char buffer[64];
	php_hash_cshake256(data1, sizeof(data1), cstr, sizeof(cstr), buffer, sizeof(out1));
	ZEND_ASSERT(memcmp(out1, buffer, 64) == 0);
	php_hash_cshake256(data2, sizeof(data2), cstr, sizeof(cstr), buffer, sizeof(out2));
	ZEND_ASSERT(memcmp(out2, buffer, 64) == 0);
	/* One more for our intended size */
	memset(buffer, 0, sizeof(buffer));
	php_hash_cshake256(data2, sizeof(data2), cstr, sizeof(cstr), buffer, raw_len);
	ZEND_ASSERT(memcmp(out2, buffer, raw_len) == 0 && buffer[raw_len] == 0);
#endif

	if (strcmp(ops->prehash, "cshake256") == 0) {
		/* Salt goes into the customization string as defined by SP 800-185 */
		php_hash_cshake256(
			(const unsigned char *) ZSTR_VAL(password), ZSTR_LEN(password),
			psalt, sizeof(psalt),
			raw, raw_len
		);
	} else if (strcmp(ops->prehash, "sha512") == 0) {
		php_hash_hmac(
			"sha512", sizeof("sha512")-1,
			(const unsigned char *) ZSTR_VAL(password), ZSTR_LEN(password),
			psalt, sizeof(psalt),
			raw, raw_len
		);
	} else {
		/* Failsafe, but should not be possible as there are prior guards.
		 * ops->prehash is not written to before correct value is determined. */
		zend_value_error("Invalid bcrypt-wrap prehash parameter specified: %s", ops->prehash);
		return NULL;
	}

	ZEND_ASSERT(raw != NULL);
	return php_base64_encode_ex((const unsigned char *) raw, raw_len, PHP_BASE64_NO_PADDING);
}
/* }}} */

static zend_string* php_password_bcrypt_wrap_hash(const zend_string *password, zend_array *options) /* {{{ */
{
	bcrypt_wrap_ops ops;
	unsigned char salt[PHP_BCRYPT_SALT_SIZE];
	char hash64[32];
	zend_string *salt64, *prehash, *hash;

	/* 1KiB is a thought-terminating limit that nobody can ever complain about.
	 * It's not to impose a limit on passwords, but to guard against extreme misuse
	 * and shut down any talk of choking the prehash with megabytes of data. */
	if (ZSTR_LEN(password) > 1024) {
		zend_value_error("Bcrypt-wrap password must not be larger than 1KiB");
		return NULL;
	}

	if (!php_password_bcrypt_wrap_get_options(options, &ops)) {
		return NULL;
	}

	if (!php_password_make_salt(salt, PHP_BCRYPT_SALT_SIZE)) {
		return NULL;
	}

	prehash = php_password_bcrypt_wrap_prehash(password, &ops, salt, PHP_BCRYPT_SALT_SIZE);
	ZEND_ASSERT(prehash != NULL);
	if (!(hash = php_password_bcrypt_hash_internal(prehash, &salt, ops.cost))) {
		zend_string_free(prehash);
		return NULL;
	}
	ZEND_SECURE_ZERO(ZSTR_VAL(prehash), ZSTR_LEN(prehash));
	zend_string_free(prehash);

	/* Can't fail unless bcrypt code is broken */
	if (sscanf(ZSTR_VAL(hash), "$2y$%*2[0-9]$%*22[a-zA-Z0-9/.]%31[a-zA-Z0-9/.]", hash64) != 1) {
		zend_string_free(hash);
		return NULL;
	}
	zend_string_free(hash);
	php_password_b64_from_bcrypt(hash64, hash64, 31);

	salt64 = php_base64_encode_ex((const unsigned char*) salt, PHP_BCRYPT_SALT_SIZE, PHP_BASE64_NO_PADDING);

	hash = strpprintf(0, "$bcrypt-wrap$v=%u$cost=%u,prehash=%s$%s$%s", ops.version, ops.cost, ops.prehash, ZSTR_VAL(salt64), hash64);
	zend_string_free(salt64);
	ZEND_ASSERT(ZSTR_LEN(hash) == 86 + strlen(ops.prehash) + (ops.cost < 10 ? 1 : 2));

	return hash;
}
/* }}} */

static bool php_password_bcrypt_wrap_valid(const zend_string *hash) /* {{{ */
{
	return php_password_bcrypt_wrap_parse(hash, NULL);
}
/* }}} */

static bool php_password_bcrypt_wrap_verify(const zend_string *password, const zend_string *hash) /* {{{ */
{
	int r = 0;
	char salt64[23], hash64[32];
	bcrypt_wrap_ops ops;
	zend_string *salt, *prehash;

	if (ZSTR_LEN(password) > 1024 || !php_password_bcrypt_wrap_parse(hash, &ops)) {
		return false;
	}

	if (sscanf(ZSTR_VAL(hash) + (ZSTR_LEN(hash) - 54), "%22[A-Za-z0-9+/]$%31[A-Za-z0-9+/]%n", salt64, hash64, &r) != 2 || r != 54) {
		return false;
	}

	if (!(salt = php_base64_decode((const unsigned char *) salt64, 22))) {
		return false;
	}

	if (ZSTR_LEN(salt) != PHP_BCRYPT_SALT_SIZE) {
		zend_string_free(salt);
		return false;
	}

	prehash = php_password_bcrypt_wrap_prehash(password, &ops, (const unsigned char *) ZSTR_VAL(salt), PHP_BCRYPT_SALT_SIZE);
	zend_string_free(salt);

	if (!prehash) {
		return false;
	}

	if (!php_password_bcrypt_from_b64(salt64, salt64, 22)) {
		return false;
	}

	if (!php_password_bcrypt_from_b64(hash64, hash64, 31)) {
		return false;
	}

	zend_string *stringed = strpprintf(0, "$2y$%02u$%.22s%.31s", ops.cost, salt64, hash64);
	zend_string *computed = php_crypt(ZSTR_VAL(prehash), (int) ZSTR_LEN(prehash), ZSTR_VAL(stringed), (int) ZSTR_LEN(stringed), 1);
	ZEND_SECURE_ZERO(ZSTR_VAL(prehash), ZSTR_LEN(prehash));
	zend_string_free(prehash);

	if (!computed) {
		zend_string_free(stringed);
		return false;
	}

	if (ZSTR_LEN(computed) != 60) {
		zend_string_free(stringed);
		zend_string_free(computed);
		return false;
	}

	/* Constant-time check, don't optimize for performance */
	r = php_safe_bcmp(stringed, computed);
	zend_string_free(stringed);
	zend_string_free(computed);

	return r == 0;
}
/* }}} */

static bool php_password_bcrypt_wrap_needs_rehash(const zend_string *hash, zend_array *options) /* {{{ */
{
	bcrypt_wrap_ops old;
	bcrypt_wrap_ops new;

	if (!php_password_bcrypt_wrap_parse(hash, &old)) {
		return true;
	}

	if (!php_password_bcrypt_wrap_get_options(options, &new)) {
		return false;
	}

	return old.version < new.version || old.cost != new.cost || strcmp(old.prehash, new.prehash) != 0;
}
/* }}} */

static int php_password_bcrypt_wrap_get_info(zval *return_value, const zend_string *hash) /* {{{ */
{
	bcrypt_wrap_ops ops;

	if (!php_password_bcrypt_wrap_parse(hash, &ops)) {
		return FAILURE;
	}
	add_assoc_long(return_value, "cost", (zend_long) ops.cost);
	add_assoc_stringl(return_value, "prehash", ops.prehash, strlen(ops.prehash));
	return SUCCESS;
}
/* }}} */

const php_password_algo php_password_algo_bcrypt_wrap = {
	"bcrypt-wrap",
	php_password_bcrypt_wrap_hash,
	php_password_bcrypt_wrap_verify,
	php_password_bcrypt_wrap_needs_rehash,
	php_password_bcrypt_wrap_get_info,
	php_password_bcrypt_wrap_valid
};
