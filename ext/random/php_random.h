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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Sammy Kaye Powers <me@sammyk.me>                            |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
*/
#ifndef PHP_RANDOM_H
# define PHP_RANDOM_H

/* System Rand functions */
# ifndef RAND_MAX
#  define RAND_MAX PHP_MT_RAND_MAX
# endif

#define PHP_RAND_MAX PHP_MT_RAND_MAX

/*
 * A bit of tricky math here.  We want to avoid using a modulus because
 * that simply tosses the high-order bits and might skew the distribution
 * of random values over the range.  Instead we map the range directly.
 *
 * We need to map the range from 0...M evenly to the range a...b
 * Let n = the random number and n' = the mapped random number
 *
 * Then we have: n' = a + n(b-a)/M
 *
 * We have a problem here in that only n==M will get mapped to b which
 # means the chances of getting b is much much less than getting any of
 # the other values in the range.  We can fix this by increasing our range
 # artificially and using:
 #
 #               n' = a + n(b-a+1)/M
 *
 # Now we only have a problem if n==M which would cause us to produce a
 # number of b+1 which would be bad.  So we bump M up by one to make sure
 # this will never happen, and the final algorithm looks like this:
 #
 #               n' = a + n(b-a+1)/(M+1)
 *
 * -RL
 */
# define RAND_RANGE_BADSCALING(__n, __min, __max, __tmax) \
	(__n) = (__min) + (zend_long) ((double) ( (double) (__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

# ifdef PHP_WIN32
#  define GENERATE_SEED() (((zend_long) (time(0) * GetCurrentProcessId())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
# else
#  define GENERATE_SEED() (((zend_long) (time(0) * getpid())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
# endif

# define PHP_MT_RAND_MAX ((zend_long) (0x7FFFFFFF)) /* (1<<31) - 1 */

# define MT_RAND_MT19937 0
# define MT_RAND_PHP 1

# define MT_N (624)

# define php_random_bytes_throw(b, s) php_random_bytes((b), (s), 1)
# define php_random_bytes_silent(b, s) php_random_bytes((b), (s), 0)

# define php_random_int_throw(min, max, result) \
	php_random_int((min), (max), (result), 1)
# define php_random_int_silent(min, max, result) \
	php_random_int((min), (max), (result), 0)

PHPAPI double php_combined_lcg(void);

PHPAPI void php_srand(zend_long seed);
PHPAPI zend_long php_rand(void);

PHPAPI void php_mt_srand(uint32_t seed);
PHPAPI uint32_t php_mt_rand(void);
PHPAPI zend_long php_mt_rand_range(zend_long min, zend_long max);
PHPAPI zend_long php_mt_rand_common(zend_long min, zend_long max);

PHPAPI int php_random_bytes(void *bytes, size_t size, bool should_throw);
PHPAPI int php_random_int(zend_long min, zend_long max, zend_long *result, bool should_throw);

extern zend_module_entry random_module_entry;
# define phpext_random_ptr &random_module_entry

PHP_MINIT_FUNCTION(random);
PHP_MSHUTDOWN_FUNCTION(random);
PHP_RINIT_FUNCTION(random);

ZEND_BEGIN_MODULE_GLOBALS(random)
	int32_t lcg_s1;
	int32_t lcg_s2;
	int lcg_seeded;
	uint32_t mt_rand_state[MT_N+1];	/* state vector + 1 extra to not violate ANSI C */
	uint32_t *mt_rand_next;			/* next random value is computed from here */
	int mt_rand_left;				/* can *next++ this many times before reloading */
	bool mt_rand_is_seeded;			/* Whether mt_rand() has been seeded */
	zend_long mt_rand_mode;
	int random_fd;
ZEND_END_MODULE_GLOBALS(random)

# define RANDOM_G(v)	ZEND_MODULE_GLOBALS_ACCESSOR(random, v)

#endif	/* PHP_RANDOM_H */
