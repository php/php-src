/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */


#ifndef PHP_REENTRANCY_H
#define PHP_REENTRANCY_H

#include "php.h"

#include <time.h>

/* currently, PHP does not check for these functions, but assumes
   that they are available on all systems. */

#define HAVE_LOCALTIME 1
#define HAVE_GMTIME 1
#define HAVE_ASCTIME 1
#define HAVE_CTIME 1


#ifdef PHP_HPUX_TIME_R
#undef HAVE_LOCALTIME_R
#undef HAVE_ASCTIME_R
#undef HAVE_CTIME_R
#undef HAVE_GMTIME_R
#endif

#if !defined(HAVE_LOCALTIME_R) && defined(HAVE_LOCALTIME)
#define PHP_NEED_REENTRANCY 1
#define localtime_r php_localtime_r
PHPAPI struct tm *localtime_r(const time_t *const timep, struct tm *p_tm);
#endif


#if !defined(HAVE_CTIME_R) && defined(HAVE_CTIME)
#define PHP_NEED_REENTRANCY 1
#define ctime_r php_ctime_r
PHPAPI char *ctime_r(const time_t *clock, char *buf);
#endif


#if !defined(HAVE_ASCTIME_R) && defined(HAVE_ASCTIME)
#define PHP_NEED_REENTRANCY 1
#define asctime_r php_asctime_r
PHPAPI char *asctime_r(const struct tm *tm, char *buf);
#endif


#if !defined(HAVE_GMTIME_R) && defined(HAVE_GMTIME)
#define PHP_NEED_REENTRANCY 1
#define gmtime_r php_gmtime_r
PHPAPI struct tm *gmtime_r(const time_t *const timep, struct tm *p_tm);
#endif

#if !defined(HAVE_STRTOK_R)
#define strtok_r php_strtok_r
PHPAPI char *strtok_r(char *s, const char *delim, char **last);
#endif

#if !defined(HAVE_RAND_R)
#define rand_r php_rand_r
PHPAPI int rand_r(unsigned int *seed);
#endif

#if !defined(ZTS)
#undef PHP_NEED_REENTRANCY
#endif

#if defined(PHP_NEED_REENTRANCY)
void reentrancy_startup(void);
void reentrancy_shutdown(void);
#else
#define reentrancy_startup()
#define reentrancy_shutdown()
#endif

#endif	
