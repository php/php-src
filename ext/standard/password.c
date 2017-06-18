/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

/* $Id$ */

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

static char* php_password_get_algo_name(const php_password_algo algo)
{
	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			return "bcrypt";
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			return "argon2i";
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			return "unknown";
	}
}

static php_password_algo php_password_determine_algo(const char *hash, const size_t len)
{
	if (len > 3 && hash[0] == '$' && hash[1] == '2' && hash[2] == 'y' && len == 60) {
		return PHP_PASSWORD_BCRYPT;
	} 
#if HAVE_ARGON2LIB
	if (len >= sizeof("$argon2i$")-1 && !memcmp(hash, "$argon2i$", sizeof("$argon2i$")-1)) {
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
		zend_string_release(buffer);
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

static int php_password_make_salt(size_t length, char *ret) /* {{{ */
{
	size_t raw_length;
	char *buffer;
	char *result;

	if (length > (INT_MAX / 3)) {
		php_error_docref(NULL, E_WARNING, "Length is too large to safely generate");
		return FAILURE;
	}

	raw_length = length * 3 / 4 + 1;

	buffer = (char *) safe_emalloc(raw_length, 1, 1);

	if (FAILURE == php_random_bytes_silent(buffer, raw_length)) {
		php_error_docref(NULL, E_WARNING, "Unable to generate salt");
		efree(buffer);
		return FAILURE;
	}

	result = safe_emalloc(length, 1, 1);
	if (php_password_salt_to64(buffer, raw_length, length, result) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Generated salt too short");
		efree(buffer);
		efree(result);
		return FAILURE;
	}
	memcpy(ret, result, length);
	efree(result);
	efree(buffer);
	ret[length] = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ proto array password_get_info(string $hash)
Retrieves information about a given hash */
PHP_FUNCTION(password_get_info)
{
	php_password_algo algo;
	size_t hash_len;
	char *hash, *algo_name;
	zval options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(hash, hash_len)
	ZEND_PARSE_PARAMETERS_END();

	array_init(&options);

	algo = php_password_determine_algo(hash, (size_t) hash_len);
	algo_name = php_password_get_algo_name(algo);

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				zend_long cost = PHP_PASSWORD_BCRYPT_COST;
				sscanf(hash, "$2y$" ZEND_LONG_FMT "$", &cost);
				add_assoc_long(&options, "cost", cost);
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			{
				zend_long v = 0;
				zend_long memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
				zend_long time_cost = PHP_PASSWORD_ARGON2_TIME_COST;
				zend_long threads = PHP_PASSWORD_ARGON2_THREADS;

				sscanf(hash, "$%*[argon2i]$v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT, &v, &memory_cost, &time_cost, &threads);
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
	add_assoc_string(return_value, "algoName", algo_name);
	add_assoc_zval(return_value, "options", &options);
}
/** }}} */

/* {{{ proto boolean password_needs_rehash(string $hash, integer $algo[, array $options])
Determines if a given hash requires re-hashing based upon parameters */
PHP_FUNCTION(password_needs_rehash)
{
	zend_long new_algo = 0;
	php_password_algo algo;
	size_t hash_len;
	char *hash;
	HashTable *options = 0;
	zval *option_buffer;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(hash, hash_len)
		Z_PARAM_LONG(new_algo)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_OBJECT_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	algo = php_password_determine_algo(hash, (size_t) hash_len);

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

				sscanf(hash, "$2y$" ZEND_LONG_FMT "$", &cost);
				if (cost != new_cost) {
					RETURN_TRUE;
				}
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
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

				sscanf(hash, "$%*[argon2i]$v=" ZEND_LONG_FMT "$m=" ZEND_LONG_FMT ",t=" ZEND_LONG_FMT ",p=" ZEND_LONG_FMT, &v, &memory_cost, &time_cost, &threads);

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

/* {{{ proto boolean password_verify(string password, string hash)
Verify a hash created using crypt() or password_hash() */
PHP_FUNCTION(password_verify)
{
	int status = 0;
	size_t i, password_len, hash_len;
	char *password, *hash;
	zend_string *ret;
	php_password_algo algo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(password, password_len)
		Z_PARAM_STRING(hash, hash_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	algo = php_password_determine_algo(hash, (size_t) hash_len);

	switch(algo) {
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			{
				argon2_type type = Argon2_i;

				status = argon2_verify(hash, password, password_len, type);
				
				if (status == ARGON2_OK) {
					RETURN_TRUE;
				}

				RETURN_FALSE;
			}
			break;
#endif
		case PHP_PASSWORD_BCRYPT:
		case PHP_PASSWORD_UNKNOWN:
		default:
			{
				if ((ret = php_crypt(password, (int)password_len, hash, (int)hash_len, 1)) == NULL) {
					RETURN_FALSE;
				}

				if (ZSTR_LEN(ret) != hash_len || hash_len < 13) {
					zend_string_free(ret);
					RETURN_FALSE;
				}

				/* We're using this method instead of == in order to provide
				* resistance towards timing attacks. This is a constant time
				* equality check that will always check every byte of both
				* values. */
				for (i = 0; i < hash_len; i++) {
					status |= (ZSTR_VAL(ret)[i] ^ hash[i]);
				}

				zend_string_free(ret);

				RETURN_BOOL(status == 0);
			}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string password_hash(string password, int algo[, array options = array()])
Hash a password */
PHP_FUNCTION(password_hash)
{
	char hash_format[8], *hash, *salt, *password;
	zend_long algo = 0;
	size_t password_len = 0;
	int hash_len;
	size_t salt_len = 0, required_salt_len = 0, hash_format_len;
	HashTable *options = 0;
	zval *option_buffer;

#if HAVE_ARGON2LIB
	size_t time_cost = PHP_PASSWORD_ARGON2_TIME_COST; 
	size_t memory_cost = PHP_PASSWORD_ARGON2_MEMORY_COST;
	size_t threads = PHP_PASSWORD_ARGON2_THREADS;
	argon2_type type = Argon2_i;
#endif

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(password, password_len)
		Z_PARAM_LONG(algo)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_OBJECT_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				zend_long cost = PHP_PASSWORD_BCRYPT_COST;

				if (options && (option_buffer = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
					cost = zval_get_long(option_buffer);
				}

				if (cost < 4 || cost > 31) {
					php_error_docref(NULL, E_WARNING, "Invalid bcrypt cost parameter specified: " ZEND_LONG_FMT, cost);
					RETURN_NULL();
				}

				required_salt_len = 22;
				sprintf(hash_format, "$2y$%02ld$", (long) cost);
				hash_format_len = 7;
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			{
				if (options && (option_buffer = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
					memory_cost = zval_get_long(option_buffer);
				}

				if (memory_cost > ARGON2_MAX_MEMORY || memory_cost < ARGON2_MIN_MEMORY) {
					php_error_docref(NULL, E_WARNING, "Memory cost is outside of allowed memory range", memory_cost);
					RETURN_NULL();
				}

				if (options && (option_buffer = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
					time_cost = zval_get_long(option_buffer);
				}

				if (time_cost > ARGON2_MAX_TIME || time_cost < ARGON2_MIN_TIME) {
					php_error_docref(NULL, E_WARNING, "Time cost is outside of allowed time range", time_cost);
					RETURN_NULL();
				}
				
				if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
					threads = zval_get_long(option_buffer);
				}

				if (threads > ARGON2_MAX_LANES || threads == 0) {
					php_error_docref(NULL, E_WARNING, "Invalid number of threads", threads);
					RETURN_NULL();
				}

				required_salt_len = 16;
			}
			break;
#endif
		case PHP_PASSWORD_UNKNOWN:
		default:
			php_error_docref(NULL, E_WARNING, "Unknown password hashing algorithm: " ZEND_LONG_FMT, algo);
			RETURN_NULL();
	}

	if (options && (option_buffer = zend_hash_str_find(options, "salt", sizeof("salt")-1)) != NULL) {
		zend_string *buffer;

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
				RETURN_NULL();
		}

		/* XXX all the crypt related APIs work with int for string length.
			That should be revised for size_t and then we maybe don't require
			the > INT_MAX check. */
		if (ZSTR_LEN(buffer) > INT_MAX) {
			php_error_docref(NULL, E_WARNING, "Supplied salt is too long");
			RETURN_NULL();
		} else if (ZSTR_LEN(buffer) < required_salt_len) {
			php_error_docref(NULL, E_WARNING, "Provided salt is too short: %zd expecting %zd", ZSTR_LEN(buffer), required_salt_len);
			zend_string_release(buffer);
			RETURN_NULL();
		} else if (php_password_salt_is_alphabet(ZSTR_VAL(buffer), ZSTR_LEN(buffer)) == FAILURE) {
			salt = safe_emalloc(required_salt_len, 1, 1);
			if (php_password_salt_to64(ZSTR_VAL(buffer), ZSTR_LEN(buffer), required_salt_len, salt) == FAILURE) {
				efree(salt);
				php_error_docref(NULL, E_WARNING, "Provided salt is too short: %zd", ZSTR_LEN(buffer));
				zend_string_release(buffer);
				RETURN_NULL();
			}
			salt_len = required_salt_len;
		} else {
			salt = safe_emalloc(required_salt_len, 1, 1);
			memcpy(salt, ZSTR_VAL(buffer), required_salt_len);
			salt_len = required_salt_len;
		}
		zend_string_release(buffer);
	} else {
		salt = safe_emalloc(required_salt_len, 1, 1);
		if (php_password_make_salt(required_salt_len, salt) == FAILURE) {
			efree(salt);
			RETURN_FALSE;
		}
		salt_len = required_salt_len;
	}

	switch (algo) {
		case PHP_PASSWORD_BCRYPT:
			{
				zend_string *result;
				salt[salt_len] = 0;

				hash = safe_emalloc(salt_len + hash_format_len, 1, 1);
				sprintf(hash, "%s%s", hash_format, salt);
				hash[hash_format_len + salt_len] = 0;

				efree(salt);

				/* This cast is safe, since both values are defined here in code and cannot overflow */
				hash_len = (int) (hash_format_len + salt_len);

				if ((result = php_crypt(password, (int)password_len, hash, hash_len, 1)) == NULL) {
					efree(hash);
					RETURN_FALSE;
				}

				efree(hash);

				if (ZSTR_LEN(result) < 13) {
					zend_string_free(result);
					RETURN_FALSE;
				}

				RETURN_STR(result);
			}
			break;
#if HAVE_ARGON2LIB
		case PHP_PASSWORD_ARGON2I:
			{
				size_t out_len = 32;
				size_t encoded_len;
				int status = 0;
				char *out;
				zend_string *encoded;

				encoded_len = argon2_encodedlen(
					time_cost,
					memory_cost,
					threads,
					(uint32_t)salt_len,
					out_len
#if HAVE_ARGON2ID
					, type
#endif
				);

				out = emalloc(out_len + 1);
				encoded = zend_string_alloc(encoded_len, 0);

				status = argon2_hash(
					time_cost,
					memory_cost,
					threads,
					password,
					password_len,
					salt,
					salt_len,
					out,
					out_len,
					ZSTR_VAL(encoded),
					encoded_len,
					type,
					ARGON2_VERSION_NUMBER
				);

				efree(out);
				efree(salt);

				if (status != ARGON2_OK) {
					zend_string_free(encoded);
					php_error_docref(NULL, E_WARNING, argon2_error_message(status));
					RETURN_FALSE;
				}
					
				RETURN_STR(encoded);
			}
			break;
#endif
		default:
			RETURN_FALSE;
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
