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
#include "ext/random/php_random.h"
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
		zend_value_error("Length is too large to safely generate");
		return NULL;
	}

	buffer = zend_string_alloc(length * 3 / 4 + 1, 0);
	if (FAILURE == php_random_bytes_silent(ZSTR_VAL(buffer), ZSTR_LEN(buffer))) {
		zend_value_error("Unable to generate salt");
		zend_string_release_ex(buffer, 0);
		return NULL;
	}

	ret = zend_string_alloc(length, 0);
	if (php_password_salt_to64(ZSTR_VAL(buffer), ZSTR_LEN(buffer), length, ZSTR_VAL(ret)) == FAILURE) {
		zend_value_error("Generated salt too short");
		zend_string_release_ex(buffer, 0);
		zend_string_release_ex(ret, 0);
		return NULL;
	}
	zend_string_release_ex(buffer, 0);
	ZSTR_VAL(ret)[length] = 0;
	return ret;
}
/* }}} */

static zend_string* php_password_get_salt(zval *unused_, size_t required_salt_len, HashTable *options) {
	if (options && zend_hash_str_exists(options, "salt", sizeof("salt") - 1)) {
		php_error_docref(NULL, E_WARNING, "The \"salt\" option has been ignored, since providing a custom salt is no longer supported");
	}

	return php_password_make_salt(required_salt_len);
}

/* bcrypt implementation */

static bool php_password_bcrypt_valid(const zend_string *hash) {
	const char *h = ZSTR_VAL(hash);
	return (ZSTR_LEN(hash) == 60) &&
		(h[0] == '$') && (h[1] == '2') && (h[2] == 'y');
}

static int php_password_bcrypt_get_info(zval *return_value, const zend_string *hash) {
	zend_long cost = PHP_PASSWORD_BCRYPT_COST;

	if (!php_password_bcrypt_valid(hash)) {
		/* Should never get called this way. */
		return FAILURE;
	}

	sscanf(ZSTR_VAL(hash), "$2y$" ZEND_LONG_FMT "$", &cost);
	add_assoc_long(return_value, "cost", cost);

	return SUCCESS;
}

static bool php_password_bcrypt_needs_rehash(const zend_string *hash, zend_array *options) {
	zval *znew_cost;
	zend_long old_cost = PHP_PASSWORD_BCRYPT_COST;
	zend_long new_cost = PHP_PASSWORD_BCRYPT_COST;

	if (!php_password_bcrypt_valid(hash)) {
		/* Should never get called this way. */
		return 1;
	}

	sscanf(ZSTR_VAL(hash), "$2y$" ZEND_LONG_FMT "$", &old_cost);
	if (options && (znew_cost = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
		new_cost = zval_get_long(znew_cost);
	}

	return old_cost != new_cost;
}

static bool php_password_bcrypt_verify(const zend_string *password, const zend_string *hash) {
	int status = 0;
	zend_string *ret = php_crypt(ZSTR_VAL(password), (int)ZSTR_LEN(password), ZSTR_VAL(hash), (int)ZSTR_LEN(hash), 1);

	if (!ret) {
		return 0;
	}

	if (ZSTR_LEN(hash) < 13) {
		zend_string_free(ret);
		return 0;
	}

	/* We're using this method instead of == in order to provide
	 * resistance towards timing attacks. This is a constant time
	 * equality check that will always check every byte of both
	 * values. */
	status = php_safe_bcmp(ret, hash);

	zend_string_free(ret);
	return status == 0;
}

static zend_string* php_password_bcrypt_hash(const zend_string *password, zend_array *options) {
	char hash_format[10];
	size_t hash_format_len;
	zend_string *result, *hash, *salt;
	zval *zcost;
	zend_long cost = PHP_PASSWORD_BCRYPT_COST;

	if (options && (zcost = zend_hash_str_find(options, "cost", sizeof("cost")-1)) != NULL) {
		cost = zval_get_long(zcost);
	}

	if (cost < 4 || cost > 31) {
		zend_value_error("Invalid bcrypt cost parameter specified: " ZEND_LONG_FMT, cost);
		return NULL;
	}

	hash_format_len = snprintf(hash_format, sizeof(hash_format), "$2y$%02" ZEND_LONG_FMT_SPEC "$", cost);
	if (!(salt = php_password_get_salt(NULL, Z_UL(22), options))) {
		return NULL;
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
		return NULL;
	}

	if (ZSTR_LEN(result) < 13) {
		zend_string_free(result);
		return NULL;
	}

	return result;
}

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
	zend_string *salt, *out, *encoded;
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

	if (!(salt = php_password_get_salt(NULL, Z_UL(16), options))) {
		return NULL;
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
	REGISTER_STRING_CONSTANT("PASSWORD_DEFAULT", "2y", CONST_CS | CONST_PERSISTENT);

	if (FAILURE == php_password_algo_register("2y", &php_password_algo_bcrypt)) {
		return FAILURE;
	}
	REGISTER_STRING_CONSTANT("PASSWORD_BCRYPT", "2y", CONST_CS | CONST_PERSISTENT);

#ifdef HAVE_ARGON2LIB
	if (FAILURE == php_password_algo_register("argon2i", &php_password_algo_argon2i)) {
		return FAILURE;
	}
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2I", "argon2i", CONST_CS | CONST_PERSISTENT);

	if (FAILURE == php_password_algo_register("argon2id", &php_password_algo_argon2id)) {
		return FAILURE;
	}
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2ID", "argon2id", CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("PASSWORD_BCRYPT_DEFAULT_COST", PHP_PASSWORD_BCRYPT_COST, CONST_CS | CONST_PERSISTENT);
#ifdef HAVE_ARGON2LIB
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_MEMORY_COST", PHP_PASSWORD_ARGON2_MEMORY_COST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_TIME_COST", PHP_PASSWORD_ARGON2_TIME_COST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_THREADS", PHP_PASSWORD_ARGON2_THREADS, CONST_CS | CONST_PERSISTENT);

	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2_PROVIDER", "standard", CONST_CS | CONST_PERSISTENT);
#endif

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
			zend_string *n = zend_string_init("argon2i", sizeof("argon2i")-1, 0);
			const php_password_algo* ret = php_password_algo_find(n);
			zend_string_release(n);
			return ret;
			}
		case 3:
			{
			zend_string *n = zend_string_init("argon2id", sizeof("argon2id")-1, 0);
			const php_password_algo* ret = php_password_algo_find(n);
			zend_string_release(n);
			return ret;
			}
#endif
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
	zend_long new_algo_long;
	bool new_algo_is_null;
	zend_array *options = 0;

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
	zend_long algo_long;
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
