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

#include "fcntl.h"
#include "php_password.h"
#include "php_rand.h"
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

static int php_password_salt_is_alphabet(const char *str, const int len)
{
        int i = 0;

        for (i = 0; i < len; i++) {
                if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/')) {
                        return 0;
                }
        }
        return 1;
}

static int php_password_salt_to64(const char *str, const int str_len, const int out_len, char *ret)
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

#define PHP_PASSWORD_FUNCTION_EXISTS(func, func_len) (zend_hash_find(EG(function_table), (func), (func_len) + 1, (void **) &func_ptr) == SUCCESS && func_ptr->type == ZEND_INTERNAL_FUNCTION && func_ptr->internal_function.handler != zif_display_disabled_function)

static int php_password_make_salt(int length, int raw, char *ret TSRMLS_DC)
{
	int i, raw_length, buffer_valid = 0;
	char *buffer;

	if (raw) {
		raw_length = length;
	} else {
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
	/* /Temp Placeholder */

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

PHP_FUNCTION(password_verify)
{
	zval *password, *hash, *ret;
	int status = 0, i;
	zend_function *func_ptr;

	if (!PHP_PASSWORD_FUNCTION_EXISTS("crypt", 5)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crypt must be loaded for password_verify to function");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &password, &hash) == FAILURE) {
                RETURN_FALSE;
        }

	zend_call_method_with_2_params(NULL, NULL, NULL, "crypt", &ret, password, hash);
	
	if (Z_TYPE_P(ret) != IS_STRING) {
		zval_ptr_dtor(&ret);
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(ret) != Z_STRLEN_P(hash)) {
		zval_ptr_dtor(&ret);
		RETURN_FALSE;
	}
	
	/* We're using this method instead of == in order to provide
	 * resistence towards timing attacks. This is a constant time
	 * equality check that will always check every byte of both
	 * values. */
	for (i = 0; i < Z_STRLEN_P(ret); i++) {
		status |= (Z_STRVAL_P(ret)[i] ^ Z_STRVAL_P(hash)[i]);
	}

	zval_ptr_dtor(&ret);

	RETURN_BOOL(status == 0);
	
}

PHP_FUNCTION(password_make_salt)
{
	char *salt;
	int length = 0;
	zend_bool raw_output = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|b", &length, &raw_output) == FAILURE) {
                RETURN_FALSE;
        }
	if (length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length cannot be less than or equal zero: %d", length);
		RETURN_FALSE;
	}
	salt = emalloc(length + 1);
	if (php_password_make_salt(length, (int) raw_output, salt TSRMLS_CC) == FAILURE) {
		efree(salt);
		RETURN_FALSE;
	}
	RETURN_STRINGL(salt, length, 0);
}


/* {{{ proto string password_hash(string password, string algo = PASSWORD_DEFAULT, array options = array())
Hash a password */
PHP_FUNCTION(password_hash)
{
        char *algo = 0, *hash_format, *hash, *salt;
        int algo_len = 0, salt_len = 0, required_salt_len = 0, hash_format_len;
        HashTable *options = 0;
        zval **option_buffer, *ret, *password, *hash_zval;
	zend_function *func_ptr;

	if (!PHP_PASSWORD_FUNCTION_EXISTS("crypt", 5)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crypt must be loaded for password_hash to function");
		RETURN_FALSE;
	}

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|sH", &password, &algo, &algo_len, &options) == FAILURE) {
                RETURN_FALSE;
        }

	if (Z_TYPE_P(password) != IS_STRING) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Password must be a string");
		RETURN_FALSE;
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
        	        RETURN_FALSE;
		}
		
                required_salt_len = 22;
		hash_format = emalloc(8);
		sprintf(hash_format, "$2y$%02d$", cost);
		hash_format_len = 7;
        } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown password hashing algorithm: %s", algo);
                RETURN_FALSE;
        }

        if (options && zend_symtable_find(options, "salt", 5, (void**) &option_buffer) == SUCCESS) {
		char *buffer;
		int buffer_len;
                if (Z_TYPE_PP(option_buffer) == IS_STRING) {
                        buffer = Z_STRVAL_PP(option_buffer);
                        buffer_len = Z_STRLEN_PP(option_buffer);
                } else {
                        zval_ptr_dtor(option_buffer);
			efree(hash_format);
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Non-string salt parameter supplied");
                        RETURN_FALSE;
                }
                if (buffer_len < required_salt_len) {
			efree(hash_format);
		        zval_ptr_dtor(option_buffer);
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Provided salt is too short: %d expecting %d", buffer_len, required_salt_len);
                        RETURN_FALSE;
                } else if (0 == php_password_salt_is_alphabet(buffer, buffer_len)) {
			salt = emalloc(required_salt_len + 1);
                        if (php_password_salt_to64(buffer, buffer_len, required_salt_len, salt) == FAILURE) {
				efree(hash_format);
				efree(salt);
			        zval_ptr_dtor(option_buffer);
	                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Provided salt is too short: %d", salt_len);
				RETURN_FALSE;
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
		if (php_password_make_salt(required_salt_len, 0, salt TSRMLS_CC) == FAILURE) {
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

	ALLOC_INIT_ZVAL(hash_zval);
	ZVAL_STRINGL(hash_zval, hash, hash_format_len + salt_len, 0);

	efree(hash_format);
	efree(salt);

	zend_call_method_with_2_params(NULL, NULL, NULL, "crypt", &ret, password, hash_zval);

	zval_ptr_dtor(&hash_zval);

	if (Z_TYPE_P(ret) != IS_STRING) {
		zval_ptr_dtor(&ret);
		RETURN_FALSE;
	} else if(Z_STRLEN_P(ret) < 13) {
		zval_ptr_dtor(&ret);
		RETURN_FALSE;
	}

	RETURN_ZVAL(ret, 0, 1);
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
