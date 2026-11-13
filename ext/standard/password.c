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
#include "ext/random/php_random_csprng.h"
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
	if (FAILURE == php_random_bytes_throw(ZSTR_VAL(buffer), ZSTR_LEN(buffer))) {
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

static bool php_password_bcrypt_get_cost(const zend_string *hash, zend_long *cost) {
	const char *h = ZSTR_VAL(hash);

	if ((ZSTR_LEN(hash) != 60) ||
		(h[0] != '$') || (h[1] != '2') || (h[2] != 'y') || (h[3] != '$') ||
		!ZEND_IS_DIGIT(h[4]) || !ZEND_IS_DIGIT(h[5]) || (h[6] != '$')) {
		return false;
	}

	*cost = (h[4] - '0') * 10 + (h[5] - '0');

	return *cost >= 4 && *cost <= 31;
}

static bool php_password_bcrypt_valid(const zend_string *hash) {
	zend_long cost;

	return php_password_bcrypt_get_cost(hash, &cost);
}

static int php_password_bcrypt_get_info(zval *return_value, const zend_string *hash) {
	zend_long cost;

	if (!php_password_bcrypt_get_cost(hash, &cost)) {
		/* Should never get called this way. */
		return FAILURE;
	}
	add_assoc_long(return_value, "cost", cost);

	return SUCCESS;
}

static bool php_password_bcrypt_needs_rehash(const zend_string *hash, zend_array *options) {
	zval *znew_cost;
	zend_long old_cost;
	zend_long new_cost = PHP_PASSWORD_BCRYPT_COST;

	if (!php_password_bcrypt_get_cost(hash, &old_cost)) {
		/* Should never get called this way. */
		return 1;
	}
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

	if (memchr(ZSTR_VAL(password), '\0', ZSTR_LEN(password))) {
		zend_value_error("Bcrypt password must not contain null character");
		return NULL;
	}

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

	hash = zend_string_concat2(hash_format, hash_format_len, ZSTR_VAL(salt), ZSTR_LEN(salt));

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

typedef struct php_password_argon2_ops {
	argon2_version v;
	uint32_t m;
	uint32_t t;
	uint8_t p;
	uint8_t salt_len;
	uint8_t digest_len;
} php_password_argon2_ops;

static bool php_password_argon2_get_options(const zend_array *options, php_password_argon2_ops *ops, const bool silent) {
	zval *zv;
	zend_long zl;
	*ops = (php_password_argon2_ops) {
		.v = ARGON2_VERSION_13, /* Don't know the implications of a future version */
		.m = PHP_PASSWORD_ARGON2_MEMORY_COST,
		.t = PHP_PASSWORD_ARGON2_TIME_COST,
		.p = PHP_PASSWORD_ARGON2_THREADS,
		.salt_len = 16,
		.digest_len = 32
	};

	if (options && (zv = zend_hash_str_find(options, "memory_cost", sizeof("memory_cost")-1)) != NULL) {
		zl = zval_get_long(zv);
		if (zl >= ARGON2_MIN_MEMORY && zl <= ARGON2_MAX_MEMORY) {
			ops->m = zl;
		} else if (!silent) {
			zend_value_error("Memory cost is outside of allowed memory range");
			return false;
		}
	}

	if (options && (zv = zend_hash_str_find(options, "time_cost", sizeof("time_cost")-1)) != NULL) {
		zl = zval_get_long(zv);
		if (zl >= ARGON2_MIN_TIME && zl <= ARGON2_MAX_TIME) {
			ops->t = zl;
		} else if (!silent) {
			zend_value_error("Time cost is outside of allowed time range");
			return false;
		}
	}

	if (options && (zv = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
		zl = zval_get_long(zv);
		/* ARGON2_MAX_LANES is a theoretical KDF value; 255 is specified by PHC strings. */
		if (zl >= ARGON2_MIN_LANES && zl <= 255) {
			ops->p = zl;
		} else if (!silent) {
			zend_value_error("Invalid number of threads");
			return false;
		}
	}

	return true;
}

static inline bool php_password_argon2_parse_prefix(const char **ptr, argon2_version *v) {
	argon2_type type;
	const char *h = *ptr;
	if (memcmp(h, "$argon2id$", sizeof("$argon2id$")-1) == 0) {
		type = Argon2_id;
		h += sizeof("$argon2id$")-1;
	} else if (memcmp(h, "$argon2i$", sizeof("$argon2i$")-1) == 0) {
		type = Argon2_i;
		h += sizeof("$argon2i$")-1;
	} else {
		return false;
	}

	if (memcmp(h, "v=19$", 5) == 0) {
		*v = ARGON2_VERSION_13;
		h += 5;
	/* 1.0 hashes didn't tag the version, but it could be set by future implementers */
	} else if (memcmp(h, "v=16$", 5) == 0) {
		*v = ARGON2_VERSION_10;
		h += 5;
	} else if (h[0] == 'm') {
		*v = ARGON2_VERSION_10;
	} else {
		return false;
	}

	*ptr = h;
	/* Argon2id was introduced by version 1.3 */
	return !(type == Argon2_id && *v == ARGON2_VERSION_10);
}

static inline bool php_password_argon2_parse_param(const char **ptr, const char param, const uint32_t maxval, zend_ulong *outval) {
	const char *h = *ptr;
	if (h[0] != param || h[1] != '=' || h[2] < '1' || h[2] > '9') {
		return false;
	}

	char *dptr = NULL;
	zend_ulong val = ZEND_STRTOUL(h + 2, &dptr, 10);
	if (*dptr != (param == 'p' ? '$' : ',') || val > maxval) {
		return false;
	}
	*outval = val;
	*ptr = (const char *)dptr + 1;
	return true;
}

static bool php_password_argon2_parse(const zend_string *hash, php_password_argon2_ops *ops) {
	/* Min/max allowed by PHC strings. keyid and data params excluded as unsupported. */
	if (ZSTR_LEN(hash) < 49 || ZSTR_LEN(hash) > 198) {
		return false;
	}

	argon2_version v;
	zend_ulong m, t, p;
	const char *hashptr = ZSTR_VAL(hash);
	// Don't use ARGON2_MAX_* values here; it gets weird on 32bit builds
	if (!php_password_argon2_parse_prefix(&hashptr, &v) ||
		!php_password_argon2_parse_param(&hashptr, 'm', UINT32_MAX, &m) ||
		!php_password_argon2_parse_param(&hashptr, 't', UINT32_MAX, &t) ||
		!php_password_argon2_parse_param(&hashptr, 'p', 255, &p)) {
		return false;
	}

	if (m < p * 8) {
		return false;
	}

	const char *end = ZSTR_VAL(hash) + ZSTR_LEN(hash);
	uint8_t s_len = 0, d_len = 0;
	for (bool is_salt = true; hashptr < end; hashptr++) {
		char c = *hashptr;
		if (c == '+' || (c >= '/' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			(is_salt) ? s_len++ : d_len++;
		} else if (c == '$' && is_salt) {
			is_salt = false;
		} else {
			return false;
		}
	}

	if (s_len < 11 || s_len > 64 || s_len % 4 == 1 || d_len < 16 || d_len > 86 || d_len % 4 == 1) {
		return false;
	}

	if (ops != NULL) {
		*ops = (php_password_argon2_ops) {
			.v = v, .m = m, .t = t, .p = p,
			/* B64 length to binary size */
			.salt_len = (3 * s_len) / 4,
			.digest_len = (3 * d_len) / 4
		};
	}

	return true;
}

static bool php_password_argon2_valid(const zend_string *hash) {
	return php_password_argon2_parse(hash, NULL);
}

static int php_password_argon2_get_info(zval *return_value, const zend_string *hash) {
	php_password_argon2_ops ops;
	if (!php_password_argon2_parse(hash, &ops)) {
		return FAILURE;
	}

	add_assoc_long(return_value, "memory_cost", ops.m);
	add_assoc_long(return_value, "time_cost", ops.t);
	/* Note: threads is a misnomer here, but it was voted-in as is. */
	add_assoc_long(return_value, "threads", ops.p);

	return SUCCESS;
}

static bool php_password_argon2_needs_rehash(const zend_string *hash, zend_array *options) {
	php_password_argon2_ops new, old;

	if (!php_password_argon2_get_options(options, &new, true)) {
		return false;
	}

	if (!php_password_argon2_parse(hash, &old)) {
		return true;
	}

	/* Keeping this separate as overriding to other conditions. */
	if (old.v > ARGON2_VERSION_NUMBER) {
		return false;
	}

	/* If anything except m,t,p differs, only rehash if it would be an upgrade */
	return new.v > old.v || new.salt_len > old.salt_len || new.digest_len > old.digest_len ||
		new.m != old.m || new.t != old.t || new.p != old.p;
}

static zend_string *php_password_argon2_hash(const zend_string *password, zend_array *options, const argon2_type type) {
	if (ZSTR_LEN(password) > ARGON2_MAX_PWD_LENGTH) {
		zend_value_error("Password is too long");
		return NULL;
	}

	php_password_argon2_ops ops;
	if (!php_password_argon2_get_options(options, &ops, false)) {
		return NULL;
	}

	zend_string *salt;
	if (!(salt = php_password_get_salt(NULL, ops.salt_len, options))) {
		return NULL;
	}

	uint8_t encoded_len = argon2_encodedlen(
		ops.t,
		ops.m,
		ops.p,
		ops.salt_len,
		ops.digest_len,
		type
	);

	zend_string *encoded = zend_string_alloc(encoded_len - 1, 0);
	const int status = argon2_hash(
		ops.t,
		ops.m,
		ops.p,
		ZSTR_VAL(password),
		ZSTR_LEN(password),
		ZSTR_VAL(salt),
		ZSTR_LEN(salt),
		NULL,
		ops.digest_len,
		ZSTR_VAL(encoded),
		encoded_len,
		type,
		ops.v
	);

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
	return ARGON2_OK == argon2i_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password));
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
	php_password_argon2_valid,
};

/* argon2id specific methods */

static bool php_password_argon2id_verify(const zend_string *password, const zend_string *hash) {
	return ARGON2_OK == argon2id_verify(ZSTR_VAL(hash), ZSTR_VAL(password), ZSTR_LEN(password));
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
	php_password_argon2_valid,
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
