/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
#include "zend_exceptions.h"
#include "php_random.h"

#if PHP_WIN32
# include "win32/winutil.h"
#endif
#ifdef __linux__
# include <sys/syscall.h>
#endif
#if defined(__OpenBSD__) || defined(__NetBSD__)
# include <sys/param.h>
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

PHPAPI int php_random_bytes(void *bytes, size_t size, zend_bool should_throw)
{
#if PHP_WIN32
	/* Defer to CryptGenRandom on Windows */
	if (php_win32_get_random_bytes(bytes, size) == FAILURE) {
		if (should_throw) {
			zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
		}
		return FAILURE;
	}
#elif HAVE_DECL_ARC4RANDOM_BUF && ((defined(__OpenBSD__) && OpenBSD >= 201405) || (defined(__NetBSD__) && __NetBSD_Version__ >= 700000001))
	arc4random_buf(bytes, size);
#elif HAVE_DECL_GETRANDOM
	/* Linux getrandom(2) syscall */
	size_t read_bytes = 0;
	size_t amount_to_read = 0;
	ssize_t n;

	/* Keep reading until we get enough entropy */
	do {
		/* Below, (bytes + read_bytes)  is pointer arithmetic.

		   bytes   read_bytes  size
		     |      |           |
		    [#######=============] (we're going to write over the = region)
		             \\\\\\\\\\\\\
		              amount_to_read

		*/
		amount_to_read = size - read_bytes;
		n = syscall(SYS_getrandom, bytes + read_bytes, amount_to_read, 0);

		if (n == -1) {
			if (errno == EINTR || errno == EAGAIN) {
				/* Try again */
				continue;
			}
			/*
				If the syscall fails, we are doomed. The loop that calls
				php_random_bytes should be terminated by the exception instead
				of proceeding to demand more entropy.
			*/
			if (should_throw) {
				zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", errno);
			}
			return FAILURE;
		}

		read_bytes += (size_t) n;
	} while (read_bytes < size);
#else
	int    fd = RANDOM_G(fd);
	struct stat st;
	size_t read_bytes = 0;
	ssize_t n;

	if (fd < 0) {
#if HAVE_DEV_URANDOM
		fd = open("/dev/urandom", O_RDONLY);
#endif
		if (fd < 0) {
			if (should_throw) {
				zend_throw_exception(zend_ce_exception, "Cannot open source device", 0);
			}
			return FAILURE;
		}
		/* Does the file exist and is it a character device? */
		if (fstat(fd, &st) != 0 || 
# ifdef S_ISNAM
                !(S_ISNAM(st.st_mode) || S_ISCHR(st.st_mode))
# else
                !S_ISCHR(st.st_mode)
# endif
		) {
			close(fd);
			if (should_throw) {
				zend_throw_exception(zend_ce_exception, "Error reading from source device", 0);
			}
			return FAILURE;
		}
		RANDOM_G(fd) = fd;
	}

	while (read_bytes < size) {
		n = read(fd, bytes + read_bytes, size - read_bytes);
		if (n <= 0) {
			break;
		}
		read_bytes += n;
	}

	if (read_bytes < size) {
		if (should_throw) {
			zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
		}
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

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if (size < 1) {
		zend_throw_exception(zend_ce_error, "Length must be greater than 0", 0);
		return;
	}

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes_throw(ZSTR_VAL(bytes), size) == FAILURE) {
		zend_string_release(bytes);
		return;
	}

	ZSTR_VAL(bytes)[size] = '\0';

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

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "ll", &min, &max) == FAILURE) {
		return;
	}

	if (min > max) {
		zend_throw_exception(zend_ce_error, "Minimum value must be less than or equal to the maximum value", 0);
		return;
	}

	if (min == max) {
		RETURN_LONG(min);
	}

	umax = max - min;

	if (php_random_bytes_throw(&result, sizeof(result)) == FAILURE) {
		return;
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
			if (php_random_bytes_throw(&result, sizeof(result)) == FAILURE) {
				return;
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
