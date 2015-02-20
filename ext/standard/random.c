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
#include <math.h>

#include "php.h"

#if PHP_WIN32
# include "win32/winutil.h"
#endif

// Big thanks to @ircmaxell for the help on this bit
union rand_long_buffer {
	char buffer[8];
	long number;
};

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
	// @todo Need to cache the fd for random_int() call within loop
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

/* {{{ proto int random_int(int maximum)
Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long maximum;
	zend_long size;
	size_t i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &maximum) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		maximum = INT_MAX;
	}

	if (maximum <= 0 || maximum > INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Cannot use maximum less than 1 or greater than %d", INT_MAX);
		RETURN_FALSE;
	}

	long range = (long) maximum; // @todo Support min?

	// Big thanks to @ircmaxell for the help on this bit
	union rand_long_buffer value;
	long result;
	int bits = (int) (log((double) range) / log(2.0)) + 1;
	int bytes = MAX(ceil(bits / 8), 1);
	long mask = (long) pow(2.0, (double) bits) - 1;

	do {
		if (php_random_bytes(&value.buffer, 8) == FAILURE) {
			return;
		}
		result = value.number & mask;
	} while (result > maximum);

	RETURN_LONG(result);
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
