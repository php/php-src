#ifndef PHP_REENTRANCY_H
#define PHP_REENTRANCY_H

#include "php_config.h"

#include <time.h>

/* currently, PHP does not check for these functions, but assumes
   that they are available on all systems. */

#define HAVE_LOCALTIME 1
#define HAVE_GMTIME 1
#define HAVE_ASCTIME 1
#define HAVE_CTIME 1


#if !defined(HAVE_LOCALTIME_R) && defined(HAVE_LOCALTIME)
#define PHP_NEED_REENTRANCY 1
#define localtime_r php_localtime_r
struct tm *localtime_r(const time_t *const timep, struct tm *p_tm);
#endif


#if !defined(HAVE_CTIME_R) && defined(HAVE_CTIME)
#define PHP_NEED_REENTRANCY 1
#define ctime_r php_ctime_r
char *ctime_r(const time_t *clock, char *buf);
#endif


#if !defined(HAVE_ASCTIME_R) && defined(HAVE_ASCTIME)
#define PHP_NEED_REENTRANCY 1
#define asctime_r php_asctime_r
char *asctime_r(const struct tm *tm, char *buf);
#endif


#if !defined(HAVE_GMTIME_R) && defined(HAVE_GMTIME)
#define PHP_NEED_REENTRANCY 1
#define gmtime_r php_gmtime_r
struct tm *gmtime_r(const time_t *const timep, struct tm *p_tm);
#endif


#if defined(ZTS) && defined(PHP_NEED_REENTRANCY)
void reentrancy_startup(void);
void reentrancy_shutdown(void);
#else
#define reentrancy_startup()
#define reentrancy_shutdown()
#endif

#endif	
