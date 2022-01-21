/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "php.h"
#include "zend_exceptions.h"
#include "php_random.h"

#ifdef PHP_WIN32
# include "win32/winutil.h"
#endif
#ifdef __linux__
# include <sys/syscall.h>
#endif
#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)
# include <sys/param.h>
# if __FreeBSD__ && __FreeBSD_version > 1200000
#  include <sys/random.h>
# endif
#endif
#if HAVE_COMMONCRYPTO_COMMONRANDOM_H
# include <CommonCrypto/CommonCryptoError.h>
# include <CommonCrypto/CommonRandom.h>
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
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

/* {{{ php_random_bytes */
PHPAPI int php_random_bytes(void *bytes, size_t size, zend_bool should_throw)
{
#ifdef PHP_WIN32
	/* Defer to CryptGenRandom on Windows */
	if (php_win32_get_random_bytes(bytes, size) == FAILURE) {
		if (should_throw) {
			zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
		}
		return FAILURE;
	}
#elif HAVE_COMMONCRYPTO_COMMONRANDOM_H
	/*
	 * Purposely prioritized upon arc4random_buf for modern macOs releases
	 * arc4random api on this platform uses `ccrng_generate` which returns
	 * a status but silented to respect the "no fail" arc4random api interface
	 * the vast majority of the time, it works fine ; but better make sure we catch failures
	 */
	if (CCRandomGenerateBytes(bytes, size) != kCCSuccess) {
		if (should_throw) {
			zend_throw_exception(zend_ce_exception, "Error generating bytes", 0);
		}
		return FAILURE;
	}
#elif HAVE_DECL_ARC4RANDOM_BUF && ((defined(__OpenBSD__) && OpenBSD >= 201405) || (defined(__NetBSD__) && __NetBSD_Version__ >= 700000001) || defined(__APPLE__))
	arc4random_buf(bytes, size);
#else
	size_t read_bytes = 0;
	ssize_t n;
#if (defined(__linux__) && defined(SYS_getrandom)) || (defined(__FreeBSD__) && __FreeBSD_version >= 1200000)
	/* Linux getrandom(2) syscall or FreeBSD getrandom(2) function*/
	/* Keep reading until we get enough entropy */
	while (read_bytes < size) {
		/* Below, (bytes + read_bytes)  is pointer arithmetic.

		   bytes   read_bytes  size
		     |      |           |
		    [#######=============] (we're going to write over the = region)
		             \\\\\\\\\\\\\
		              amount_to_read

		*/
		size_t amount_to_read = size - read_bytes;
#if defined(__linux__)
		n = syscall(SYS_getrandom, bytes + read_bytes, amount_to_read, 0);
#else
		n = getrandom(bytes + read_bytes, amount_to_read, 0);
#endif

		if (n == -1) {
			if (errno == ENOSYS) {
				/* This can happen if PHP was compiled against a newer kernel where getrandom()
				 * is available, but then runs on an older kernel without getrandom(). If this
				 * happens we simply fall back to reading from /dev/urandom. */
				ZEND_ASSERT(read_bytes == 0);
				break;
			} else if (errno == EINTR || errno == EAGAIN) {
				/* Try again */
				continue;
			} else {
			    /* If the syscall fails, fall back to reading from /dev/urandom */
				break;
			}
		}

#if __has_feature(memory_sanitizer)
		/* MSan does not instrument manual syscall invocations. */
		__msan_unpoison(bytes + read_bytes, n);
#endif
		read_bytes += (size_t) n;
	}
#endif
	if (read_bytes < size) {
		int    fd = RANDOM_G(fd);
		struct stat st;

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

		for (read_bytes = 0; read_bytes < size; read_bytes += (size_t) n) {
			n = read(fd, bytes + read_bytes, size - read_bytes);
			if (n <= 0) {
				break;
			}
		}

		if (read_bytes < size) {
			if (should_throw) {
				zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
			}
			return FAILURE;
		}
	}
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ Return an arbitrary length of pseudo-random bytes as binary string */
PHP_FUNCTION(random_bytes)
{
	zend_long size;
	zend_string *bytes;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes_throw(ZSTR_VAL(bytes), size) == FAILURE) {
		zend_string_release_ex(bytes, 0);
		RETURN_THROWS();
	}

	ZSTR_VAL(bytes)[size] = '\0';

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ */
PHPAPI int php_random_int(zend_long min, zend_long max, zend_long *result, zend_bool should_throw)
{
	zend_ulong umax;
	zend_ulong trial;

	if (min == max) {
		*result = min;
		return SUCCESS;
	}

	umax = (zend_ulong) max - (zend_ulong) min;

	if (php_random_bytes(&trial, sizeof(trial), should_throw) == FAILURE) {
		return FAILURE;
	}

	/* Special case where no modulus is required */
	if (umax == ZEND_ULONG_MAX) {
		*result = (zend_long)trial;
		return SUCCESS;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) != 0) {
		/* Ceiling under which ZEND_LONG_MAX % max == 0 */
		zend_ulong limit = ZEND_ULONG_MAX - (ZEND_ULONG_MAX % umax) - 1;

		/* Discard numbers over the limit to avoid modulo bias */
		while (trial > limit) {
			if (php_random_bytes(&trial, sizeof(trial), should_throw) == FAILURE) {
				return FAILURE;
			}
		}
	}

	*result = (zend_long)((trial % umax) + min);
	return SUCCESS;
}
/* }}} */

/* {{{ Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long min;
	zend_long max;
	zend_long result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (min > max) {
		zend_argument_value_error(1, "must be less than or equal to argument #2 ($max)");
		RETURN_THROWS();
	}

	if (php_random_int_throw(min, max, &result) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(result);
}
/* }}} */
