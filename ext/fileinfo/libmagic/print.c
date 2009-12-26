/*
 * Copyright (c) Ian F. Darwin 1986-1995.
 * Software written by Ian F. Darwin and others;
 * maintained 1995-present by Christos Zoulas and others.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice immediately at the beginning of the file, without modification,
 *    this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * print.c - debugging printout routines
 */

#include "php.h"

#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: print.c,v 1.66 2009/02/03 20:27:51 christos Exp $")
#endif  /* lint */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>

#define SZOF(a)	(sizeof(a) / sizeof(a[0]))

/*VARARGS*/
protected void
file_magwarn(struct magic_set *ms, const char *f, ...)
{
	va_list va;
	char *expanded_format;
	TSRMLS_FETCH();

	va_start(va, f);
	vasprintf(&expanded_format, f, va);
	va_end(va);
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Warning: %s", expanded_format);

	free(expanded_format);
}

protected const char *
file_fmttime(uint32_t v, int local)
{
	char *pp;
	time_t t = (time_t)v;
	struct tm *tm;

	if (local) {
		pp = ctime(&t);
	} else {
#ifndef HAVE_DAYLIGHT
		private int daylight = 0;
#ifdef HAVE_TM_ISDST
		private time_t now = (time_t)0;

		if (now == (time_t)0) {
			struct tm *tm1;
			(void)time(&now);
			tm1 = localtime(&now);
			if (tm1 == NULL)
				return "*Invalid time*";
			daylight = tm1->tm_isdst;
		}
#endif /* HAVE_TM_ISDST */
#endif /* HAVE_DAYLIGHT */
		if (daylight)
			t += 3600;
		tm = gmtime(&t);
		if (tm == NULL)
			return "*Invalid time*";
		pp = asctime(tm);
	}

	pp[strcspn(pp, "\n")] = '\0';
	return pp;
}
