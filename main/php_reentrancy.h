/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_REENTRANCY_H
#define PHP_REENTRANCY_H

#include "php.h"

#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <time.h>

/* currently, PHP does not check for these functions, but assumes
   that they are available on all systems. */

#define HAVE_LOCALTIME 1
#define HAVE_GMTIME 1
#define HAVE_ASCTIME 1
#define HAVE_CTIME 1

#if defined(PHP_IRIX_TIME_R)
#undef HAVE_ASCTIME_R
#undef HAVE_CTIME_R
#endif

#if defined(PHP_HPUX_TIME_R)
#undef HAVE_LOCALTIME_R
#undef HAVE_ASCTIME_R
#undef HAVE_CTIME_R
#undef HAVE_GMTIME_R
#endif

BEGIN_EXTERN_C()

#if !defined(HAVE_LOCALTIME_R) && defined(HAVE_LOCALTIME)
#define PHP_NEED_REENTRANCY 1
PHPAPI struct tm *php_localtime_r(const time_t *const timep, struct tm *p_tm);
#else
#define php_localtime_r localtime_r
#ifdef MISSING_LOCALTIME_R_DECL
struct tm *localtime_r(const time_t *const timep, struct tm *p_tm);
#endif
#endif


#if !defined(HAVE_CTIME_R) && defined(HAVE_CTIME)
#define PHP_NEED_REENTRANCY 1
PHPAPI char *php_ctime_r(const time_t *clock, char *buf);
#else
#define php_ctime_r ctime_r
#ifdef MISSING_CTIME_R_DECL
char *ctime_r(const time_t *clock, char *buf);
#endif
#endif


#if !defined(HAVE_ASCTIME_R) && defined(HAVE_ASCTIME)
#define PHP_NEED_REENTRANCY 1
PHPAPI char *php_asctime_r(const struct tm *tm, char *buf);
#else
#define php_asctime_r asctime_r
#ifdef MISSING_ASCTIME_R_DECL
char *asctime_r(const struct tm *tm, char *buf);
#endif
#endif


#if !defined(HAVE_GMTIME_R) && defined(HAVE_GMTIME)
#define PHP_NEED_REENTRANCY 1
PHPAPI struct tm *php_gmtime_r(const time_t *const timep, struct tm *p_tm);
#else
#define php_gmtime_r gmtime_r
#ifdef MISSING_GMTIME_R_DECL
struct tm *php_gmtime_r(const time_t *const timep, struct tm *p_tm);
#endif
#endif

#if !defined(HAVE_STRTOK_R)
PHPAPI char *php_strtok_r(char *s, const char *delim, char **last);
#else
#define php_strtok_r strtok_r
#ifdef MISSING_STRTOK_R_DECL
char *strtok_r(char *s, const char *delim, char **last);
#endif
#endif

#if !defined(HAVE_RAND_R)
PHPAPI int php_rand_r(unsigned int *seed);
#else
#define php_rand_r rand_r
#endif

END_EXTERN_C()

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
