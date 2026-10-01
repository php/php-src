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
FILE_RCSID("@(#)$File: funcs.c,v 1.153 2026/05/25 14:07:05 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include "swap.h"
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

file_protected int
file_bigendian(void)
{
	union {
		unsigned short x;
		unsigned char s[sizeof(unsigned short)];
	} u;

	u.x = 1;
	return u.s[0] != 1;
}

file_protected char *
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

file_private void
file_clearbuf(struct magic_set *ms)
{
	efree(ms->o.buf);
	ms->o.buf = NULL;
	ms->o.blen = 0;
}

file_private int
file_checkfield(char *msg, size_t mlen, const char *what, const char **pp)
{
	const char *p = *pp;
	int fw = 0;

	while (*p && isdigit((unsigned char)*p)) {
		fw = fw * 10 + (*p++ - '0');
		if (fw > 1024)
			break;
	}

	*pp = p;

	if (fw < 1024)
		return 1;
	if (msg)
		snprintf(msg, mlen, "field %s too large: %d", what, fw);

	return 0;
}

file_protected int
file_checkfmt(char *msg, size_t mlen, const char *fmt)
{
	const char *p;
	for (p = fmt; *p; p++) {
		if (*p != '%')
			continue;
		if (*++p == '%')
			continue;
		if (*p == '\0') {
			if (msg)
				snprintf(msg, mlen, "incomplete %% format");
			return -1;
		}
		// Skip uninteresting.
		while (*p != '\0' && strchr("#0.'+- ", *p) != NULL)
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
file_protected int
file_vprintf(struct magic_set *ms, const char *fmt, va_list ap)
{
	size_t len;
	char *buf, *newstr;
	char tbuf[1024];

	if (ms->event_flags & EVENT_HAD_ERR)
		return 0;

	if (file_checkfmt(tbuf, sizeof(tbuf), fmt)) {
		file_clearbuf(ms);
		file_error(ms, 0, "Bad magic format `%s' (%s)", fmt, tbuf);
		return -1;
	}

	len = vspprintf(&buf, 0, fmt, ap);
	if (len > 1024 || len + ms->o.blen > 1024 * 1024) {
		size_t blen = ms->o.blen;
		if (buf) efree(buf);
		file_clearbuf(ms);
		file_error(ms, 0, "Output buffer space exceeded %" SIZE_T_FORMAT "u+%"
		    SIZE_T_FORMAT "u", len, blen);
		return -1;
	}

	if (ms->o.buf != NULL) {
		len = spprintf(&newstr, 0, "%s%s", ms->o.buf, buf);
		efree(buf);
		efree(ms->o.buf);
		buf = newstr;
	}
	ms->o.buf = buf;
	ms->o.blen = len;
	return 0;
}

file_protected int
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
file_private void
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
file_protected void
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
file_protected void
file_magerror(struct magic_set *ms, const char *f, ...)
{
	va_list va;
	va_start(va, f);
	file_error_core(ms, 0, f, va, ms->line);
	va_end(va);
}

file_protected void
file_oomem(struct magic_set *ms, size_t len)
{
	file_error(ms, errno, "cannot allocate %" SIZE_T_FORMAT "u bytes",
	    len);
}

file_protected void
file_badseek(struct magic_set *ms)
{
	file_error(ms, errno, "error seeking");
}

file_protected void
file_badread(struct magic_set *ms)
{
	file_error(ms, errno, "error reading");
}

#ifndef COMPILE_ONLY
#define FILE_SEPARATOR "\n- "

file_protected int
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

file_protected int
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
		// This is not a typo: Type: UNKN Creator: UNKN
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
file_protected int
file_buffer(struct magic_set *ms, php_stream *stream, zend_stat_t *st,
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
	int fd = -1;

	if (stream) {
#ifdef _WIN64
		php_socket_t _fd = fd;
#else
		int _fd;
#endif
		int _ret = php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&_fd, 0);
		if (SUCCESS == _ret) {
			fd = (int)_fd;
		}
	}

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

#ifdef PHP_FILEINFO_UNCOMPRESS
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
		m = file_is_csv(ms, &b, looks_text, code);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try csv %d]\n", m);
		if (m) {
			if (checkdone(ms, &rv))
				goto done;
		}
	}

	/* Check if we have a SIMH tape file */
	if ((ms->flags & MAGIC_NO_CHECK_SIMH) == 0) {
		m = file_is_simh(ms, &b);
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void)fprintf(stderr, "[try simh %d]\n", m);
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
			efree(rbuf);
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
#ifdef PHP_FILEINFO_UNCOMPRESS
 done_encoding:
#endif
	efree(rbuf);
	buffer_fini(&b);
	if (rv)
		return rv;

	return m;
}
#endif

file_protected int
file_reset(struct magic_set *ms, int checkloaded)
{
	if (checkloaded && ms->mlist[0] == NULL) {
		file_error(ms, 0, "no magic files loaded");
		return -1;
	}
	file_clearbuf(ms);
	if (ms->o.pbuf) {
		efree(ms->o.pbuf);
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

file_protected const char *
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
	if ((pbuf = CAST(char *, erealloc(ms->o.pbuf, psize))) == NULL) {
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

file_protected int
file_check_mem(struct magic_set *ms, unsigned int level)
{
	size_t len;

	if (level >= ms->c.len) {
		len = (ms->c.len = 20 + level) * sizeof(*ms->c.li);
		ms->c.li = CAST(struct level_info *, (ms->c.li == NULL) ?
		    emalloc(len) :
		    erealloc(ms->c.li, len));
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

file_protected size_t
file_printedlen(const struct magic_set *ms)
{
	return ms->o.blen;
}

file_protected int
file_replace(struct magic_set *ms, const char *pat, const char *rep)
{
	zend_string *pattern;
	uint32_t opts = 0;
	pcre_cache_entry *pce;
	zend_string *res;
	zend_string *repl;
	size_t rep_cnt = 0;

	opts |= PCRE2_MULTILINE;
	pattern = convert_libmagic_pattern(pat, strlen(pat), opts);
	pce = pcre_get_compiled_regex_cache_ex(pattern, 0);
	zend_string_release_ex(pattern, 0);
	if (pce == NULL) {
		rep_cnt = -1;
		goto out;
	}

	repl = zend_string_init(rep, strlen(rep), 0);
	res = php_pcre_replace_impl(pce, NULL, ms->o.buf, strlen(ms->o.buf), repl, -1, &rep_cnt);

	zend_string_release_ex(repl, 0);
	if (NULL == res) {
		rep_cnt = -1;
		goto out;
	}

	memcpy(ms->o.buf, ZSTR_VAL(res), ZSTR_LEN(res));
	ms->o.buf[ZSTR_LEN(res)] = '\0';

	zend_string_release_ex(res, 0);

out:
	return rep_cnt;
}

file_protected file_pushbuf_t *
file_push_buffer(struct magic_set *ms)
{
	file_pushbuf_t *pb;

	if (ms->event_flags & EVENT_HAD_ERR)
		return NULL;

	if ((pb = (CAST(file_pushbuf_t *, emalloc(sizeof(*pb))))) == NULL)
		return NULL;

	pb->buf = ms->o.buf;
	pb->blen = ms->o.blen;
	pb->offset = ms->offset;

	ms->o.buf = NULL;
	ms->o.blen = 0;
	ms->offset = 0;

	return pb;
}

file_protected char *
file_pop_buffer(struct magic_set *ms, file_pushbuf_t *pb)
{
	char *rbuf;

	if (ms->event_flags & EVENT_HAD_ERR) {
		efree(pb->buf);
		efree(pb);
		return NULL;
	}

	rbuf = ms->o.buf;

	ms->o.buf = pb->buf;
	ms->o.blen = pb->blen;
	ms->offset = pb->offset;

	efree(pb);
	return rbuf;
}

/*
 * convert string to ascii printable format.
 */
file_protected char *
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

static char XDIGIT[]  = "0123456789abcdef";
static int
atox(const uint8_t c)
{
	uint8_t d = isupper(c) ? tolower(c) : c;
	const char *q = d ? strchr(XDIGIT, isupper(c) ? tolower(c) : c) : NULL;
	if (q == NULL)
		return -1;
	return q - XDIGIT;
}

static int
getxvalue(void *p, const char *s, size_t n)
{
	uint64_t v = 0;
	for (size_t i = 0; i < n; i++) {
		int x = atox(s[i]);
		if (x == -1)
			return 0;
		v = (v << 4) | x;
	}
	switch (n) {
	case 8:
		*(uint32_t *)p = v;
		return 1;
	case 4:
		*(uint16_t *)p = v;
		return 1;
	case 2:
		*(uint8_t *)p = v;
		return 1;
	default:
		return 0;
	}
}

file_protected int
file_parse_guid(const char *s, uint64_t *guid)
{
	struct guid *g = CAST(struct guid *, CAST(void *, guid));

	if (!getxvalue(&g->data1, s, 8) || s[8] != '-')
		return -1;
	s += 9;
	if (!getxvalue(&g->data2, s, 4) || s[4] != '-')
		return -1;
	s += 5;
	if (!getxvalue(&g->data3, s, 4) || s[4] != '-')
		return -1;
	s += 5;
	if (!getxvalue(&g->data4[0], s, 2) ||
	    !getxvalue(&g->data4[1], s + 2, 2) || s[4] != '-')
		return -1;
	s += 5;
	if (!getxvalue(&g->data4[2], s, 2) ||
	    !getxvalue(&g->data4[3], s + 2, 2) ||
	    !getxvalue(&g->data4[4], s + 4, 2) ||
	    !getxvalue(&g->data4[5], s + 6, 2) ||
	    !getxvalue(&g->data4[6], s + 8, 2) ||
	    !getxvalue(&g->data4[7], s + 10, 2))
		return -1;

	if (file_bigendian()) {
		g->data1 = file_swap4(g->data1);
		g->data2 = file_swap2(g->data2);
		g->data3 = file_swap2(g->data3);

	}
	return 0;
}

file_private int
file_print_guid(char *str, size_t len, const struct guid *g)
{
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

file_protected int
file_print_leguid(char *str, size_t len, const uint64_t *guid)
{
	const struct guid *g = CAST(const struct guid *,
	    CAST(const void *, guid));
	return file_print_guid(str, len, g);
}

file_protected int
file_print_beguid(char *str, size_t len, const uint64_t *guid)
{
	const struct guid *g = CAST(const struct guid *,
	    CAST(const void *, guid));
	struct guid gg = *g;
	gg.data1 = file_swap4(gg.data1);
	gg.data2 = file_swap2(gg.data2);
	gg.data3 = file_swap2(gg.data3);
	return file_print_guid(str, len, &gg);
}

#if 0
file_protected int
file_pipe_closexec(int *fds)
{
#ifdef __MINGW32__
	return 0;
#elif defined(HAVE_PIPE2)
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
#endif

file_protected int
file_clear_closexec(int fd) {
#ifdef F_SETFD
	return fcntl(fd, F_SETFD, 0);
#else
	return 0;
#endif
}

file_protected char *
file_strtrim(char *str)
{
	char *last;

	while (isspace(CAST(unsigned char, *str)))
		str++;
	last = str;
	while (*last)
		last++;
	if (last == str)
		return str;
	--last;
	while (isspace(CAST(unsigned char, *last)))
		last--;
	*++last = '\0';
	return str;
}
