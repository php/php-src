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
   |          Jeroen van Wolffelaar <jeroen@php.net>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdlib.h>

#include "php.h"
#include "php_rand.h"

#include "basic_functions.h"


/* rand() & co (thread safe this time!): */
static void _php_srand_sys(long seed) 
{
	BG(rand_sys_seed) = (unsigned int) seed;
}

static long _php_rand_sys(void)
{
	return (long) rand_r(&BG(rand_sys_seed));
}

PHP_MINIT_FUNCTION(rand_sys)
{
#ifndef RAND_MAX
#define RAND_MAX (1<<15)
#endif
	PHP_RANDGEN_ENTRY(PHP_RAND_SYS,
		_php_srand_sys,	/* void srand(long seed)	*/
		_php_rand_sys,	/* long rand(void)			*/
		(long)RAND_MAX,	/* long randmax				*/
		"system"		/* char *ini_str			*/
	);

	/* random() is left away, no manual page on my system, no bigger range than
	 * rand() 
	 *    --jeroen
	 */

	/* lrand48 (_not_ TS) */
#if HAVE_LRAND48
	PHP_RANDGEN_ENTRY(PHP_RAND_LRAND48,
		srand48,		/* void srand(long seed)	*/
		lrand48,		/* long rand(void)			*/
		2147483647L,	/* long randmax				*/
		"lrand48"		/* char *ini_str			*/
	);
#else
	php_randgen_entries[PHP_RAND_LRAND48] = NULL;
#endif

}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim: sw=4 ts=4 tw=78
 */
