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
#include "php_random.h"

#if PHP_WIN32
# include "win32/winutil.h"
#endif

#ifdef ZTS
int random_globals_id;
#else
php_random_globals random_globals;
#endif

static void random_globals_ctor(php_random_globals *random_globals_p)
{
	random_globals_p->fd = -1;
}

static void random_globals_dtor(php_random_globals *random_globals_p)
{
	if (random_globals_p->fd > 0) {
		close(random_globals_p->fd);
		random_globals_p->fd = -1;
	}
}

/* {{{ */
PHP_MINIT_FUNCTION(random)
{
#ifdef ZTS
	ts_allocate_id(&random_globals_id, sizeof(php_random_globals), (ts_allocate_ctor)random_globals_ctor, (ts_allocate_dtor)random_globals_dtor);
#else
	random_globals_ctor(&random_globals);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ */
PHP_MSHUTDOWN_FUNCTION(random)
{
#ifndef ZTS
	random_globals_dtor(&random_globals);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_random_bytes(void *bytes, size_t size)
{
#if PHP_WIN32
	/* Defer to CryptGenRandom on Windows */
	if (php_win32_get_random_bytes(bytes, size) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Could not gather sufficient random data");
		return FAILURE;
	}
#elif HAVE_DECL_ARC4RANDOM_BUF
	arc4random_buf(bytes, size);
#else
	int    fd = RANDOM_G(fd);
	size_t read_bytes = 0;

	if (fd < 0) {
#if HAVE_DEV_ARANDOM
		fd = open("/dev/arandom", O_RDONLY);
#elif HAVE_DEV_URANDOM
		fd = open("/dev/urandom", O_RDONLY);
#endif
		if (fd < 0) {
			php_error_docref(NULL, E_WARNING, "Cannot open source device");
			return FAILURE;
		}

		RANDOM_G(fd) = fd;
	}

	while (read_bytes < size) {
		ssize_t n = read(fd, bytes + read_bytes, size - read_bytes);
		if (n < 0) {
			break;
		}
		read_bytes += n;
	}

	if (read_bytes < size) {
		php_error_docref(NULL, E_WARNING, "Could not gather sufficient random data");
		return FAILURE;
	}
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ proto string random_bytes(int length)
Return an arbitrary length of pseudo-random bytes as binary string */
PHP_FUNCTION(random_bytes)
{
	zend_long size;
	zend_string *bytes;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if (size < 1) {
		php_error_docref(NULL, E_WARNING, "Length must be greater than 0");
		RETURN_FALSE;
	}

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes(bytes->val, size) == FAILURE) {
		zend_string_release(bytes);
		RETURN_FALSE;
	}

	bytes->val[size] = '\0';

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ proto int random_int(int min, int max)
Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long min;
	zend_long max;
	zend_ulong umax;
	zend_ulong result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &min, &max) == FAILURE) {
		return;
	}

	if (min >= max) {
		php_error_docref(NULL, E_WARNING, "Minimum value must be less than the maximum value");
		RETURN_FALSE;
	}

	umax = max - min;

	if (php_random_bytes(&result, sizeof(result)) == FAILURE) {
		RETURN_FALSE;
	}

	/* Special case where no modulus is required */
	if (umax == ZEND_ULONG_MAX) {
		RETURN_LONG((zend_long)result);
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) != 0) {
		/* Ceiling under which ZEND_LONG_MAX % max == 0 */
		zend_ulong limit = ZEND_ULONG_MAX - (ZEND_ULONG_MAX % umax) - 1;
	
		/* Discard numbers over the limit to avoid modulo bias */
		while (result > limit) {
			if (php_random_bytes(&result, sizeof(result)) == FAILURE) {
				RETURN_FALSE;
			}
		}
	}

	RETURN_LONG((zend_long)((result % umax) + min));
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
