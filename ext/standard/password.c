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
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   |          Charles R. Portwood II <charlesportwoodii@erianna.com>      |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>

#include "php.h"

#include "fcntl.h"
#include "php_password.h"
#include "php_rand.h"
#include "php_crypt.h"
#include "base64.h"
#include "zend_interfaces.h"
#include "info.h"
#include "php_random.h"
#if HAVE_ARGON2LIB
#include "argon2.h"
#endif

#ifdef PHP_WIN32
#include "win32/winutil.h"
#endif

PHP_MINIT_FUNCTION(password) /* {{{ */
{
	REGISTER_LONG_CONSTANT("PASSWORD_DEFAULT", PHP_PASSWORD_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_BCRYPT", PHP_PASSWORD_BCRYPT, CONST_CS | CONST_PERSISTENT);
#if HAVE_ARGON2LIB
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2I", PHP_PASSWORD_ARGON2I, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2ID", PHP_PASSWORD_ARGON2ID, CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("PASSWORD_BCRYPT_DEFAULT_COST", PHP_PASSWORD_BCRYPT_COST, CONST_CS | CONST_PERSISTENT);
#if HAVE_ARGON2LIB
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_MEMORY_COST", PHP_PASSWORD_ARGON2_MEMORY_COST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_TIME_COST", PHP_PASSWORD_ARGON2_TIME_COST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_THREADS", PHP_PASSWORD_ARGON2_THREADS, CONST_CS | CONST_PERSISTENT);
#endif

	return SUCCESS;
}
/* }}} */

static zend_string* php_password_get_algo_name(const php_password_algo algo)
{
	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			return zend_string_init("bcrypt", sizeof("bcrypt") - 1, 0);
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			return zend_string_init("argon2i", sizeof("argon2i") - 1, 0);
		case PHP_PASSWORD_ARGON2ID:
			return zend_string_init("argon2id", sizeof("argon2id") - 1, 0);
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			return zend_string_init("unknown", sizeof("unknown") - 1, 0);
	}
}

static php_password_algo php_password_determine_algo(const zend_string *hash)
{
	const char *h = ZSTR_VAL(hash);
	const size_t len = ZSTR_LEN(hash);
	if (len == 60 && h[0] == '$' && h[1] == '2' && h[2] == 'y') {
		return PHP_PASSWORD_BCRYPT;
	}
#if HAVE_ARGON2LIB
	if (len >= sizeof("$argon2id$")-1 && !memcmp(h, "$argon2id$", sizeof("$argon2id$")-1)) {
    	return PHP_PASSWORD_ARGON2ID;
	}

	if (len >= sizeof("$argon2i$")-1 && !memcmp(h, "$argon2i$", sizeof("$argon2i$")-1)) {
    	return PHP_PASSWORD_ARGON2I;
	}
#endif

	return PHP_PASSWORD_UNKNOWN;
}

static int php_password_salt_is_alphabet(const char *str, const size_t len) /* {{{ */
{
	size_t i = 0;

	for (i = 0; i < len; i++) {
		if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/')) {
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

static int php_password_salt_to64(const char *str, const size_t str_len, const size_t out_len, char *ret) /* {{{ */
{
	size_t pos = 0;
	zend_string *buffer;
	if ((int) str_len < 0) {
		return FAILURE;
	}
	buffer = php_base64_encode((unsigned char*) str, str_len);
	if (ZSTR_LEN(buffer) < out_len) {
		/* Too short of an encoded string generated */
		zend_string_release_ex(buffer, 0);
		return FAILURE;
	}
	for (pos = 0; pos < out_len; pos++) {
		if (ZSTR_VAL(buffer)[pos] == '+') {
			ret[pos] = '.';
		} else if (ZSTR_VAL(buffer)[pos] == '=') {
			zend_string_free(buffer);
			return FAILURE;
		} else {
			ret[pos] = ZSTR_VAL(buffer)[pos];
		}
	}
	zend_string_free(buffer);
	return SUCCESS;
}
/* }}} */

static zend_string* php_password_make_salt(size_t length) /* {{{ */
{
	zend_string *ret, *buffer;

	if (length > (INT_MAX / 3)) {
		php_error_docref(NULL, E_WARNING, "Length is too large to safely generate");
		return NULL;
	}

	buffer = zend_string_alloc(length * 3 / 4 + 1, 0);
	if (FAILURE == php_random_bytes_silent(ZSTR_VAL(buffer), ZSTR_LEN(buffer))) {
		php_error_docref(NULL, E_WARNING, "Unable to generate salt");
		zend_string_release_ex(buffer, 0);
		return NULL;
	}

	ret = zend_string_alloc(length, 0);
	if (php_password_salt_to64(ZSTR_VAL(buffer), ZSTR_LEN(buffer), length, ZSTR_VAL(ret)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Generated salt too short");
		zend_string_release_ex(buffer, 0);
		zend_string_release_ex(ret, 0);
		return NULL;
	}
	zend_string_release_ex(buffer, 0);
	ZSTR_VAL(ret)[length] = 0;
	return ret;
}
/* }}} */

#if HAVE_ARGON2LIB
static void extract_argon2_parameters(const php_password_algo algo, const zend_string *hash,
									  zend_long *v, zend_long *memory_cost,
									  zend_long *time_cost, zend_long *threads) /* {{{ */
{
	if (algo == PHP_PASSWORD_ARGON2ID) {
		sscanf(ZSTR_VAL(hash), "$%*[argon2id]$v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT, v, memory_cost, time_cost, threads);
	} else if (algo == PHP_PASSWORD_ARGON2I) {
		sscanf(ZSTR_VAL(hash), "$%*[argon2i]$v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT, v, memory_cost, time_cost, threads);
	}

	return;
}
#endif

/* {{{ proto array password_get_info(string $hash)
Retrieves information about a given hash */
PHP_FUNCTION(password_get_info)
{
	php_password_algo algo;
	zend_string *hash, *algo_name;
	zval options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(hash)
	ZEND_PARSE_PARAMETERS_END();

	array_init(&options);

	algo = php_password_determine_algo(hash);
	algo_name = php_password_get_algo_name(algo);

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				zend_long cost = PHP_PASSWORD_BCRYPT_COST;
				sscanf(ZSTR_VAL(hash), "$2y$" ZEND_LONG_FMT "$", &cost);
				add_assoc_long(&options, "cost", cost);
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
		case PHP_PASSWORD_ARGON2ID:
			{
				zend_long v = 0;
				zend_long memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
				zend_long time_cost = PHP_PASSWORD_ARGON2_TIME_COST;
				zend_long threads = PHP_PASSWORD_ARGON2_THREADS;

				extract_argon2_parameters(algo, hash, &v, &memory_cost, &time_cost, &threads);

				add_assoc_long(&options, "memory_cost", memory_cost);
				add_assoc_long(&options, "time_cost", time_cost);
				add_assoc_long(&options, "threads", threads);
			}
			break;
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			break;
	}

	array_init(return_value);

	add_assoc_long(return_value, "algo", algo);
	add_assoc_str(return_value, "algoName", algo_name);
	add_assoc_zval(return_value, "options", &options);
}
/** }}} */

/* {{{ proto bool password_needs_rehash(string $hash, int $algo[, array $options])
Determines if a given hash requires re-hashing based upon parameters */
PHP_FUNCTION(password_needs_rehash)
{
	zend_long new_algo = 0;
	php_password_algo algo;
	zend_string *hash;
	HashTable *options = 0;
	zval *option_buffer;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(hash)
		Z_PARAM_LONG(new_algo)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_OBJECT_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	algo = php_password_determine_algo(hash);

	if ((zend_long)algo != new_algo) {
		RETURN_TRUE;
	}

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				zend_long new_cost = PHP_PASSWORD_BCRYPT_COST, cost = 0;

				if (options && (option_buffer = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
					new_cost = zval_get_long(option_buffer);
				}

				sscanf(ZSTR_VAL(hash), "$2y$" ZEND_LONG_FMT "$", &cost);
				if (cost != new_cost) {
					RETURN_TRUE;
				}
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
		case PHP_PASSWORD_ARGON2ID:
			{
				zend_long v = 0;
				zend_long new_memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST, memory_cost = 0;
				zend_long new_time_cost = PHP_PASSWORD_ARGON2_TIME_COST, time_cost = 0;
				zend_long new_threads = PHP_PASSWORD_ARGON2_THREADS, threads = 0;

				if (options && (option_buffer = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
					new_memory_cost = zval_get_long(option_buffer);
				}

				if (options && (option_buffer = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
					new_time_cost = zval_get_long(option_buffer);
				}

				if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
					new_threads = zval_get_long(option_buffer);
				}

				extract_argon2_parameters(algo, hash, &v, &memory_cost, &time_cost, &threads);

				if (new_time_cost != time_cost || new_memory_cost != memory_cost || new_threads != threads) {
					RETURN_TRUE;
				}
			}
			break;
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			break;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool password_verify(string password, string hash)
Verify a hash created using crypt() or password_hash() */
PHP_FUNCTION(password_verify)
{
	zend_string *password, *hash;
	php_password_algo algo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(password)
		Z_PARAM_STR(hash)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	algo = php_password_determine_algo(hash);

	switch(algo) {
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
		case PHP_PASSWORD_ARGON2ID:
			{
				argon2_type type;
				if (algo == PHP_PASSWORD_ARGON2ID) {
					type = Argon2_id;
				} else if (algo == PHP_PASSWORD_ARGON2I) {
					type = Argon2_i;
				}
				RETURN_BOOL(ARGON2_OK == argon2_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password), type));
			}
			break;
#endif
		case PHP_PASSWORD_BCRYPT:
		case PHP_PASSWORD_UNKNOWN:
		default:
			{
				size_t i;
				int status = 0;
				zend_string *ret = php_crypt(ZSTR_VAL(password), (int)ZSTR_LEN(password), ZSTR_VAL(hash), (int)ZSTR_LEN(hash), 1);

				if (!ret) {
					RETURN_FALSE;
				}

				if (ZSTR_LEN(ret) != ZSTR_LEN(hash) || ZSTR_LEN(hash) < 13) {
					zend_string_free(ret);
					RETURN_FALSE;
				}

				/* We're using this method instead of == in order to provide
				* resistance towards timing attacks. This is a constant time
				* equality check that will always check every byte of both
				* values. */
				for (i = 0; i < ZSTR_LEN(hash); i++) {
					status |= (ZSTR_VAL(ret)[i] ^ ZSTR_VAL(hash)[i]);
				}

				zend_string_free(ret);

				RETURN_BOOL(status == 0);
			}
	}

	RETURN_FALSE;
}
/* }}} */

static zend_string* php_password_get_salt(zval *return_value, size_t required_salt_len, HashTable *options) {
	zend_string *buffer;
	zval *option_buffer;

	if (!options || !(option_buffer = zend_hash_str_find(options, "salt", sizeof("salt") - 1))) {
		buffer = php_password_make_salt(required_salt_len);
		if (!buffer) {
			RETVAL_FALSE;
		}
		return buffer;
	}

	php_error_docref(NULL, E_DEPRECATED, "Use of the 'salt' option to password_hash is deprecated");

	switch (Z_TYPE_P(option_buffer)) {
		case IS_STRING:
			buffer = zend_string_copy(Z_STR_P(option_buffer));
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_OBJECT:
			buffer = zval_get_string(option_buffer);
			break;
		case IS_FALSE:
		case IS_TRUE:
		case IS_NULL:
		case IS_RESOURCE:
		case IS_ARRAY:
		default:
			php_error_docref(NULL, E_WARNING, "Non-string salt parameter supplied");
			return NULL;
	}

	/* XXX all the crypt related APIs work with int for string length.
		That should be revised for size_t and then we maybe don't require
		the > INT_MAX check. */
	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(buffer))) {
		php_error_docref(NULL, E_WARNING, "Supplied salt is too long");
		zend_string_release_ex(buffer, 0);
		return NULL;
	}

	if (ZSTR_LEN(buffer) < required_salt_len) {
		php_error_docref(NULL, E_WARNING, "Provided salt is too short: %zd expecting %zd", ZSTR_LEN(buffer), required_salt_len);
		zend_string_release_ex(buffer, 0);
		return NULL;
	}

	if (php_password_salt_is_alphabet(ZSTR_VAL(buffer), ZSTR_LEN(buffer)) == FAILURE) {
		zend_string *salt = zend_string_alloc(required_salt_len, 0);
		if (php_password_salt_to64(ZSTR_VAL(buffer), ZSTR_LEN(buffer), required_salt_len, ZSTR_VAL(salt)) == FAILURE) {
			php_error_docref(NULL, E_WARNING, "Provided salt is too short: %zd", ZSTR_LEN(buffer));
			zend_string_release_ex(salt, 0);
			zend_string_release_ex(buffer, 0);
			return NULL;
		}
		zend_string_release_ex(buffer, 0);
		return salt;
	} else {
		zend_string *salt = zend_string_alloc(required_salt_len, 0);
		memcpy(ZSTR_VAL(salt), ZSTR_VAL(buffer), required_salt_len);
		zend_string_release_ex(buffer, 0);
		return salt;
	}
}

/* {{{ proto string password_hash(string password, int algo[, array options = array()])
Hash a password */
PHP_FUNCTION(password_hash)
{
	zend_string *password;
	zend_long algo = PHP_PASSWORD_DEFAULT;
	HashTable *options = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(password)
		Z_PARAM_LONG(algo)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_OBJECT_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				char hash_format[10];
				size_t hash_format_len;
				zend_string *result, *hash, *salt;
				zval *option_buffer;
				zend_long cost = PHP_PASSWORD_BCRYPT_COST;

				if (options && (option_buffer = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
					cost = zval_get_long(option_buffer);
				}

				if (cost < 4 || cost > 31) {
					php_error_docref(NULL, E_WARNING, "Invalid bcrypt cost parameter specified: " ZEND_LONG_FMT, cost);
					RETURN_NULL();
				}

				hash_format_len = snprintf(hash_format, sizeof(hash_format), "$2y$%02" ZEND_LONG_FMT_SPEC "$", cost);
				if (!(salt = php_password_get_salt(return_value, Z_UL(22), options))) {
					return;
				}
				ZSTR_VAL(salt)[ZSTR_LEN(salt)] = 0;

				hash = zend_string_alloc(ZSTR_LEN(salt) + hash_format_len, 0);
				sprintf(ZSTR_VAL(hash), "%s%s", hash_format, ZSTR_VAL(salt));
				ZSTR_VAL(hash)[hash_format_len + ZSTR_LEN(salt)] = 0;

				zend_string_release_ex(salt, 0);

				/* This cast is safe, since both values are defined here in code and cannot overflow */
				result = php_crypt(ZSTR_VAL(password), (int)ZSTR_LEN(password), ZSTR_VAL(hash), (int)ZSTR_LEN(hash), 1);
				zend_string_release_ex(hash, 0);

				if (!result) {
					RETURN_FALSE;
				}

				if (ZSTR_LEN(result) < 13) {
					zend_string_free(result);
					RETURN_FALSE;
				}

				RETURN_STR(result);
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
		case PHP_PASSWORD_ARGON2ID:
			{
				zval *option_buffer;
				zend_string *salt, *out, *encoded;
				size_t time_cost = PHP_PASSWORD_ARGON2_TIME_COST;
				size_t memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
				size_t threads = PHP_PASSWORD_ARGON2_THREADS;
				argon2_type type;
				if (algo == PHP_PASSWORD_ARGON2ID) {
					type = Argon2_id;
				} else if (algo == PHP_PASSWORD_ARGON2I) {
					type = Argon2_i;
				}
				size_t encoded_len;
				int status = 0;

				if (options && (option_buffer = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
					memory_cost = zval_get_long(option_buffer);
				}

				if (memory_cost > ARGON2_MAX_MEMORY || memory_cost < ARGON2_MIN_MEMORY) {
					php_error_docref(NULL, E_WARNING, "Memory cost is outside of allowed memory range");
					RETURN_NULL();
				}

				if (options && (option_buffer = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
					time_cost = zval_get_long(option_buffer);
				}

				if (time_cost > ARGON2_MAX_TIME || time_cost < ARGON2_MIN_TIME) {
					php_error_docref(NULL, E_WARNING, "Time cost is outside of allowed time range");
					RETURN_NULL();
				}

				if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
					threads = zval_get_long(option_buffer);
				}

				if (threads > ARGON2_MAX_LANES || threads == 0) {
					php_error_docref(NULL, E_WARNING, "Invalid number of threads");
					RETURN_NULL();
				}

				if (!(salt = php_password_get_salt(return_value, Z_UL(16), options))) {
					return;
				}

				out = zend_string_alloc(32, 0);
				encoded_len = argon2_encodedlen(
					time_cost,
					memory_cost,
					threads,
					(uint32_t)ZSTR_LEN(salt),
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
					ZSTR_VAL(salt),
					ZSTR_LEN(salt),
					ZSTR_VAL(out),
					ZSTR_LEN(out),
					ZSTR_VAL(encoded),
					encoded_len,
					type,
					ARGON2_VERSION_NUMBER
				);

				zend_string_release_ex(out, 0);
				zend_string_release_ex(salt, 0);

				if (status != ARGON2_OK) {
					zend_string_efree(encoded);
					php_error_docref(NULL, E_WARNING, "%s", argon2_error_message(status));
					RETURN_FALSE;
				}

				ZSTR_VAL(encoded)[ZSTR_LEN(encoded)] = 0;
				RETURN_NEW_STR(encoded);
			}
			break;
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			php_error_docref(NULL, E_WARNING, "Unknown password hashing algorithm: " ZEND_LONG_FMT, algo);
			RETURN_NULL();
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
