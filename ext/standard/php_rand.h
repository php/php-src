/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |                                                                      |
   | Based on code from: Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* Layout implementation random functions
 *
 * The PHPAPI contains these functions:
 * - long php_rand()
 * - long php_rand_range(long min, long max)
 * - void php_srand()
 * - long php_getrandmax()
 *
 * Note that it is not possible to choose the algoritm. This is done to
 * give the user the possibility to control all randomness by means of
 * srand()/php.ini in a portable and consistent way.
 *
 * rand.c: (the only rand*.c file with PHP_API and PHP_FUNCTION functions)
 *
 * - PHP_FUNCTION([mt_]srand)
 *   +-> void php_srand(void)
 *   +-> void php_srand2(long seed, int alg)
 *       +-> (rand_sys.c) long php_rand_sys()
 *       +-> (rand_mt.c ) long php_rand_mt()
 *
 * - PHP_FUNCTION([mt_]rand)
 *   +-> long php_rand()
 *       +-> (rand_sys.c) long php_rand_sys()
 *       +-> (rand_mt.c ) long php_rand_mt()
 *   +-> long php_rand_range(long min, long max)
 *       +-> calls php_rand()
 *       
 * - PHP_FUNCTION([mt_]getrandmax)
 *   +-> PHPAPI long php_randmax(void)
 *       +-> (rand_sys.c) long php_randmax_sys()
 *       +-> (rand_mt.c ) long php_randmax_mt()
 *   
 *   --Jeroen
 */

#ifndef PHP_RAND_H
#define	PHP_RAND_H

#include <stdlib.h>

#ifndef RAND_MAX
#define RAND_MAX (1<<15)
#endif

#if HAVE_LRAND48
#define php_randmax_sys() 2147483647
#else
#define php_randmax_sys() RAND_MAX
#endif

/*
 * Melo: it could be 2^^32 but we only use 2^^31 to maintain
 * compatibility with the previous php_rand
 */
#define php_randmax_mt() ((long)(0x7FFFFFFF)) /* 2^^31 - 1 */

PHP_FUNCTION(srand);
PHP_FUNCTION(rand);
PHP_FUNCTION(getrandmax);
PHP_FUNCTION(mt_srand);
PHP_FUNCTION(mt_rand);
PHP_FUNCTION(mt_getrandmax);

PHPAPI long php_rand(void);
PHPAPI long php_rand_range(long min, long max);
PHPAPI long php_randmax(void);
long php_rand_mt(void);
void php_srand_mt(long seed TSRMLS_DC);

/* Define rand Function wrapper */
#ifdef HAVE_RANDOM
#define php_rand_sys() random()
#else
#ifdef HAVE_LRAND48
#define php_rand_sys() lrand48()
#else
#define php_rand_sys() rand()
#endif
#endif

/* Define srand Function wrapper */
#ifdef HAVE_SRANDOM
#define php_srand_sys(seed) srandom((unsigned int)seed)
#else
#ifdef HAVE_SRAND48
#define php_srand_sys(seed) srand48((long)seed)
#else
#define php_srand_sys(seed) srand((unsigned int)seed)
#endif
#endif

/* Define random generator constants */
#define RAND_SYS 1
#define RAND_MT  2

/* BC */
#define PHP_RAND_MAX php_randmax()

#endif	/* PHP_RAND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: sw=4 ts=4 tw=78
 */
