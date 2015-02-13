/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sammy Kaye Powers <me@sammyk.me>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <stdlib.h>

#include "php.h"

// Copy/pasted from string.c
static char hexconvtab[] = "0123456789abcdef";

// Copy/pasted from string.c
static zend_string *php_bin_to_hex(const unsigned char *old, const size_t oldlen)
{
	zend_string *result;
	size_t i, j;

	result = zend_string_safe_alloc(oldlen, 2 * sizeof(char), 0, 0);

	for (i = j = 0; i < oldlen; i++) {
		result->val[j++] = hexconvtab[old[i] >> 4];
		result->val[j++] = hexconvtab[old[i] & 15];
	}
	result->val[j] = '\0';

	return result;
}

static int *php_random_bytes(zend_string *bytes, size_t length)
{
	FILE *fp;

#ifdef PHP_WIN32
	// @todo Need to add Windows support
	fp = NULL;
#else
	fp = fopen("/dev/urandom" , "rb");
#endif

	if (fp == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to open /dev/urandom");
		return FAILURE;
	}

	// @todo I think this has to be char & int!
	if (fgets(bytes, 10, fp) == NULL) {
		fclose(fp);
		php_error_docref(NULL, E_WARNING, "Unable to read from /dev/urandom");
		return FAILURE;
	}
	fclose(fp);

	return bytes;
}

/* {{{ proto string random_bytes(int bytes)
Return an arbitrary length of pseudo-random bytes as binary string */
PHP_FUNCTION(random_bytes)
{
	size_t length;
	zend_string *bytes;

	int argc = ZEND_NUM_ARGS();

	if (argc != 0) {
		if (zend_parse_parameters(argc, "l", &length) == FAILURE) {
			return;
		} else if (length <= 0) {
			php_error_docref(NULL, E_WARNING, "length(" ZEND_LONG_FMT ") must be greater than 0", length);
			RETURN_FALSE;
		}
	}

	if (php_random_bytes(bytes, length) == FAILURE) {
		return;
	}

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ proto string random_hex(int bytes)
Return an arbitrary length of pseudo-random bytes as hexadecimal string */
PHP_FUNCTION(random_hex)
{
	size_t length;
	zend_string *bytes;
	zend_string *hex;

	int argc = ZEND_NUM_ARGS();

	if (argc != 0) {
		if (zend_parse_parameters(argc, "l", &length) == FAILURE) {
			return;
		} else if (length <= 0) {
			php_error_docref(NULL, E_WARNING, "length(" ZEND_LONG_FMT ") must be greater than 0", length);
			RETURN_FALSE;
		}
	}

	if (php_random_bytes(bytes, length) == FAILURE) {
		return;
	}

	hex = php_bin_to_hex(bytes, length);

	RETURN_STR(hex);
}
/* }}} */

/* {{{ proto int random_int(int min, int max)
Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long min;
	zend_long max;
	zend_long number;
	int argc = ZEND_NUM_ARGS();

	if (argc != 0) {
		if (zend_parse_parameters(argc, "ll", &min, &max) == FAILURE) {
			return;
		} else if (max < min) {
			php_error_docref(NULL, E_WARNING, "max(" ZEND_LONG_FMT ") is smaller than min(" ZEND_LONG_FMT ")", max, min);
			RETURN_FALSE;
		}
	}

	number = min + max;

	RETURN_LONG(number);
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
