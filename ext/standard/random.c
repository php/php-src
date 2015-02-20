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

/*
// Copy/pasted from string.c
static char hexconvtab[] = "0123456789abcdef";

// Copy/pasted from string.c
static void php_bin_to_hex(zend_string *old, const zend_long old_len, zend_string *hex)
{
	zend_long i, j;

	hex = zend_string_alloc(old_len * 2, 0); // @todo is this right?

	for (i = j = 0; i < old_len; i++) {
		hex->val[j++] = hexconvtab[old->val[i] >> 4];
		hex->val[j++] = hexconvtab[old->val[i] & 15];
	}

	hex->val[j] = '\0';
}
*/

// Copy/pasted from mcrypt.c
static int php_random_bytes(char *bytes, zend_long size)
{
	int n = 0;

#if PHP_WIN32
	/* random/urandom equivalent on Windows */
	BYTE *win_bytes = (BYTE *) bytes;
	if (php_win32_get_random_bytes(win_bytes, (size_t) size) == FAILURE) {
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

	// @todo - Do we need to do this?
	bytes[size] = '\0';

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

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes(bytes->val, size) == FAILURE) {
		zend_string_release(bytes);
		return;
	}

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ proto string random_hex(int bytes)
Return an arbitrary length of pseudo-random bytes as hexadecimal string */
PHP_FUNCTION(random_hex)
{
	/*
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

	if (php_random_bytes(bytes, size) == FAILURE) {
		return;
	}

	int hex_size = size * 2;

	php_bin_to_hex(bytes, hex_size, hex);

	zend_string_release(bytes);
	*/

	RETURN_STR("Foo!");
}
/* }}} */

/* {{{ proto int random_int(int max)
Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long max;
	zend_long size;
	zend_long number = 0;
	zend_string *bytes;
	size_t i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &max) == FAILURE) {
		return;
	}

	if (max >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Cannot use max greater than %d", INT_MAX);
		RETURN_FALSE;
	}

	size = sizeof(number);

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes(bytes->val, size) == FAILURE) {
		zend_string_release(bytes);
		return;
	}

	// @todo bin-to-int: I know this is wrong but don't know how to fix
	for (i = 0; i < size; i++) {
		unsigned char c = bytes->val[i++];
		unsigned char d;

		if (c >= '0' && c <= '9') {
			d = c - '0';
		} else if (c >= 'a' && c <= 'f') {
			d = c - 'a' - 10;
		} else if (c >= 'A' && c <= 'F') {
			d = c - 'A' - 10;
		} else {
			continue;
		}

		// Binary = base-2
		number = number * 2 + d;
	}

	zend_string_release(bytes);

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
