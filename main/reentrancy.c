/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#include "php_reentrancy.h"

#include <string.h>

enum {
	LOCALTIME_R,
	CTIME_R,
	ASCTIME_R,
	GMTIME_R,
	NUMBER_OF_LOCKS
};


#if defined(ZTS)

#include <TSRM.h>

static MUTEX_T reentrant_locks[NUMBER_OF_LOCKS];

#define local_lock(x) tsrm_mutex_lock(reentrant_locks[x])
#define local_unlock(x) tsrm_mutex_unlock(reentrant_locks[x])

#else

#define local_lock(x)
#define local_unlock(x)

#endif


#if !defined(HAVE_LOCALTIME_R) && defined(HAVE_LOCALTIME)

struct tm *localtime_r(const time_t *const timep, struct tm *p_tm)
{
	struct tm *tmp;
	
	local_lock(LOCALTIME_R);

	tmp = localtime(timep);
	memcpy(p_tm, tmp, sizeof(struct tm));
	
	local_unlock(LOCALTIME_R);

	return p_tm;
}

#endif

#if !defined(HAVE_CTIME_R) && defined(HAVE_CTIME)

char *ctime_r(const time_t *clock, char *buf)
{
	char *tmp;
	
	local_lock(CTIME_R);

	tmp = ctime(clock);
	strcpy(buf, tmp);

	local_unlock(CTIME_R);
	
	return buf;
}

#endif

#if !defined(HAVE_ASCTIME_R) && defined(HAVE_ASCTIME)

char *asctime_r(const struct tm *tm, char *buf)
{
	char *tmp;
	
	local_lock(ASCTIME_R);

	tmp = asctime(tm);
	strcpy(buf, tmp);

	local_unlock(ASCTIME_R);

	return buf;
}

#endif

#if !defined(HAVE_GMTIME_R) && defined(HAVE_GMTIME)

struct tm *gmtime_r(const time_t *const timep, struct tm *p_tm)
{
	struct tm *tmp;
	
	local_lock(GMTIME_R);

	tmp = gmtime(timep);
	memcpy(p_tm, tmp, sizeof(struct tm));
	
	local_unlock(GMTIME_R);

	return p_tm;
}

#endif

#if defined(PHP_NEED_REENTRANCY) && defined(ZTS)

void reentrancy_shutdown(void)
{
	int i;

	for (i = 0; i < NUMBER_OF_LOCKS; i++) {
		tsrm_mutex_init(reentrant_locks[i]);
	}
}

void reentrancy_shutdown(void)
{
	int i;

	for (i = 0; i < NUMBER_OF_LOCKS; i++) {
		tsrm_mutex_destroy(reentrant_locks[i]);
	}
}

#endif
