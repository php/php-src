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
   |          Jeroen van Wolffelaar <jeroen@php.net>                      |
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

/* TODO:
 * - make constants available to PHP-user
 * - MINFO section about which random number generators are available
 * - Nuke randmax by enhancing PHP_RAND_RANGE to work well in the case of a
 *   greater request than the real (internal) randmax is
 * - Implement LCG
 * - Implement a real-random source? (via internet, and/or /dev/urandom?)
 * - Can lrand48 be thread-safe?
 * - Is random() useful sometimes?
 * - Which system algorithms are available, maybe name them after real
 *   algorithm by compile-time detection?
 * - Get this to compile :-)
 */
#ifndef PHP_RAND_H
#define	PHP_RAND_H

#include <stdlib.h>

/* FIXME: that '_php_randgen_entry' needed, or not? */
typedef struct _php_randgen_entry {
	void (*srand)(long seed);
	long (*rand)(void);
	long randmax;
	char *ini_str;
} php_randgen_entry;

/* an ARRAY of POINTERS, not vice versa */
extern php_randgen_entry *php_randgen_entries[];

#define PHP_RANDGEN_ENTRY(which, nsrand, nrand, nrandmax, nini_str) { \
	php_randgen_entries[which] = emalloc(sizeof(php_randgen_entry)); \
	php_randgen_entries[which]->srand	= nsrand;	\
	php_randgen_entries[which]->rand	= nrand;	\
	php_randgen_entries[which]->randmax	= nrandmax;	\
	php_randgen_entries[which]->ini_str	= nini_str;	\
}

/* Define random generator constants */
#define PHP_RAND_SYS		0
#define PHP_RAND_LRAND48	1
#define PHP_RAND_MT			2
#define PHP_RAND_LCG		3

#define PHP_RAND_DEFAULT	PHP_RAND_MT

/* how many there are */
#define PHP_RAND_NUMRANDS	4

/* Proto's */
PHP_FUNCTION(srand);
PHP_FUNCTION(rand);
PHP_FUNCTION(getrandmax);
PHP_FUNCTION(mt_srand);
PHP_FUNCTION(mt_rand);
PHP_FUNCTION(mt_getrandmax);

PHPAPI long   php_rand(void);
PHPAPI long   php_rand_range(long min, long max);
PHPAPI double php_drand(void);
PHPAPI long   php_randmax(void);

#endif	/* PHP_RAND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: sw=4 ts=4 tw=78
 */
