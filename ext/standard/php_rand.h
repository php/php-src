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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_RAND_H
#define	PHP_RAND_H

#include "php_lcg.h"
#include "php_mt_rand.h"

/* System Rand functions */
#ifndef RAND_MAX
#define RAND_MAX PHP_MT_RAND_MAX
#endif

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
#define RAND_RANGE_BADSCALING(__n, __min, __max, __tmax) \
	(__n) = (__min) + (zend_long) ((double) ( (double) (__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

#ifdef PHP_WIN32
#define GENERATE_SEED() (((zend_long) (time(0) * GetCurrentProcessId())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
#else
#define GENERATE_SEED() (((zend_long) (time(0) * getpid())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
#endif

PHPAPI void php_srand(zend_long seed);
PHPAPI zend_long php_rand(void);

#endif	/* PHP_RAND_H */
