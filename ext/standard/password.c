/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <stdlib.h>

#include "php.h"
#if HAVE_CRYPT

#include "fcntl.h"
#include "php_password.h"
#include "php_rand.h"
#include "php_crypt.h"
#include "base64.h"
#include "zend_interfaces.h"
#include "info.h"

#if PHP_WIN32
#include "win32/winutil.h"
#endif

PHP_MINIT_FUNCTION(password) /* {{{ */
{
	REGISTER_STRING_CONSTANT("PASSWORD_DEFAULT", PHP_PASSWORD_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PASSWORD_BCRYPT", PHP_PASSWORD_BCRYPT, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(password) /* {{{ */
{
	php_info_print_table_row(2, "Default Password BCrypt Cost", INI_STR("password.bcrypt_cost"));
}
/* }}} */

static int php_password_salt_is_alphabet(const char *str, const int len) /* {{{ */
{
	int i = 0;

	for (i = 0; i < len; i++) {
		if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/')) {
			return 0;
		}
	}
	return 1;
}
/* }}} */

static int php_password_salt_to64(const char *str, const int str_len, const int out_len, char *ret) /* {{{ */
{
	int pos = 0;
	unsigned char *buffer;
	buffer = php_base64_encode((unsigned char*) str, str_len, NULL);
	for (pos = 0; pos < out_len; pos++) {
		if (buffer[pos] == '+') {
			ret[pos] = '.';
		} else if (buffer[pos] == '=') {
			efree(buffer);
			return FAILURE;
		} else {
			ret[pos] = buffer[pos];
		}
	}
	efree(buffer);
	return SUCCESS;
}
/* }}} */

#define PHP_PASSWORD_FUNCTION_EXISTS(func, func_len) (zend_hash_find(EG(function_table), (func), (func_len) + 1, (void **) &func_ptr) == SUCCESS && func_ptr->type == ZEND_INTERNAL_FUNCTION && func_ptr->internal_function.handler != zif_display_disabled_function)

static int php_password_make_salt(long length, int raw, char *ret TSRMLS_DC) /* {{{ */
{
	int buffer_valid = 0;
	long i, raw_length;
	char *buffer;

	if (raw) {
		raw_length = length;
	} else {
		if (length > (LONG_MAX / 3)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length is too large to safely generate");
			return FAILURE;
		}
		raw_length = length * 3 / 4 + 1;
	}
	buffer = (char *) emalloc(raw_length + 1);

#if PHP_WIN32
	{
		BYTE *iv_b = (BYTE *) buffer;
		if (php_win32_get_random_bytes(iv_b, (size_t) raw_length) == SUCCESS) {
			buffer_valid = 1;
		}
	}
#else
	{
		int fd, n;
		size_t read_bytes = 0;
		fd = open("/dev/urandom", O_RDONLY);
		if (fd >= 0) {
			while (read_bytes < raw_length) {
				n = read(fd, buffer + read_bytes, raw_length - read_bytes);
				if (n < 0) {
					break;
				}
				read_bytes += n;
			}
			close(fd);
		}
		if (read_bytes == raw_length) {
			buffer_valid = 1;
		}
	}
#endif
	if (!buffer_valid) {
		for (i = 0; i < raw_length; i++) {
			buffer[i] ^= (char) (255.0 * php_rand(TSRMLS_C) / RAND_MAX);
		}
	}

	if (raw) {
		memcpy(ret, buffer, length);
	} else {
		char *result;
		result = emalloc(length + 1); 
		if (php_password_salt_to64(buffer, raw_length, length, result) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Generated salt too short");
			efree(buffer);
			efree(result);
			return FAILURE;
		} else {
			memcpy(ret, result, length);
			efree(result);
		}
	}
	efree(buffer);
	ret[length] = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ proto boolean password_make_salt(string password, string hash)
Verify a hash created using crypt() or password_hash() */
PHP_FUNCTION(password_verify)
{
	int status = 0, i;
	int password_len, hash_len;
	char *ret, *password, *hash;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &password, &password_len, &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (php_crypt(password, password_len, hash, hash_len, &ret) == FAILURE) {
		RETURN_FALSE;
	}

	if (strlen(ret) != hash_len) {
		efree(ret);
		RETURN_FALSE;
	}
	
	/* We're using this method instead of == in order to provide
	 * resistence towards timing attacks. This is a constant time
	 * equality check that will always check every byte of both
	 * values. */
	for (i = 0; i < hash_len; i++) {
		status |= (ret[i] ^ hash[i]);
	}

	efree(ret);

	RETURN_BOOL(status == 0);
	
}
/* }}} */

/* {{{ proto string password_make_salt(int length, boolean raw_output = false)
Make a new random salt */
PHP_FUNCTION(password_make_salt)
{
	char *salt;
	long length = 0;
	zend_bool raw_output = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|b", &length, &raw_output) == FAILURE) {
		RETURN_NULL();
	}
	if (length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length cannot be less than or equal zero: %ld", length);
		RETURN_NULL();
	} else if (length > (LONG_MAX / 3)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length is too large to safely generate");
		RETURN_NULL();
	}

	salt = emalloc(length + 1);
	if (php_password_make_salt(length, (int) raw_output, salt TSRMLS_CC) == FAILURE) {
		efree(salt);
		RETURN_FALSE;
	}
	RETURN_STRINGL(salt, length, 0);
}
/* }}} */

/* {{{ proto string password_hash(string password, string algo = PASSWORD_DEFAULT, array options = array())
Hash a password */
PHP_FUNCTION(password_hash)
{
	char *algo = 0, *hash_format, *hash, *salt, *password, *result;
	int algo_len = 0, salt_len = 0, required_salt_len = 0, hash_format_len, password_len;
	HashTable *options = 0;
	zval **option_buffer;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sH", &password, &password_len, &algo, &algo_len, &options) == FAILURE) {
		RETURN_NULL();
	}

	if (algo_len == 0) {
		algo = PHP_PASSWORD_DEFAULT;
		algo_len = strlen(PHP_PASSWORD_DEFAULT);
	}

	if (strcmp(algo, PHP_PASSWORD_BCRYPT) == 0) {
		int cost = 0;
		cost = (int) INI_INT("password.bcrypt_cost");

		if (options && zend_symtable_find(options, "cost", 5, (void **) &option_buffer) == SUCCESS) {
			convert_to_long_ex(option_buffer);
			cost = Z_LVAL_PP(option_buffer);
			zval_ptr_dtor(option_buffer);
		}

		if (cost < 4 || cost > 31) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid bcrypt cost parameter specified: %d", cost);
			RETURN_NULL();
		}
		
		required_salt_len = 22;
		hash_format = emalloc(8);
		sprintf(hash_format, "$2y$%02d$", cost);
		hash_format_len = 7;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown password hashing algorithm: %s", algo);
		RETURN_NULL();
	}

	if (options && zend_symtable_find(options, "salt", 5, (void**) &option_buffer) == SUCCESS) {
		char *buffer;
		int buffer_len;
		switch (Z_TYPE_PP(option_buffer)) {
			case IS_NULL:
			case IS_STRING:
			case IS_LONG:
			case IS_DOUBLE:
			case IS_BOOL:
			case IS_OBJECT:
				convert_to_string_ex(option_buffer);
				if (Z_TYPE_PP(option_buffer) == IS_STRING) {
					buffer = Z_STRVAL_PP(option_buffer);
					buffer_len = Z_STRLEN_PP(option_buffer);
					break;
				}
			case IS_RESOURCE:
			case IS_ARRAY:
			default:
				zval_ptr_dtor(option_buffer);
				efree(hash_format);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Non-string salt parameter supplied");
				RETURN_NULL();
		}
		if (buffer_len < required_salt_len) {
			efree(hash_format);
			zval_ptr_dtor(option_buffer);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Provided salt is too short: %d expecting %d", buffer_len, required_salt_len);
			RETURN_NULL();
		} else if (0 == php_password_salt_is_alphabet(buffer, buffer_len)) {
			salt = emalloc(required_salt_len + 1);
			if (php_password_salt_to64(buffer, buffer_len, required_salt_len, salt) == FAILURE) {
				efree(hash_format);
				efree(salt);
				zval_ptr_dtor(option_buffer);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Provided salt is too short: %d", salt_len);
				RETURN_NULL();
			}
			salt_len = required_salt_len;
		} else {
			salt = emalloc(required_salt_len + 1);
			memcpy(salt, buffer, required_salt_len);
			salt_len = required_salt_len;
		}
		zval_ptr_dtor(option_buffer);
	} else {
		salt = emalloc(required_salt_len + 1);
		if (php_password_make_salt((long) required_salt_len, 0, salt TSRMLS_CC) == FAILURE) {
			efree(hash_format);
			efree(salt);
			RETURN_FALSE;
		}
		salt_len = required_salt_len;
	}
	
	salt[salt_len] = 0;

	hash = emalloc(salt_len + hash_format_len + 1);
	sprintf(hash, "%s%s", hash_format, salt);
	hash[hash_format_len + salt_len] = 0;

	efree(hash_format);
	efree(salt);

	if (php_crypt(password, password_len, hash, hash_format_len + salt_len, &result) == FAILURE) {
		efree(hash);
		RETURN_FALSE;
	}

	efree(hash);

	if (strlen(result) < 13) {
		efree(result);
		RETURN_FALSE;
	}

	RETURN_STRING(result, 0);
}
/* }}} */

#endif /* HAVE_CRYPT */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
