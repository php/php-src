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
   | Authors: Stig Bakken <ssb@php.net>                                   |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Pierre Joye <pierre@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>

#include "php.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if PHP_USE_PHP_CRYPT_R
# include "php_crypt_r.h"
# include "crypt_freesec.h"
#else
# ifdef HAVE_CRYPT_H
#  if defined(CRYPT_R_GNU_SOURCE) && !defined(_GNU_SOURCE)
#   define _GNU_SOURCE
#  endif
#  include <crypt.h>
# endif
#endif
#include <time.h>
#include <string.h>

#ifdef PHP_WIN32
#include <process.h>
#endif

#include "php_crypt.h"
#include "ext/random/php_random.h"

/* Used to check DES salts to ensure that they contain only valid characters */
#define IS_VALID_SALT_CHARACTER(c) (((c) >= '.' && (c) <= '9') || ((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))

PHP_MINIT_FUNCTION(crypt) /* {{{ */
{
#if PHP_USE_PHP_CRYPT_R
	php_init_crypt_r();
#endif

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(crypt) /* {{{ */
{
#if PHP_USE_PHP_CRYPT_R
	php_shutdown_crypt_r();
#endif

	return SUCCESS;
}
/* }}} */

PHPAPI zend_string *php_crypt(const char *password, const int pass_len, const char *salt, int salt_len, bool quiet)
{
	char *crypt_res;
	zend_string *result;

	if (salt[0] == '*' && (salt[1] == '0' || salt[1] == '1')) {
		return NULL;
	}

/* Windows (win32/crypt) has a stripped down version of libxcrypt and
	a CryptoApi md5_crypt implementation */
#if PHP_USE_PHP_CRYPT_R
	{
		struct php_crypt_extended_data buffer;

		if (salt[0]=='$' && salt[1]=='1' && salt[2]=='$') {
			char output[MD5_HASH_MAX_LEN], *out;

			out = php_md5_crypt_r(password, salt, output);
			if (out) {
				return zend_string_init(out, strlen(out), 0);
			}
			return NULL;
		} else if (salt[0]=='$' && salt[1]=='6' && salt[2]=='$') {
			char *output;
			output = emalloc(PHP_MAX_SALT_LEN);

			crypt_res = php_sha512_crypt_r(password, salt, output, PHP_MAX_SALT_LEN);
			if (!crypt_res) {
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN);
				efree(output);
				return NULL;
			} else {
				result = zend_string_init(output, strlen(output), 0);
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN);
				efree(output);
				return result;
			}
		} else if (salt[0]=='$' && salt[1]=='5' && salt[2]=='$') {
			char *output;
			output = emalloc(PHP_MAX_SALT_LEN);

			crypt_res = php_sha256_crypt_r(password, salt, output, PHP_MAX_SALT_LEN);
			if (!crypt_res) {
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN);
				efree(output);
				return NULL;
			} else {
				result = zend_string_init(output, strlen(output), 0);
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN);
				efree(output);
				return result;
			}
		} else if (
				salt[0] == '$' &&
				salt[1] == '2' &&
				salt[2] != 0 &&
				salt[3] == '$') {
			char output[PHP_MAX_SALT_LEN + 1];

			memset(output, 0, PHP_MAX_SALT_LEN + 1);

			crypt_res = php_crypt_blowfish_rn(password, salt, output, sizeof(output));
			if (!crypt_res) {
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
				return NULL;
			} else {
				result = zend_string_init(output, strlen(output), 0);
				ZEND_SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
				return result;
			}
		} else if (salt[0] == '_'
				|| (IS_VALID_SALT_CHARACTER(salt[0]) && IS_VALID_SALT_CHARACTER(salt[1]))) {
			/* DES Fallback */
			memset(&buffer, 0, sizeof(buffer));
			_crypt_extended_init_r();

			crypt_res = _crypt_extended_r((const unsigned char *) password, salt, &buffer);
			if (!crypt_res || (salt[0] == '*' && salt[1] == '0')) {
				return NULL;
			} else {
				result = zend_string_init(crypt_res, strlen(crypt_res), 0);
				return result;
			}
		} else {
			/* Unknown hash type */
			return NULL;
		}
	}
#else

# if defined(HAVE_CRYPT_R) && (defined(_REENTRANT) || defined(_THREAD_SAFE))
#  if defined(CRYPT_R_STRUCT_CRYPT_DATA)
	struct crypt_data buffer;
	memset(&buffer, 0, sizeof(buffer));
#  elif defined(CRYPT_R_CRYPTD)
	CRYPTD buffer;
#  else
#   error Data struct used by crypt_r() is unknown. Please report.
#  endif
	crypt_res = crypt_r(password, salt, &buffer);
# elif defined(HAVE_CRYPT)
	crypt_res = crypt(password, salt);
# else
#  error No crypt() implementation
# endif
#endif

	if (!crypt_res || (salt[0] == '*' && salt[1] == '0')) {
		return NULL;
	} else {
		result = zend_string_init(crypt_res, strlen(crypt_res), 0);
		return result;
	}
}
/* }}} */


/* {{{ Hash a string */
PHP_FUNCTION(crypt)
{
	char salt[PHP_MAX_SALT_LEN + 1];
	char *str, *salt_in = NULL;
	size_t str_len, salt_in_len = 0;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_STRING(salt_in, salt_in_len)
	ZEND_PARSE_PARAMETERS_END();

	salt[0] = salt[PHP_MAX_SALT_LEN] = '\0';

	/* This will produce suitable results if people depend on DES-encryption
	 * available (passing always 2-character salt). At least for glibc6.1 */
	memset(&salt[1], '$', PHP_MAX_SALT_LEN - 1);
	memcpy(salt, salt_in, MIN(PHP_MAX_SALT_LEN, salt_in_len));

	salt_in_len = MIN(PHP_MAX_SALT_LEN, salt_in_len);
	salt[salt_in_len] = '\0';

	if ((result = php_crypt(str, (int)str_len, salt, (int)salt_in_len, 0)) == NULL) {
		if (salt[0] == '*' && salt[1] == '0') {
			RETURN_STRING("*1");
		} else {
			RETURN_STRING("*0");
		}
	}
	RETURN_STR(result);
}
/* }}} */
