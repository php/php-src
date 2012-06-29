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
   | Authors: Stig Bakken <ssb@php.net>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Pierre Joye <pierre@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <stdlib.h>

#include "php.h"
#if HAVE_CRYPT

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if PHP_USE_PHP_CRYPT_R
# include "php_crypt_r.h"
# include "crypt_freesec.h"
#else
# if HAVE_CRYPT_H
#  if defined(CRYPT_R_GNU_SOURCE) && !defined(_GNU_SOURCE)
#   define _GNU_SOURCE
#  endif
#  include <crypt.h>
# endif
#endif
#if TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef PHP_WIN32
#include <process.h>
#endif

#include "php_lcg.h"
#include "php_crypt.h"
#include "php_rand.h"

/* The capabilities of the crypt() function is determined by the test programs
 * run by configure from aclocal.m4.  They will set PHP_STD_DES_CRYPT,
 * PHP_EXT_DES_CRYPT, PHP_MD5_CRYPT and PHP_BLOWFISH_CRYPT as appropriate
 * for the target platform. */

#if PHP_STD_DES_CRYPT
#define PHP_MAX_SALT_LEN 2
#endif

#if PHP_EXT_DES_CRYPT
#undef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 9
#endif

#if PHP_MD5_CRYPT
#undef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 12
#endif

#if PHP_BLOWFISH_CRYPT
#undef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 60
#endif

#if PHP_SHA512_CRYPT
#undef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 123
#endif


/* If the configure-time checks fail, we provide DES.
 * XXX: This is a hack. Fix the real problem! */

#ifndef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 2
#undef PHP_STD_DES_CRYPT
#define PHP_STD_DES_CRYPT 1
#endif

#define PHP_CRYPT_RAND php_rand(TSRMLS_C)

PHP_MINIT_FUNCTION(crypt) /* {{{ */
{
	REGISTER_LONG_CONSTANT("CRYPT_SALT_LENGTH", PHP_MAX_SALT_LEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_STD_DES", PHP_STD_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_EXT_DES", PHP_EXT_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_MD5", PHP_MD5_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_BLOWFISH", PHP_BLOWFISH_CRYPT, CONST_CS | CONST_PERSISTENT);

#ifdef PHP_SHA256_CRYPT
   REGISTER_LONG_CONSTANT("CRYPT_SHA256", PHP_SHA256_CRYPT, CONST_CS | CONST_PERSISTENT);
#endif

#ifdef PHP_SHA512_CRYPT
   REGISTER_LONG_CONSTANT("CRYPT_SHA512", PHP_SHA512_CRYPT, CONST_CS | CONST_PERSISTENT);
#endif

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

static unsigned char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void php_to64(char *s, long v, int n) /* {{{ */
{
	while (--n >= 0) {
		*s++ = itoa64[v&0x3f];
		v >>= 6;
	}
}
/* }}} */

/* {{{ proto string crypt(string str [, string salt])
   Hash a string */
PHP_FUNCTION(crypt)
{
	char salt[PHP_MAX_SALT_LEN + 1];
	char *str, *salt_in = NULL;
	int str_len, salt_in_len = 0;
	char *crypt_res;
	salt[0] = salt[PHP_MAX_SALT_LEN] = '\0';

	/* This will produce suitable results if people depend on DES-encryption
	 * available (passing always 2-character salt). At least for glibc6.1 */
	memset(&salt[1], '$', PHP_MAX_SALT_LEN - 1);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &str, &str_len, &salt_in, &salt_in_len) == FAILURE) {
		return;
	}

	if (salt_in) {
		memcpy(salt, salt_in, MIN(PHP_MAX_SALT_LEN, salt_in_len));
	}

	/* The automatic salt generation covers standard DES, md5-crypt and Blowfish (simple) */
	if (!*salt) {
#if PHP_MD5_CRYPT
		strncpy(salt, "$1$", PHP_MAX_SALT_LEN);
		php_to64(&salt[3], PHP_CRYPT_RAND, 4);
		php_to64(&salt[7], PHP_CRYPT_RAND, 4);
		strncpy(&salt[11], "$", PHP_MAX_SALT_LEN - 11);
#elif PHP_STD_DES_CRYPT
		php_to64(&salt[0], PHP_CRYPT_RAND, 2);
		salt[2] = '\0';
#endif
		salt_in_len = strlen(salt);
	} else {
		salt_in_len = MIN(PHP_MAX_SALT_LEN, salt_in_len);
	}

/* Windows (win32/crypt) has a stripped down version of libxcrypt and 
	a CryptoApi md5_crypt implementation */
#if PHP_USE_PHP_CRYPT_R
	{
		struct php_crypt_extended_data buffer;

		if (salt[0]=='$' && salt[1]=='1' && salt[2]=='$') {
			char output[MD5_HASH_MAX_LEN];

			RETURN_STRING(php_md5_crypt_r(str, salt, output), 1);
		} else if (salt[0]=='$' && salt[1]=='6' && salt[2]=='$') {
			const char sha512_salt_prefix[] = "$6$";
			const char sha512_rounds_prefix[] = "rounds=";
			char *output;
			int needed = (sizeof(sha512_salt_prefix) - 1
						+ sizeof(sha512_rounds_prefix) + 9 + 1
						+ salt_in_len + 1 + 86 + 1);
			output = emalloc(needed);
			salt[salt_in_len] = '\0';

			crypt_res = php_sha512_crypt_r(str, salt, output, needed);
			if (!crypt_res) {
				if (salt[0]=='*' && salt[1]=='0') {
					RETVAL_STRING("*1", 1);
				} else {
					RETVAL_STRING("*0", 1);
				}
			} else {
				RETVAL_STRING(output, 1);
			}

			memset(output, 0, needed);
			efree(output);
		} else if (salt[0]=='$' && salt[1]=='5' && salt[2]=='$') {
			const char sha256_salt_prefix[] = "$5$";
			const char sha256_rounds_prefix[] = "rounds=";
			char *output;
			int needed = (sizeof(sha256_salt_prefix) - 1
						+ sizeof(sha256_rounds_prefix) + 9 + 1
						+ salt_in_len + 1 + 43 + 1);
			output = emalloc(needed);
			salt[salt_in_len] = '\0';

			crypt_res = php_sha256_crypt_r(str, salt, output, needed);
			if (!crypt_res) {
				if (salt[0]=='*' && salt[1]=='0') {
					RETVAL_STRING("*1", 1);
				} else {
					RETVAL_STRING("*0", 1);
				}
			} else {
				RETVAL_STRING(output, 1);
			}

			memset(output, 0, needed);
			efree(output);
		} else if (
				salt[0] == '$' &&
				salt[1] == '2' &&
				salt[2] >= 'a' && salt[2] <= 'z' &&
				salt[3] == '$' &&
				salt[4] >= '0' && salt[4] <= '3' &&
				salt[5] >= '0' && salt[5] <= '9' &&
				salt[6] == '$') {
			char output[PHP_MAX_SALT_LEN + 1];

			memset(output, 0, PHP_MAX_SALT_LEN + 1);

			crypt_res = php_crypt_blowfish_rn(str, salt, output, sizeof(output));
			if (!crypt_res) {
				if (salt[0]=='*' && salt[1]=='0') {
					RETVAL_STRING("*1", 1);
				} else {
					RETVAL_STRING("*0", 1);
				}
			} else {
				RETVAL_STRING(output, 1);
			}

			memset(output, 0, PHP_MAX_SALT_LEN + 1);
		} else {
			memset(&buffer, 0, sizeof(buffer));
			_crypt_extended_init_r();

			crypt_res = _crypt_extended_r(str, salt, &buffer);
			if (!crypt_res) {
				if (salt[0]=='*' && salt[1]=='0') {
					RETURN_STRING("*1", 1);
				} else {
					RETURN_STRING("*0", 1);
				}
			} else {
				RETURN_STRING(crypt_res, 1);
			}
		}
	}
#else

# if defined(HAVE_CRYPT_R) && (defined(_REENTRANT) || defined(_THREAD_SAFE))
	{
#  if defined(CRYPT_R_STRUCT_CRYPT_DATA)
		struct crypt_data buffer;
		memset(&buffer, 0, sizeof(buffer));
#  elif defined(CRYPT_R_CRYPTD)
		CRYPTD buffer;
#  else
#    error Data struct used by crypt_r() is unknown. Please report.
#  endif
		crypt_res = crypt_r(str, salt, &buffer);
		if (!crypt_res) {
				if (salt[0]=='*' && salt[1]=='0') {
					RETURN_STRING("*1", 1);
				} else {
					RETURN_STRING("*0", 1);
				}
		} else {
			RETURN_STRING(crypt_res, 1);
		}
	}
# endif
#endif
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
