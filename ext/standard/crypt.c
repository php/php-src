/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Bakken <ssb@gaurdian.no>                               |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#include <stdlib.h>

#include "php.h"

#if HAVE_CRYPT

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_CRYPT_H
#include <crypt.h>
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
extern char *crypt(char *__key, char *__salt);
#endif

#include "php_lcg.h"
#include "php_crypt.h"
#include "php_rand.h"

/* 
   The capabilities of the crypt() function is determined by the test programs
   run by configure from aclocal.m4.  They will set PHP_STD_DES_CRYPT,
   PHP_EXT_DES_CRYPT, PHP_MD5_CRYPT and PHP_BLOWFISH_CRYPT as appropriate 
   for the target platform
*/
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

 /*
  * If the configure-time checks fail, we provide DES.
  * XXX: This is a hack. Fix the real problem
  */

#ifndef PHP_MAX_SALT_LEN
#define PHP_MAX_SALT_LEN 2
#undef PHP_STD_DES_CRYPT
#define PHP_STD_DES_CRYPT 1
#endif


#define PHP_CRYPT_RAND php_rand(TSRMLS_C)

static int php_crypt_rand_seeded=0;

PHP_MINIT_FUNCTION(crypt)
{
	REGISTER_LONG_CONSTANT("CRYPT_SALT_LENGTH", PHP_MAX_SALT_LEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_STD_DES", PHP_STD_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_EXT_DES", PHP_EXT_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_MD5", PHP_MD5_CRYPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_BLOWFISH", PHP_BLOWFISH_CRYPT, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}


PHP_RINIT_FUNCTION(crypt)
{
	if(!php_crypt_rand_seeded) {
		php_srand(time(0) * getpid() * (unsigned long) (php_combined_lcg(TSRMLS_C) * 10000.0) TSRMLS_CC);
		php_crypt_rand_seeded=1;
	} 
	return SUCCESS;
}


static unsigned char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void php_to64(char *s, long v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v&0x3f]; 		
		v >>= 6;
	} 
} 

/* {{{ proto string crypt(string str [, string salt])
   Encrypt a string */
PHP_FUNCTION(crypt)
{
	char salt[PHP_MAX_SALT_LEN+1];
	char *str, *salt_in = NULL;
	int str_len, salt_in_len;

	salt[0]=salt[PHP_MAX_SALT_LEN]='\0';
	/* This will produce suitable results if people depend on DES-encryption
	   available (passing always 2-character salt). At least for glibc6.1 */
	memset(&salt[1], '$', PHP_MAX_SALT_LEN-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &str, &str_len,
							  &salt_in, &salt_in_len) == FAILURE) {
		return;
	}

	if (salt_in) {
		memcpy(salt, salt_in, MIN(PHP_MAX_SALT_LEN, salt_in_len));
	}

	/* The automatic salt generation only covers standard DES and md5-crypt */
	if(!*salt) {
#if PHP_MD5_CRYPT
		strcpy(salt, "$1$");
		php_to64(&salt[3], PHP_CRYPT_RAND, 4);
		php_to64(&salt[7], PHP_CRYPT_RAND, 4);
		strcpy(&salt[11], "$");
#elif PHP_STD_DES_CRYPT
		php_to64(&salt[0], PHP_CRYPT_RAND, 2);
		salt[2] = '\0';
#endif
	}

	RETVAL_STRING(crypt(str, salt), 1);
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
