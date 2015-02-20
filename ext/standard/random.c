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
#include <sys/stat.h>
#include <fcntl.h>

#include "php.h"

#if PHP_WIN32
# include "win32/winutil.h"
#endif

// Copy/pasted from string.c
static char hexconvtab[] = "0123456789abcdef";

// Copy/pasted from string.c
static void php_bin_to_hex(char *old, const zend_long old_len, char *hex)
{
	zend_long i, j;

	// @todo I don't think this is doing it right
	for (i = j = 0; i < old_len; i++) {
		hex[j++] = hexconvtab[old[i] >> 4];
		hex[j++] = hexconvtab[old[i] & 15];
	}

	hex[j] = '\0';
}

// Copy/pasted from mcrypt.c
static int php_random_bytes(zend_string *bytes, zend_long size)
{
	int n = 0;

#if PHP_WIN32
	/* random/urandom equivalent on Windows */
	BYTE *win_bytes = (BYTE *) bytes;
	if (php_win32_get_random_bytes(win_bytes, (size_t) size) == FAILURE){
		php_error_docref(NULL, E_WARNING, "Could not gather sufficient random data");
		return FAILURE;
	}
	n = (int)size;
#else
	int    fd;
	size_t read_bytes = 0;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		php_error_docref(NULL, E_WARNING, "Cannot open source device");
		return FAILURE;
	}
	while (read_bytes < size) {
		n = read(fd, bytes + read_bytes, size - read_bytes);
		if (n < 0) {
			break;
		}
		read_bytes += n;
	}
	n = read_bytes;
	close(fd);
	if (n < size) {
		php_error_docref(NULL, E_WARNING, "Could not gather sufficient random data");
		return FAILURE;
	}
#endif

	return SUCCESS;
}

/* {{{ proto string random_bytes(int bytes)
Return an arbitrary length of pseudo-random bytes as binary string */
PHP_FUNCTION(random_bytes)
{
	zend_long size;
	zend_string *bytes;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if (size <= 0 || size >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Cannot genrate a random string with a size of less than 1 or greater than %d", INT_MAX);
		RETURN_FALSE;
	}

	bytes = zend_string_alloc(size + 1, 0);

	if (php_random_bytes(bytes, size) == FAILURE) {
		zend_string_release(bytes);
		return;
	}

	RETVAL_STRINGL(bytes, size);

	zend_string_release(bytes);
}
/* }}} */

/* {{{ proto string random_hex(int bytes)
Return an arbitrary length of pseudo-random bytes as hexadecimal string */
PHP_FUNCTION(random_hex)
{
	zend_long size;
	zend_string *bytes;
	zend_string *hex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if (size <= 0 || size >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Cannot genrate a random string with a size of less than 1 or greater than %d", INT_MAX);
		RETURN_FALSE;
	}

	// @todo should we half the size for hex? How for odd num of chars?
	bytes = zend_string_alloc(size + 1, 0);

	if (php_random_bytes(bytes, size) == FAILURE) {
		zend_string_release(bytes);
		return;
	}

	int hex_size = size * 2;
	hex = zend_string_alloc(hex_size + 1, 0);
	php_bin_to_hex(bytes, hex_size, hex);

	RETVAL_STRINGL(hex, hex_size);

	zend_string_release(bytes);
	zend_string_release(hex);
}
/* }}} */

/* {{{ proto int random_int(int min, int max)
Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long min;
	zend_long max;
	zend_long number;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &min, &max) == FAILURE) {
		return;
	}

	if (min >= INT_MAX || max >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Cannot use range greater than %d", INT_MAX);
		RETURN_FALSE;
	}

	if (max < min) {
		php_error_docref(NULL, E_WARNING, "Max value (%d) is less than min value (%d)", max, min);
		RETURN_FALSE;
	}

	// @todo Insert bin-to-int stuff here
	number = min + max * 100;

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
