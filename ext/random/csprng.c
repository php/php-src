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
   | Authors: Tim Düsterhus <timwolla@php.net>                            |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "php.h"

#include "Zend/zend_exceptions.h"
#include "Zend/zend_atomic.h"

#include "php_random.h"
#include "php_random_csprng.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef PHP_WIN32
# include "win32/time.h"
# include "win32/winutil.h"
# include <process.h>
#endif

#ifdef __linux__
# include <sys/syscall.h>
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
# if (defined(__FreeBSD__) && __FreeBSD_version > 1200000) || (defined(__DragonFly__) && __DragonFly_version >= 500700) || \
     (defined(__sun) && defined(HAVE_GETRANDOM)) || (defined(__NetBSD__) && __NetBSD_Version__ >= 1000000000) || defined(__midipix__)
#  include <sys/random.h>
# endif
#endif

#ifdef HAVE_COMMONCRYPTO_COMMONRANDOM_H
# include <CommonCrypto/CommonCryptoError.h>
# include <CommonCrypto/CommonRandom.h>
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
#endif

#ifndef PHP_WIN32
static zend_atomic_int random_fd = ZEND_ATOMIC_INT_INITIALIZER(-1);
#endif

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_random_bytes_ex(void *bytes, size_t size, char *errstr, size_t errstr_size)
{
#ifdef PHP_WIN32
	/* Defer to CryptGenRandom on Windows */
	if (php_win32_get_random_bytes(bytes, size) == FAILURE) {
		snprintf(errstr, errstr_size, "Failed to retrieve randomness from the operating system (BCryptGenRandom)");
		return FAILURE;
	}
#elif defined(HAVE_COMMONCRYPTO_COMMONRANDOM_H)
	/*
	 * Purposely prioritized upon arc4random_buf for modern macOs releases
	 * arc4random api on this platform uses `ccrng_generate` which returns
	 * a status but silented to respect the "no fail" arc4random api interface
	 * the vast majority of the time, it works fine ; but better make sure we catch failures
	 */
	if (CCRandomGenerateBytes(bytes, size) != kCCSuccess) {
		snprintf(errstr, errstr_size, "Failed to retrieve randomness from the operating system (CCRandomGenerateBytes)");
		return FAILURE;
	}
#elif defined(HAVE_ARC4RANDOM_BUF) && ((defined(__OpenBSD__) && OpenBSD >= 201405) || (defined(__NetBSD__) && __NetBSD_Version__ >= 700000001 && __NetBSD_Version__ < 1000000000) || \
  defined(__APPLE__))
	/*
	 * OpenBSD until there is a valid equivalent
	 * or NetBSD before the 10.x release
	 * falls back to arc4random_buf
	 * giving a decent output, the main benefit
	 * is being (relatively) failsafe.
	 * Older macOs releases fall also into this
	 * category for reasons explained above.
	 */
	arc4random_buf(bytes, size);
#else
	size_t read_bytes = 0;
# if (defined(__linux__) && defined(SYS_getrandom)) || (defined(__FreeBSD__) && __FreeBSD_version >= 1200000) || (defined(__DragonFly__) && __DragonFly_version >= 500700) || \
  (defined(__sun) && defined(HAVE_GETRANDOM)) || (defined(__NetBSD__) && __NetBSD_Version__ >= 1000000000) || defined(__midipix__)
	/* Linux getrandom(2) syscall or FreeBSD/DragonFlyBSD/NetBSD getrandom(2) function
	 * Being a syscall, implemented in the kernel, getrandom offers higher quality output
	 * compared to the arc4random api albeit a fallback to /dev/urandom is considered.
	 */
	while (read_bytes < size) {
		/* Below, (bytes + read_bytes)  is pointer arithmetic.

		   bytes   read_bytes  size
		     |      |           |
		    [#######=============] (we're going to write over the = region)
		             \\\\\\\\\\\\\
		              amount_to_read
		*/
		size_t amount_to_read = size - read_bytes;
		ssize_t n;

		errno = 0;
#  if defined(__linux__)
		n = syscall(SYS_getrandom, bytes + read_bytes, amount_to_read, 0);
#  else
		n = getrandom(bytes + read_bytes, amount_to_read, 0);
#  endif

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

#  if __has_feature(memory_sanitizer)
		/* MSan does not instrument manual syscall invocations. */
		__msan_unpoison(bytes + read_bytes, n);
#  endif
		read_bytes += (size_t) n;
	}
# endif
	if (read_bytes < size) {
		int    fd = zend_atomic_int_load_ex(&random_fd);
		struct stat st;

		if (fd < 0) {
			errno = 0;
			fd = open("/dev/urandom", O_RDONLY);
			if (fd < 0) {
				if (errno != 0) {
					snprintf(errstr, errstr_size, "Cannot open /dev/urandom: %s", strerror(errno));
				} else {
					snprintf(errstr, errstr_size, "Cannot open /dev/urandom");
				}
				return FAILURE;
			}

			errno = 0;
			/* Does the file exist and is it a character device? */
			if (fstat(fd, &st) != 0 ||
# ifdef S_ISNAM
					!(S_ISNAM(st.st_mode) || S_ISCHR(st.st_mode))
# else
					!S_ISCHR(st.st_mode)
# endif
			) {
				close(fd);
				if (errno != 0) {
					snprintf(errstr, errstr_size, "Error reading from /dev/urandom: %s", strerror(errno));
				} else {
					snprintf(errstr, errstr_size, "Error reading from /dev/urandom");
				}
				return FAILURE;
			}
			int expected = -1;
			if (!zend_atomic_int_compare_exchange_ex(&random_fd, &expected, fd)) {
				close(fd);
				/* expected is now the actual value of random_fd */
				fd = expected;
			}
		}

		read_bytes = 0;
		while (read_bytes < size) {
			errno = 0;
			ssize_t n = read(fd, bytes + read_bytes, size - read_bytes);

			if (n <= 0) {
				if (errno != 0) {
					snprintf(errstr, errstr_size, "Could not gather sufficient random data: %s", strerror(errno));
				} else {
					snprintf(errstr, errstr_size, "Could not gather sufficient random data");
				}
				return FAILURE;
			}

			read_bytes += (size_t) n;
		}
	}
#endif

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_random_bytes(void *bytes, size_t size, bool should_throw)
{
	char errstr[128];
	zend_result result = php_random_bytes_ex(bytes, size, errstr, sizeof(errstr));

	if (result == FAILURE && should_throw) {
		zend_throw_exception(random_ce_Random_RandomException, errstr, 0);
	}

	return result;
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_random_int(zend_long min, zend_long max, zend_long *result, bool should_throw)
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

PHPAPI void php_random_csprng_shutdown(void)
{
#ifndef PHP_WIN32
	int fd = zend_atomic_int_exchange(&random_fd, -1);
	if (fd != -1) {
		close(fd);
	}
#endif
}
