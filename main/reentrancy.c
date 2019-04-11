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

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#include "php_reentrancy.h"
#include "ext/standard/php_rand.h"                   /* for PHP_RAND_MAX */

enum {
	LOCALTIME_R,
	CTIME_R,
	ASCTIME_R,
	GMTIME_R,
	READDIR_R,
	NUMBER_OF_LOCKS
};

#if defined(PHP_NEED_REENTRANCY)

#include <TSRM.h>

static MUTEX_T reentrant_locks[NUMBER_OF_LOCKS];

#define local_lock(x) tsrm_mutex_lock(reentrant_locks[x])
#define local_unlock(x) tsrm_mutex_unlock(reentrant_locks[x])

#else

#define local_lock(x)
#define local_unlock(x)

#endif

#if defined(PHP_IRIX_TIME_R)

#define HAVE_CTIME_R 1
#define HAVE_ASCTIME_R 1

PHPAPI char *php_ctime_r(const time_t *clock, char *buf)
{
	if (ctime_r(clock, buf) == buf)
		return (buf);
	return (NULL);
}

PHPAPI char *php_asctime_r(const struct tm *tm, char *buf)
{
	if (asctime_r(tm, buf) == buf)
		return (buf);
	return (NULL);
}

#endif

#if defined(PHP_HPUX_TIME_R)

#define HAVE_LOCALTIME_R 1
#define HAVE_CTIME_R 1
#define HAVE_ASCTIME_R 1
#define HAVE_GMTIME_R 1

PHPAPI struct tm *php_localtime_r(const time_t *const timep, struct tm *p_tm)
{
	if (localtime_r(timep, p_tm) == 0)
		return (p_tm);
	return (NULL);
}

PHPAPI char *php_ctime_r(const time_t *clock, char *buf)
{
	if (ctime_r(clock, buf, 26) != -1)
		return (buf);
	return (NULL);
}

PHPAPI char *php_asctime_r(const struct tm *tm, char *buf)
{
	if (asctime_r(tm, buf, 26) != -1)
		return (buf);
	return (NULL);
}

PHPAPI struct tm *php_gmtime_r(const time_t *const timep, struct tm *p_tm)
{
	if (gmtime_r(timep, p_tm) == 0)
		return (p_tm);
	return (NULL);
}

#endif

#if !defined(HAVE_LOCALTIME_R) && defined(HAVE_LOCALTIME)

PHPAPI struct tm *php_localtime_r(const time_t *const timep, struct tm *p_tm)
{
	struct tm *tmp;

	local_lock(LOCALTIME_R);

	tmp = localtime(timep);
	if (tmp) {
		memcpy(p_tm, tmp, sizeof(struct tm));
		tmp = p_tm;
	}

	local_unlock(LOCALTIME_R);

	return tmp;
}

#endif

#if !defined(HAVE_CTIME_R) && defined(HAVE_CTIME)

PHPAPI char *php_ctime_r(const time_t *clock, char *buf)
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

PHPAPI char *php_asctime_r(const struct tm *tm, char *buf)
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

PHPAPI struct tm *php_gmtime_r(const time_t *const timep, struct tm *p_tm)
{
	struct tm *tmp;

	local_lock(GMTIME_R);

	tmp = gmtime(timep);
	if (tmp) {
		memcpy(p_tm, tmp, sizeof(struct tm));
		tmp = p_tm;
	}

	local_unlock(GMTIME_R);

	return tmp;
}

#endif

#if defined(PHP_NEED_REENTRANCY)

void reentrancy_startup(void)
{
	int i;

	for (i = 0; i < NUMBER_OF_LOCKS; i++) {
		reentrant_locks[i] = tsrm_mutex_alloc();
	}
}

void reentrancy_shutdown(void)
{
	int i;

	for (i = 0; i < NUMBER_OF_LOCKS; i++) {
		tsrm_mutex_free(reentrant_locks[i]);
	}
}

#endif

#ifndef HAVE_RAND_R

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Posix rand_r function added May 1999 by Wes Peters <wes@softweyr.com>.
 */

#include <sys/types.h>
#include <stdlib.h>

static int
do_rand(unsigned long *ctx)
{
	return ((*ctx = *ctx * 1103515245 + 12345) % ((u_long)PHP_RAND_MAX + 1));
}


PHPAPI int
php_rand_r(unsigned int *ctx)
{
	u_long val = (u_long) *ctx;
	*ctx = do_rand(&val);
	return (int) *ctx;
}

#endif


#ifndef HAVE_STRTOK_R

/*
 * Copyright (c) 1998 Softweyr LLC.  All rights reserved.
 *
 * strtok_r, from Berkeley strtok
 * Oct 13, 1998 by Wes Peters <wes@softweyr.com>
 *
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notices, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notices, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *
 *	This product includes software developed by Softweyr LLC, the
 *      University of California, Berkeley, and its contributors.
 *
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SOFTWEYR LLC, THE REGENTS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SOFTWEYR LLC, THE
 * REGENTS, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>

PHPAPI char *
php_strtok_r(char *s, const char *delim, char **last)
{
    char *spanp;
    int c, sc;
    char *tok;

    if (s == NULL && (s = *last) == NULL)
    {
	return NULL;
    }

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0; )
    {
	if (c == sc)
	{
	    goto cont;
	}
    }

    if (c == 0)		/* no non-delimiter characters */
    {
	*last = NULL;
	return NULL;
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {
	c = *s++;
	spanp = (char *)delim;
	do
	{
	    if ((sc = *spanp++) == c)
	    {
		if (c == 0)
		{
		    s = NULL;
		}
		else
		{
		    char *w = s - 1;
		    *w = '\0';
		}
		*last = s;
		return tok;
	    }
	}
	while (sc != 0);
    }
    /* NOTREACHED */
}

#endif
