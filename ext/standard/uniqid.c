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
   | Author: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#if MSVC5
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include "uniqid.h"

/* {{{ proto string uniqid(string prefix)
   Generate a unique id */
void php3_uniqid(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef HAVE_GETTIMEOFDAY
	pval *prefix;

	char uniqid[128];
	int sec, usec;
	struct timeval tv;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&prefix)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(prefix);

	/* Do some bounds checking since we are using a char array. */
	if (strlen(prefix->value.str.val) > 114) {
		php3_error(E_WARNING, "The prefix to uniqid should not be more than 114 characters.");
		return;
	}
	/* dont need this on windows so lets not do it*/
#if HAVE_USLEEP && !(WIN32|WINNT)
	usleep(1);
#endif
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 1000000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs:
	 */
	sprintf(uniqid, "%s%08x%05x", prefix->value.str.val, sec, usec);

	RETURN_STRING(uniqid,1);
#endif
}
/* }}} */

function_entry uniqid_functions[] = {
	{"uniqid",		php3_uniqid,		NULL},
	{NULL, NULL, NULL}
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
