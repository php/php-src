/*
 * Copyright (c) Christos Zoulas 2003.
 * All Rights Reserved.
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
#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: funcs.c,v 1.131 2022/09/13 18:46:07 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>	/* for pipe2() */
#endif
#if defined(HAVE_WCHAR_H)
#include <wchar.h>
#endif
#if defined(HAVE_WCTYPE_H)
#include <wctype.h>
#endif
#include <limits.h>

#ifndef SIZE_MAX
#define SIZE_MAX	((size_t)~0)
#endif

protected char *
file_copystr(char *buf, size_t blen, size_t width, const char *str)
{
	if (blen == 0)
		return buf;
	if (width >= blen)
		width = blen - 1;
	memcpy(buf, str, width);
	buf[width] = '\0';
	return buf;
}

private void
file_clearbuf(struct magic_set *ms)
{
	free(ms->o.buf);
	ms->o.buf = NULL;
	ms->o.blen = 0;
}

private int
file_checkfield(char *msg, size_t mlen, const char *what, const char **pp)
{
	const char *p = *pp;
	int fw = 0;

	while (*p && isdigit((unsigned char)*p))
		fw = fw * 10 + (*p++ - '0');

	*pp = p;

	if (fw < 1024)
		return 1;
	if (msg)
		snprintf(msg, mlen, "field %s too large: %d", what, fw);

	return 0;
}

protected int
file_checkfmt(char *msg, size_t mlen, const char *fmt)
{
	const char *p;
	for (p = fmt; *p; p++) {
		if (*p != '%')
			continue;
		if (*++p == '%')
			continue;
		// Skip uninteresting.
		while (strchr("#0.'+- ", *p) != NULL)
			p++;
		if (*p == '*') {
			if (msg)
				snprintf(msg, mlen, "* not allowed in format");
			return -1;
		}

		if (!file_checkfield(msg, mlen, "width", &p))
			return -1;

		if (*p == '.') {
			p++;
			if (!file_checkfield(msg, mlen, "precision", &p))
				return -1;
		}

		if (!isalpha((unsigned char)*p)) {
			if (msg)
				snprintf(msg, mlen, "bad format char: %c", *p);
			return -1;
		}
	}
	return 0;
}

/*
 * Like printf, only we append to a buffer.
 */
protected int
file_vprintf(struct magic_set *ms, const char *fmt, va_list ap)
{
	int len;
	char *buf, *newstr;
	char tbuf[1024];

	if (ms->event_flags & EVENT_HAD_ERR)
		return 0;

	if (file_checkfmt(tbuf, sizeof(tbuf), fmt)) {
		file_clearbuf(ms);
		file_error(ms, 0, "Bad magic format `%s' (%s)", fmt, tbuf);
		return -1;
	}

	len = vasprintf(&buf, fmt, ap);
	if (len < 0 || (size_t)len > 1024 || len + ms->o.blen > 1024 * 1024) {
		size_t blen = ms->o.blen;
		free(buf);
		file_clearbuf(ms);
		file_error(ms, 0, "Output buffer space exceeded %d+%"
		    SIZE_T_FORMAT "u", len, blen);
		return -1;
	}

	if (ms->o.buf != NULL) {
		len = asprintf(&newstr, "%s%s", ms->o.buf, buf);
		free(buf);
		if (len < 0)
			goto out;
		free(ms->o.buf);
		buf = newstr;
	}
	ms->o.buf = buf;
	ms->o.blen = len;
	return 0;
out:
	file_clearbuf(ms);
	file_error(ms, errno, "vasprintf failed");
	return -1;
}

protected int
file_printf(struct magic_set *ms, const char *fmt, ...)
{
	int rv;
	va_list ap;

	va_start(ap, fmt);
	rv = file_vprintf(ms, fmt, ap);
	va_end(ap);
	return rv;
}

/*
 * error - print best error message possible
 */
/*VARARGS*/
__attribute__((__format__(__printf__, 3, 0)))
private void
file_error_core(struct magic_set *ms, int error, const char *f, va_list va,
    size_t lineno)
{
	/* Only the first error is ok */
	if (ms->event_flags & EVENT_HAD_ERR)
		return;
	if (lineno != 0) {
		file_clearbuf(ms);
		(void)file_printf(ms, "line %" SIZE_T_FORMAT "u:", lineno);
	}
	if (ms->o.buf && *ms->o.buf)
		(void)file_printf(ms, " ");
	(void)file_vprintf(ms, f, va);
	if (error > 0)
		(void)file_printf(ms, " (%s)", strerror(error));
	ms->event_flags |= EVENT_HAD_ERR;
	ms->error = error;
}

/*VARARGS*/
protected void
file_error(struct magic_set *ms, int error, const char *f, ...)
{
	va_list va;
	va_start(va, f);
	file_error_core(ms, error, f, va, 0);
	va_end(va);
}

/*
 * Print an error with magic line number.
 */
/*VARARGS*/
protected void
file_magerror(struct magic_set *ms, const char *f, ...)
{
	va_list va;
	va_start(va, f);
	file_error_core(ms, 0, f, va, ms->line);
	va_end(va);
}

protected void
file_oomem(struct magic_set *ms, size_t len)
{
	file_error(ms, errno, "cannot allocate %" SIZE_T_FORMAT "u bytes",
	    len);
}

protected void
file_badseek(struct magic_set *ms)
{
	file_error(ms, errno, "error seeking");
}

protected void
file_badread(struct magic_set *ms)
{
	file_error(ms, errno, "error reading");
}

#ifndef COMPILE_ONLY
#define FILE_SEPARATOR "\n- "

protected int
file_separator(struct magic_set *ms)
{
	return file_printf(ms, FILE_SEPARATOR);
}

static void
trim_separator(struct magic_set *ms)
{
	size_t l;

	if (ms->o.buf == NULL)
		return;

	l = strlen(ms->o.buf);
	if (l < sizeof(FILE_SEPARATOR))
		return;

	l -= sizeof(FILE_SEPARATOR) - 1;
	if (strcmp(ms->o.buf + l, FILE_SEPARATOR) != 0)
		return;

	ms->o.buf[l] = '\0';
}

static int
checkdone(struct magic_set *ms, int *rv)
{
	if ((ms->flags & MAGIC_CONTINUE) == 0)
		return 1;
	if (file_separator(ms) == -1)
		*rv = -1;
	return 0;
}

protected int
file_default(struct magic_set *ms, size_t nb)
{
	if (ms->flags & MAGIC_MIME) {
		if ((ms->flags & MAGIC_MIME_TYPE) &&
		    file_printf(ms, "application/%s",
			nb ? "octet-stream" : "x-empty") == -1)
			return -1;
		return 1;
	}
	if (ms->flags & MAGIC_APPLE) {
		if (file_printf(ms, "UNKNUNKN") == -1)
			return -1;
		return 1;
	}
	if (ms->flags & MAGIC_EXTENSION) {
		if (file_printf(ms, "???") == -1)
			return -1;
		return 1;
	}
	return 0;
}

/*
 * The magic detection functions return:
 *	 1: found
 *	 0: not found
 *	-1: error
 */
/*ARGSUSED*/
protected int
file_buffer(struct magic_set *ms, int fd, struct stat *st,
    const char *inname __attribute__ ((__unused__)),
    const void *buf, size_t nb)
{
	int m = 0, rv = 0, looks_text = 0;
	const char *code = NULL;
	const char *code_mime = "binary";
	const char *def = "data";
	const char *ftype = NULL;
	char *rbuf = NULL;
	struct buffer b;

	buffer_init(&b, fd, st, buf, nb);
	ms->mode = b.st.st_mode;

	if (nb == 0) {
		def = "empty";
		goto simple;
	} else if (nb == 1) {
		def = "very short file (no magic)";
		goto simple;
	}

	if ((ms->flags & MAGIC_NO_CHECK_ENCODING) == 0) {
		looks_text = file_encoding(ms, &b, NULL, 0,
		    &code, &code_mime, &ftype);
	}

#ifdef __EMX__
	if ((ms->flags & MAGIC_NO_CHECK_APPTYPE) == 0 && inname) {
		m = file_os2_apptype(ms, inname, &b);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try os2_apptype %d]\n", m);
		switch (m) {
		case -1:
			return -1;
		case 0:
			break;
		default:
			return 1;
		}
	}
#endif
#if HAVE_FORK
	/* try compression stuff */
	if ((ms->flags & MAGIC_NO_CHECK_COMPRESS) == 0) {
		m = file_zmagic(ms, &b, inname);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try zmagic %d]\n", m);
		if (m) {
			goto done_encoding;
		}
	}
#endif
	/* Check if we have a tar file */
	if ((ms->flags & MAGIC_NO_CHECK_TAR) == 0) {
		m = file_is_tar(ms, &b);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try tar %d]\n", m);
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}

	/* Check if we have a JSON file */
	if ((ms->flags & MAGIC_NO_CHECK_JSON) == 0) {
		m = file_is_json(ms, &b);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try json %d]\n", m);
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}

	/* Check if we have a CSV file */
	if ((ms->flags & MAGIC_NO_CHECK_CSV) == 0) {
		m = file_is_csv(ms, &b, looks_text);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try csv %d]\n", m);
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}

	/* Check if we have a CDF file */
	if ((ms->flags & MAGIC_NO_CHECK_CDF) == 0) {
		m = file_trycdf(ms, &b);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try cdf %d]\n", m);
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}
#ifdef BUILTIN_ELF
	if ((ms->flags & MAGIC_NO_CHECK_ELF) == 0 && nb > 5 && fd != -1) {
		file_pushbuf_t *pb;
		/*
		 * We matched something in the file, so this
		 * *might* be an ELF file, and the file is at
		 * least 5 bytes long, so if it's an ELF file
		 * it has at least one byte past the ELF magic
		 * number - try extracting information from the
		 * ELF headers that cannot easily be  extracted
		 * with rules in the magic file. We we don't
		 * print the information yet.
		 */
		if ((pb = file_push_buffer(ms)) == NULL)
			return -1;

		rv = file_tryelf(ms, &b);
		rbuf = file_pop_buffer(ms, pb);
		if (rv == -1) {
			free(rbuf);
			rbuf = NULL;
		}
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try elf %d]\n", m);
	}
#endif

	/* try soft magic tests */
	if ((ms->flags & MAGIC_NO_CHECK_SOFT) == 0) {
		m = file_softmagic(ms, &b, NULL, NULL, BINTEST, looks_text);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try softmagic %d]\n", m);
		if (m == 1 && rbuf) {
			if (file_printf(ms, "%s", rbuf) == -1)
				goto done;
		}
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}

	/* try text properties */
	if ((ms->flags & MAGIC_NO_CHECK_TEXT) == 0) {

		m = file_ascmagic(ms, &b, looks_text);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try ascmagic %d]\n", m);
		if (m) {
			goto done;
		}
	}

simple:
	/* give up */
	if (m == 0) {
		m = 1;
		rv = file_default(ms, nb);
		if (rv == 0)
			if (file_printf(ms, "%s", def) == -1)
				rv = -1;
	}
 done:
	trim_separator(ms);
	if ((ms->flags & MAGIC_MIME_ENCODING) != 0) {
		if (ms->flags & MAGIC_MIME_TYPE)
			if (file_printf(ms, "; charset=") == -1)
				rv = -1;
		if (file_printf(ms, "%s", code_mime) == -1)
			rv = -1;
	}
#if HAVE_FORK
 done_encoding:
#endif
	free(rbuf);
	buffer_fini(&b);
	if (rv)
		return rv;

	return m;
}
#endif

protected int
file_reset(struct magic_set *ms, int checkloaded)
{
	if (checkloaded && ms->mlist[0] == NULL) {
		file_error(ms, 0, "no magic files loaded");
		return -1;
	}
	file_clearbuf(ms);
	if (ms->o.pbuf) {
		free(ms->o.pbuf);
		ms->o.pbuf = NULL;
	}
	ms->event_flags &= ~EVENT_HAD_ERR;
	ms->error = -1;
	return 0;
}

#define OCTALIFY(n, o)	\
	/*LINTED*/ \
	(void)(*(n)++ = '\\', \
	*(n)++ = ((CAST(uint32_t, *(o)) >> 6) & 3) + '0', \
	*(n)++ = ((CAST(uint32_t, *(o)) >> 3) & 7) + '0', \
	*(n)++ = ((CAST(uint32_t, *(o)) >> 0) & 7) + '0', \
	(o)++)

protected const char *
file_getbuffer(struct magic_set *ms)
{
	char *pbuf, *op, *np;
	size_t psize, len;

	if (ms->event_flags & EVENT_HAD_ERR)
		return NULL;

	if (ms->flags & MAGIC_RAW)
		return ms->o.buf;

	if (ms->o.buf == NULL)
		return NULL;

	/* * 4 is for octal representation, + 1 is for NUL */
	len = strlen(ms->o.buf);
	if (len > (SIZE_MAX - 1) / 4) {
		file_oomem(ms, len);
		return NULL;
	}
	psize = len * 4 + 1;
	if ((pbuf = CAST(char *, realloc(ms->o.pbuf, psize))) == NULL) {
		file_oomem(ms, psize);
		return NULL;
	}
	ms->o.pbuf = pbuf;

#if defined(HAVE_WCHAR_H) && defined(HAVE_MBRTOWC) && defined(HAVE_WCWIDTH)
	{
		mbstate_t state;
		wchar_t nextchar;
		int mb_conv = 1;
		size_t bytesconsumed;
		char *eop;
		(void)memset(&state, 0, sizeof(mbstate_t));

		np = ms->o.pbuf;
		op = ms->o.buf;
		eop = op + len;

		while (op < eop) {
			bytesconsumed = mbrtowc(&nextchar, op,
			    CAST(size_t, eop - op), &state);
			if (bytesconsumed == CAST(size_t, -1) ||
			    bytesconsumed == CAST(size_t, -2)) {
				mb_conv = 0;
				break;
			}

			if (iswprint(nextchar)) {
				(void)memcpy(np, op, bytesconsumed);
				op += bytesconsumed;
				np += bytesconsumed;
			} else {
				while (bytesconsumed-- > 0)
					OCTALIFY(np, op);
			}
		}
		*np = '\0';

		/* Parsing succeeded as a multi-byte sequence */
		if (mb_conv != 0)
			return ms->o.pbuf;
	}
#endif

	for (np = ms->o.pbuf, op = ms->o.buf; *op;) {
		if (isprint(CAST(unsigned char, *op))) {
			*np++ = *op++;
		} else {
			OCTALIFY(np, op);
		}
	}
	*np = '\0';
	return ms->o.pbuf;
}

protected int
file_check_mem(struct magic_set *ms, unsigned int level)
{
	size_t len;

	if (level >= ms->c.len) {
		len = (ms->c.len = 20 + level) * sizeof(*ms->c.li);
		ms->c.li = CAST(struct level_info *, (ms->c.li == NULL) ?
		    malloc(len) :
		    realloc(ms->c.li, len));
		if (ms->c.li == NULL) {
			file_oomem(ms, len);
			return -1;
		}
	}
	ms->c.li[level].got_match = 0;
#ifdef ENABLE_CONDITIONALS
	ms->c.li[level].last_match = 0;
	ms->c.li[level].last_cond = COND_NONE;
#endif /* ENABLE_CONDITIONALS */
	return 0;
}

protected size_t
file_printedlen(const struct magic_set *ms)
{
	return ms->o.blen;
}

protected int
file_replace(struct magic_set *ms, const char *pat, const char *rep)
{
	file_regex_t rx;
	int rc, rv = -1;

	rc = file_regcomp(ms, &rx, pat, REG_EXTENDED);
	if (rc == 0) {
		regmatch_t rm;
		int nm = 0;
		while (file_regexec(ms, &rx, ms->o.buf, 1, &rm, 0) == 0) {
			ms->o.buf[rm.rm_so] = '\0';
			if (file_printf(ms, "%s%s", rep,
			    rm.rm_eo != 0 ? ms->o.buf + rm.rm_eo : "") == -1)
				goto out;
			nm++;
		}
		rv = nm;
	}
out:
	file_regfree(&rx);
	return rv;
}

protected int
file_regcomp(struct magic_set *ms file_locale_used, file_regex_t *rx,
    const char *pat, int flags)
{
#ifdef USE_C_LOCALE
	locale_t old = uselocale(ms->c_lc_ctype);
	assert(old != NULL);
#else
	char old[1024];
	strlcpy(old, setlocale(LC_CTYPE, NULL), sizeof(old));
	(void)setlocale(LC_CTYPE, "C");
#endif
	int rc;
	rc = regcomp(rx, pat, flags);

#ifdef USE_C_LOCALE
	uselocale(old);
#else
	(void)setlocale(LC_CTYPE, old);
#endif
	if (rc > 0 && (ms->flags & MAGIC_CHECK)) {
		char errmsg[512];

		(void)regerror(rc, rx, errmsg, sizeof(errmsg));
		file_magerror(ms, "regex error %d for `%s', (%s)", rc, pat,
		    errmsg);
	}
	return rc;
}

/*ARGSUSED*/
protected int
file_regexec(struct magic_set *ms file_locale_used, file_regex_t *rx,
    const char *str, size_t nmatch, regmatch_t* pmatch, int eflags)
{
#ifdef USE_C_LOCALE
	locale_t old = uselocale(ms->c_lc_ctype);
	assert(old != NULL);
#else
	char old[1024];
	strlcpy(old, setlocale(LC_CTYPE, NULL), sizeof(old));
	(void)setlocale(LC_CTYPE, "C");
#endif
	int rc;
	/* XXX: force initialization because glibc does not always do this */
	if (nmatch != 0)
		memset(pmatch, 0, nmatch * sizeof(*pmatch));
	rc = regexec(rx, str, nmatch, pmatch, eflags);
#ifdef USE_C_LOCALE
	uselocale(old);
#else
	(void)setlocale(LC_CTYPE, old);
#endif
	return rc;
}

protected void
file_regfree(file_regex_t *rx)
{
	regfree(rx);
}

protected file_pushbuf_t *
file_push_buffer(struct magic_set *ms)
{
	file_pushbuf_t *pb;

	if (ms->event_flags & EVENT_HAD_ERR)
		return NULL;

	if ((pb = (CAST(file_pushbuf_t *, malloc(sizeof(*pb))))) == NULL)
		return NULL;

	pb->buf = ms->o.buf;
	pb->blen = ms->o.blen;
	pb->offset = ms->offset;

	ms->o.buf = NULL;
	ms->o.blen = 0;
	ms->offset = 0;

	return pb;
}

protected char *
file_pop_buffer(struct magic_set *ms, file_pushbuf_t *pb)
{
	char *rbuf;

	if (ms->event_flags & EVENT_HAD_ERR) {
		free(pb->buf);
		free(pb);
		return NULL;
	}

	rbuf = ms->o.buf;

	ms->o.buf = pb->buf;
	ms->o.blen = pb->blen;
	ms->offset = pb->offset;

	free(pb);
	return rbuf;
}

/*
 * convert string to ascii printable format.
 */
protected char *
file_printable(struct magic_set *ms, char *buf, size_t bufsiz,
    const char *str, size_t slen)
{
	char *ptr, *eptr = buf + bufsiz - 1;
	const unsigned char *s = RCAST(const unsigned char *, str);
	const unsigned char *es = s + slen;

	for (ptr = buf;  ptr < eptr && s < es && *s; s++) {
		if ((ms->flags & MAGIC_RAW) != 0 || isprint(*s)) {
			*ptr++ = *s;
			continue;
		}
		if (ptr >= eptr - 3)
			break;
		*ptr++ = '\\';
		*ptr++ = ((CAST(unsigned int, *s) >> 6) & 7) + '0';
		*ptr++ = ((CAST(unsigned int, *s) >> 3) & 7) + '0';
		*ptr++ = ((CAST(unsigned int, *s) >> 0) & 7) + '0';
	}
	*ptr = '\0';
	return buf;
}

struct guid {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
};

protected int
file_parse_guid(const char *s, uint64_t *guid)
{
	struct guid *g = CAST(struct guid *, CAST(void *, guid));
#ifndef WIN32
	return sscanf(s,
	    "%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
	    &g->data1, &g->data2, &g->data3, &g->data4[0], &g->data4[1],
	    &g->data4[2], &g->data4[3], &g->data4[4], &g->data4[5],
	    &g->data4[6], &g->data4[7]) == 11 ? 0 : -1;
#else
	/* MS-Windows runtime doesn't support %hhx, except under
	   non-default __USE_MINGW_ANSI_STDIO.  */
	uint16_t data16[8];
	int rv = sscanf(s, "%8x-%4hx-%4hx-%2hx%2hx-%2hx%2hx%2hx%2hx%2hx%2hx",
	    &g->data1, &g->data2, &g->data3, &data16[0], &data16[1],
	    &data16[2], &data16[3], &data16[4], &data16[5],
	    &data16[6], &data16[7]) == 11 ? 0 : -1;
	int i;
	for (i = 0; i < 8; i++)
	    g->data4[i] = data16[i];
	return rv;
#endif
}

protected int
file_print_guid(char *str, size_t len, const uint64_t *guid)
{
	const struct guid *g = CAST(const struct guid *,
	    CAST(const void *, guid));

#ifndef WIN32
	return snprintf(str, len, "%.8X-%.4hX-%.4hX-%.2hhX%.2hhX-"
	    "%.2hhX%.2hhX%.2hhX%.2hhX%.2hhX%.2hhX",
	    g->data1, g->data2, g->data3, g->data4[0], g->data4[1],
	    g->data4[2], g->data4[3], g->data4[4], g->data4[5],
	    g->data4[6], g->data4[7]);
#else
	return snprintf(str, len, "%.8X-%.4hX-%.4hX-%.2hX%.2hX-"
	    "%.2hX%.2hX%.2hX%.2hX%.2hX%.2hX",
	    g->data1, g->data2, g->data3, g->data4[0], g->data4[1],
	    g->data4[2], g->data4[3], g->data4[4], g->data4[5],
	    g->data4[6], g->data4[7]);
#endif
}

protected int
file_pipe_closexec(int *fds)
{
#ifdef HAVE_PIPE2
	return pipe2(fds, O_CLOEXEC);
#else
	if (pipe(fds) == -1)
		return -1;
# ifdef F_SETFD
	(void)fcntl(fds[0], F_SETFD, FD_CLOEXEC);
	(void)fcntl(fds[1], F_SETFD, FD_CLOEXEC);
# endif
	return 0;
#endif
}

protected int
file_clear_closexec(int fd) {
#ifdef F_SETFD
	return fcntl(fd, F_SETFD, 0);
#else
	return 0;
#endif
}

protected char *
file_strtrim(char *str)
{
	char *last;

	while (isspace(CAST(unsigned char, *str)))
		str++;
	last = str;
	while (*last)
		last++;
	--last;
	while (isspace(CAST(unsigned char, *last)))
		last--;
	*++last = '\0';
	return str;
}
