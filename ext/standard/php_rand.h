/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef PHP_RAND_H
#define	PHP_RAND_H

#include <stdlib.h>
#include "basic_functions.h"

/* System Rand functions */
#ifndef RAND_MAX
#define RAND_MAX (1<<15)
#endif

#if HAVE_LRAND48
#define PHP_RAND_MAX 2147483647
#else
#define PHP_RAND_MAX RAND_MAX
#endif

#define RAND_RANGE(__n, __min, __max, __tmax) \
    (__n) = (__min) + (long) ((double) ((__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

/* MT Rand */
#define PHP_MT_RAND_MAX ((long) (0x7FFFFFFF)) /* (1<<31) - 1 */ 

PHPAPI void php_srand(long seed TSRMLS_DC);
PHPAPI long php_rand(TSRMLS_D);
PHPAPI void php_mt_srand(php_uint32 seed TSRMLS_DC);
PHPAPI php_uint32 php_mt_rand(TSRMLS_D);

#endif	/* PHP_RAND_H */
