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

#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: print.c,v 1.92 2022/09/10 13:21:42 christos Exp $")
#endif  /* lint */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>

#include "cdf.h"

#ifndef COMPILE_ONLY
protected void
file_mdump(struct magic *m)
{
	static const char optyp[] = { FILE_OPS };
	char tbuf[256];

	(void) fprintf(stderr, "%u: %.*s %u", m->lineno,
	    (m->cont_level & 7) + 1, ">>>>>>>>", m->offset);

	if (m->flag & INDIR) {
		(void) fprintf(stderr, "(%s,",
		    /* Note: type is unsigned */
		    (m->in_type < file_nnames) ? file_names[m->in_type] :
		    "*bad in_type*");
		if (m->in_op & FILE_OPINVERSE)
			(void) fputc('~', stderr);
		(void) fprintf(stderr, "%c%u),",
		    (CAST(size_t, m->in_op & FILE_OPS_MASK) <
		    __arraycount(optyp)) ?
		    optyp[m->in_op & FILE_OPS_MASK] : '?', m->in_offset);
	}
	(void) fprintf(stderr, " %s%s", (m->flag & UNSIGNED) ? "u" : "",
	    /* Note: type is unsigned */
	    (m->type < file_nnames) ? file_names[m->type] : "*bad type");
	if (m->mask_op & FILE_OPINVERSE)
		(void) fputc('~', stderr);

	if (IS_LIBMAGIC_STRING(m->type)) {
		if (m->str_flags) {
			(void) fputc('/', stderr);
			if (m->str_flags & STRING_COMPACT_WHITESPACE)
				(void) fputc(CHAR_COMPACT_WHITESPACE, stderr);
			if (m->str_flags & STRING_COMPACT_OPTIONAL_WHITESPACE)
				(void) fputc(CHAR_COMPACT_OPTIONAL_WHITESPACE,
				    stderr);
			if (m->str_flags & STRING_IGNORE_LOWERCASE)
				(void) fputc(CHAR_IGNORE_LOWERCASE, stderr);
			if (m->str_flags & STRING_IGNORE_UPPERCASE)
				(void) fputc(CHAR_IGNORE_UPPERCASE, stderr);
			if (m->str_flags & REGEX_OFFSET_START)
				(void) fputc(CHAR_REGEX_OFFSET_START, stderr);
			if (m->str_flags & STRING_TEXTTEST)
				(void) fputc(CHAR_TEXTTEST, stderr);
			if (m->str_flags & STRING_BINTEST)
				(void) fputc(CHAR_BINTEST, stderr);
			if (m->str_flags & PSTRING_1_BE)
				(void) fputc(CHAR_PSTRING_1_BE, stderr);
			if (m->str_flags & PSTRING_2_BE)
				(void) fputc(CHAR_PSTRING_2_BE, stderr);
			if (m->str_flags & PSTRING_2_LE)
				(void) fputc(CHAR_PSTRING_2_LE, stderr);
			if (m->str_flags & PSTRING_4_BE)
				(void) fputc(CHAR_PSTRING_4_BE, stderr);
			if (m->str_flags & PSTRING_4_LE)
				(void) fputc(CHAR_PSTRING_4_LE, stderr);
			if (m->str_flags & PSTRING_LENGTH_INCLUDES_ITSELF)
				(void) fputc(
				    CHAR_PSTRING_LENGTH_INCLUDES_ITSELF,
				    stderr);
		}
		if (m->str_range)
			(void) fprintf(stderr, "/%u", m->str_range);
	}
	else {
		if (CAST(size_t, m->mask_op & FILE_OPS_MASK) <
		    __arraycount(optyp))
			(void) fputc(optyp[m->mask_op & FILE_OPS_MASK], stderr);
		else
			(void) fputc('?', stderr);

		if (m->num_mask) {
			(void) fprintf(stderr, "%.8llx",
			    CAST(unsigned long long, m->num_mask));
		}
	}
	(void) fprintf(stderr, ",%c", m->reln);

	if (m->reln != 'x') {
		switch (m->type) {
		case FILE_BYTE:
		case FILE_SHORT:
		case FILE_LONG:
		case FILE_LESHORT:
		case FILE_LELONG:
		case FILE_MELONG:
		case FILE_BESHORT:
		case FILE_BELONG:
		case FILE_INDIRECT:
			(void) fprintf(stderr, "%d", m->value.l);
			break;
		case FILE_BEQUAD:
		case FILE_LEQUAD:
		case FILE_QUAD:
		case FILE_OFFSET:
			(void) fprintf(stderr, "%" INT64_T_FORMAT "d",
			    CAST(long long, m->value.q));
			break;
		case FILE_PSTRING:
		case FILE_STRING:
		case FILE_REGEX:
		case FILE_BESTRING16:
		case FILE_LESTRING16:
		case FILE_SEARCH:
			file_showstr(stderr, m->value.s,
			    CAST(size_t, m->vallen));
			break;
		case FILE_DATE:
		case FILE_LEDATE:
		case FILE_BEDATE:
		case FILE_MEDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdatetime(tbuf, sizeof(tbuf), m->value.l, 0));
			break;
		case FILE_LDATE:
		case FILE_LELDATE:
		case FILE_BELDATE:
		case FILE_MELDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdatetime(tbuf, sizeof(tbuf), m->value.l,
			    FILE_T_LOCAL));
			break;
		case FILE_QDATE:
		case FILE_LEQDATE:
		case FILE_BEQDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdatetime(tbuf, sizeof(tbuf), m->value.q, 0));
			break;
		case FILE_QLDATE:
		case FILE_LEQLDATE:
		case FILE_BEQLDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdatetime(tbuf, sizeof(tbuf), m->value.q,
			    FILE_T_LOCAL));
			break;
		case FILE_QWDATE:
		case FILE_LEQWDATE:
		case FILE_BEQWDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdatetime(tbuf, sizeof(tbuf), m->value.q,
			    FILE_T_WINDOWS));
			break;
		case FILE_FLOAT:
		case FILE_BEFLOAT:
		case FILE_LEFLOAT:
			(void) fprintf(stderr, "%G", m->value.f);
			break;
		case FILE_DOUBLE:
		case FILE_BEDOUBLE:
		case FILE_LEDOUBLE:
			(void) fprintf(stderr, "%G", m->value.d);
			break;
		case FILE_LEVARINT:
		case FILE_BEVARINT:
			(void)fprintf(stderr, "%s", file_fmtvarint(
			    tbuf, sizeof(tbuf), m->value.us, m->type));
			break;
		case FILE_MSDOSDATE:
		case FILE_BEMSDOSDATE:
		case FILE_LEMSDOSDATE:
			(void)fprintf(stderr, "%s,",
			    file_fmtdate(tbuf, sizeof(tbuf), m->value.h));
			break;
		case FILE_MSDOSTIME:
		case FILE_BEMSDOSTIME:
		case FILE_LEMSDOSTIME:
			(void)fprintf(stderr, "%s,",
			    file_fmttime(tbuf, sizeof(tbuf), m->value.h));
			break;
		case FILE_OCTAL:
			(void)fprintf(stderr, "%s",
			    file_fmtnum(tbuf, sizeof(tbuf), m->value.s, 8));
			break;
		case FILE_DEFAULT:
			/* XXX - do anything here? */
			break;
		case FILE_USE:
		case FILE_NAME:
		case FILE_DER:
			(void) fprintf(stderr, "'%s'", m->value.s);
			break;
		case FILE_GUID:
			(void) file_print_guid(tbuf, sizeof(tbuf),
			    m->value.guid);
			(void) fprintf(stderr, "%s", tbuf);
			break;

		default:
			(void) fprintf(stderr, "*bad type %d*", m->type);
			break;
		}
	}
	(void) fprintf(stderr, ",\"%s\"]\n", m->desc);
}
#endif

/*VARARGS*/
protected void
file_magwarn(struct magic_set *ms, const char *f, ...)
{
	va_list va;
	char *expanded_format = NULL;
	int expanded_len;

	va_start(va, f);
	expanded_len = vasprintf(&expanded_format, f, va);
	va_end(va);

	if (expanded_len >= 0 && expanded_format) {
		php_error_docref(NULL, E_WARNING, "%s", expanded_format);

		free(expanded_format);
	}
}

protected const char *
file_fmtvarint(char *buf, size_t blen, const unsigned char *us, int t)
{
	snprintf(buf, blen, "%jd", file_varint2uintmax_t(us, t, NULL));
	return buf;
}

protected const char *
file_fmtdatetime(char *buf, size_t bsize, uint64_t v, int flags)
{
	char *pp;
	time_t t;
	struct tm *tm, tmz;

	if (flags & FILE_T_WINDOWS) {
		struct timespec ts;
		cdf_timestamp_to_timespec(&ts, CAST(cdf_timestamp_t, v));
		t = ts.tv_sec;
	} else {
		// XXX: perhaps detect and print something if overflow
		// on 32 bit time_t?
		t = CAST(time_t, v);
	}

	if (flags & FILE_T_LOCAL) {
		tm = php_localtime_r(&t, &tmz);
	} else {
		tm = php_gmtime_r(&t, &tmz);
	}
	if (tm == NULL)
		goto out;
	pp = php_asctime_r(tm, buf);

	if (pp == NULL)
		goto out;
	pp[strcspn(pp, "\n")] = '\0';
	return pp;
out:
	strlcpy(buf, "*Invalid datetime*", bsize);
	return buf;
}

/* 
 * https://docs.microsoft.com/en-us/windows/win32/api/winbase/\
 *	nf-winbase-dosdatetimetofiletime?redirectedfrom=MSDN
 */
protected const char *
file_fmtdate(char *buf, size_t bsize, uint16_t v)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	tm.tm_mday = v & 0x1f;
	tm.tm_mon = ((v >> 5) & 0xf) - 1;
	tm.tm_year = (v >> 9) + 80;

	if (strftime(buf, bsize, "%a, %b %d %Y", &tm) == 0)
		goto out;

	return buf;
out:
	strlcpy(buf, "*Invalid date*", bsize);
	return buf;
}

protected const char *
file_fmttime(char *buf, size_t bsize, uint16_t v)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	tm.tm_sec = (v & 0x1f) * 2;
	tm.tm_min = ((v >> 5) & 0x3f);
	tm.tm_hour = (v >> 11);

	if (strftime(buf, bsize, "%T", &tm) == 0)
		goto out;

	return buf;
out:
	strlcpy(buf, "*Invalid time*", bsize);
	return buf;

}

protected const char *
file_fmtnum(char *buf, size_t blen, const char *us, int base)
{
	char *endptr;
	unsigned long long val;

	errno = 0;
	val = strtoull(us, &endptr, base);
	if (*endptr || errno) {
bad:		strlcpy(buf, "*Invalid number*", blen);
		return buf;
	}

	if (snprintf(buf, blen, "%llu", val) < 0)
		goto bad;
	return buf;
}
