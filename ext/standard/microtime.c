/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "microtime.h"
#include "snprintf.h"

#include <stdio.h>
#if HAVE_GETTIMEOFDAY
#if MSVC5
#include "win32/time.h"
#else
#include <sys/time.h>
#endif
#endif

#define NUL  '\0'
#define MICRO_IN_SEC 1000000.00

/* {{{ proto string microtime(void)
   Returns a string containing the current time in seconds and microseconds */
#ifdef __cplusplus
void php3_microtime(HashTable *)
#else
void php3_microtime(INTERNAL_FUNCTION_PARAMETERS)
#endif
{
#if HAVE_GETTIMEOFDAY
	struct timeval tp;
	long sec = 0L;
	double msec = 0.0;
	char ret[100];
	TLS_VARS;
	
	if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
		msec = (double) (tp.tv_usec / MICRO_IN_SEC);
		sec = tp.tv_sec;
	}
	snprintf(ret, 100, "%.8f %ld", msec, sec);
	RETVAL_STRING(ret,1);
#endif
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
