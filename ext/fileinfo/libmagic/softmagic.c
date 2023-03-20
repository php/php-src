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
 * softmagic - interpret variable magic from MAGIC
 */

#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: softmagic.c,v 1.328 2022/09/13 18:46:07 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "der.h"

private int match(struct magic_set *, struct magic *, size_t,
    const struct buffer *, size_t, int, int, int, uint16_t *,
    uint16_t *, int *, int *, int *, int *);
private int mget(struct magic_set *, struct magic *, const struct buffer *,
    const unsigned char *, size_t,
    size_t, unsigned int, int, int, int, uint16_t *,
    uint16_t *, int *, int *, int *, int *);
private int msetoffset(struct magic_set *, struct magic *, struct buffer *,
    const struct buffer *, size_t, unsigned int);
private int magiccheck(struct magic_set *, struct magic *);
private int mprint(struct magic_set *, struct magic *);
private int moffset(struct magic_set *, struct magic *, const struct buffer *,
    int32_t *);
private void mdebug(uint32_t, const char *, size_t);
private int mcopy(struct magic_set *, union VALUETYPE *, int, int,
    const unsigned char *, uint32_t, size_t, struct magic *);
private int mconvert(struct magic_set *, struct magic *, int);
private int print_sep(struct magic_set *, int);
private int handle_annotation(struct magic_set *, struct magic *, int);
private int cvt_8(union VALUETYPE *, const struct magic *);
private int cvt_16(union VALUETYPE *, const struct magic *);
private int cvt_32(union VALUETYPE *, const struct magic *);
private int cvt_64(union VALUETYPE *, const struct magic *);

#define OFFSET_OOB(n, o, i)	((n) < CAST(uint32_t, (o)) || (i) > ((n) - (o)))
#define BE64(p) ( \
    (CAST(uint64_t, (p)->hq[0])<<56)| \
    (CAST(uint64_t, (p)->hq[1])<<48)| \
    (CAST(uint64_t, (p)->hq[2])<<40)| \
    (CAST(uint64_t, (p)->hq[3])<<32)| \
    (CAST(uint64_t, (p)->hq[4])<<24)| \
    (CAST(uint64_t, (p)->hq[5])<<16)| \
    (CAST(uint64_t, (p)->hq[6])<<8)| \
    (CAST(uint64_t, (p)->hq[7])))
#define LE64(p) ( \
    (CAST(uint64_t, (p)->hq[7])<<56)| \
    (CAST(uint64_t, (p)->hq[6])<<48)| \
    (CAST(uint64_t, (p)->hq[5])<<40)| \
    (CAST(uint64_t, (p)->hq[4])<<32)| \
    (CAST(uint64_t, (p)->hq[3])<<24)| \
    (CAST(uint64_t, (p)->hq[2])<<16)| \
    (CAST(uint64_t, (p)->hq[1])<<8)| \
    (CAST(uint64_t, (p)->hq[0])))
#define LE32(p) ( \
    (CAST(uint32_t, (p)->hl[3])<<24)| \
    (CAST(uint32_t, (p)->hl[2])<<16)| \
    (CAST(uint32_t, (p)->hl[1])<<8)| \
    (CAST(uint32_t, (p)->hl[0])))
#define BE32(p) ( \
    (CAST(uint32_t, (p)->hl[0])<<24)| \
    (CAST(uint32_t, (p)->hl[1])<<16)| \
    (CAST(uint32_t, (p)->hl[2])<<8)| \
    (CAST(uint32_t, (p)->hl[3])))
#define ME32(p) ( \
    (CAST(uint32_t, (p)->hl[1])<<24)| \
    (CAST(uint32_t, (p)->hl[0])<<16)| \
    (CAST(uint32_t, (p)->hl[3])<<8)| \
    (CAST(uint32_t, (p)->hl[2])))

#define BE16(p) ((CAST(uint16_t, (p)->hs[0])<<8)|(CAST(uint16_t, (p)->hs[1])))
#define LE16(p) ((CAST(uint16_t, (p)->hs[1])<<8)|(CAST(uint16_t, (p)->hs[0])))
#define SEXT(s,v,p) ((s) ? \
	CAST(intmax_t, CAST(int##v##_t, p)) : \
	CAST(intmax_t, CAST(uint##v##_t, p)))

/*
 * softmagic - lookup one file in parsed, in-memory copy of database
 * Passed the name and FILE * of one file to be typed.
 */
/*ARGSUSED1*/		/* nbytes passed for regularity, maybe need later */
protected int
file_softmagic(struct magic_set *ms, const struct buffer *b,
    uint16_t *indir_count, uint16_t *name_count, int mode, int text)
{
	struct mlist *ml;
	int rv = 0, printed_something = 0, need_separator = 0;
	uint16_t nc, ic;

	if (name_count == NULL) {
		nc = 0;
		name_count = &nc;
	}
	if (indir_count == NULL) {
		ic = 0;
		indir_count = &ic;
	}

	for (ml = ms->mlist[0]->next; ml != ms->mlist[0]; ml = ml->next) {
		int ret = match(ms, ml->magic, ml->nmagic, b,
		    0, mode, text, 0, indir_count, name_count,
		    &printed_something, &need_separator, NULL, NULL);
		switch (ret) {
		case -1:
			return ret;
		case 0:
			continue;
		default:
			if ((ms->flags & MAGIC_CONTINUE) == 0)
				return ret;
			rv = ret;
			break;
		}
	}

	return rv;
}


#if defined(FILE_FMTDEBUG) && defined(HAVE_FMTCHECK)
#define F(a, b, c) file_fmtcheck((a), (b), (c), __FILE__, __LINE__)

private const char * __attribute__((__format_arg__(3)))
file_fmtcheck(struct magic_set *ms, const char *desc, const char *def,
	const char *file, size_t line)
{
	const char *ptr;

	if (strchr(desc, '%') == NULL)
		return desc;

	ptr = fmtcheck(desc, def);
	if (ptr == def)
		file_magerror(ms,
		    "%s, %" SIZE_T_FORMAT "u: format `%s' does not match"
		    " with `%s'", file, line, desc, def);
	return ptr;
}
#elif defined(HAVE_FMTCHECK)
#define F(a, b, c) fmtcheck((b), (c))
#else
#define F(a, b, c) ((b))
#endif

/* NOTE this function has been kept an the state of 5.39 for BC. Observe
 * further as the upgrade to 5.41 or above goes. */
/*
 * Go through the whole list, stopping if you find a match.  Process all
 * the continuations of that match before returning.
 *
 * We support multi-level continuations:
 *
 *	At any time when processing a successful top-level match, there is a
 *	current continuation level; it represents the level of the last
 *	successfully matched continuation.
 *
 *	Continuations above that level are skipped as, if we see one, it
 *	means that the continuation that controls them - i.e, the
 *	lower-level continuation preceding them - failed to match.
 *
 *	Continuations below that level are processed as, if we see one,
 *	it means we've finished processing or skipping higher-level
 *	continuations under the control of a successful or unsuccessful
 *	lower-level continuation, and are now seeing the next lower-level
 *	continuation and should process it.  The current continuation
 *	level reverts to the level of the one we're seeing.
 *
 *	Continuations at the current level are processed as, if we see
 *	one, there's no lower-level continuation that may have failed.
 *
 *	If a continuation matches, we bump the current continuation level
 *	so that higher-level continuations are processed.
 */
private int
match(struct magic_set *ms, struct magic *magic,
    size_t nmagic, const struct buffer *b, size_t offset, int mode, int text,
    int flip, uint16_t *indir_count, uint16_t *name_count,
    int *printed_something, int *need_separator, int *returnval,
    int *found_match)
{
	uint32_t magindex = 0;
	unsigned int cont_level = 0;
	int found_matchv = 0; /* if a match is found it is set to 1*/
	int returnvalv = 0, e;
	/* a flag to print X\n  X\n- X */
	int firstline = !(*printed_something || *need_separator);
	struct buffer bb;
	int print = (ms->flags & MAGIC_NODESC) == 0;

	/*
	 * returnval can be 0 if a match is found, but there was no
	 * annotation to be printed.
	 */
	if (returnval == NULL)
		returnval = &returnvalv;
	if (found_match == NULL)
		found_match = &found_matchv;

	if (file_check_mem(ms, cont_level) == -1)
		return -1;

	for (magindex = 0; magindex < nmagic; magindex++) {
		int flush = 0;
		struct magic *m = &magic[magindex];

		if (m->type != FILE_NAME)
		if ((IS_LIBMAGIC_STRING(m->type) &&
#define FLT (STRING_BINTEST | STRING_TEXTTEST)
		     ((text && (m->str_flags & FLT) == STRING_BINTEST) ||
		      (!text && (m->str_flags & FLT) == STRING_TEXTTEST))) ||
		    (m->flag & mode) != mode) {
flush:
			/* Skip sub-tests */
			while (magindex < nmagic - 1 &&
			    magic[magindex + 1].cont_level != 0)
				magindex++;
			cont_level = 0;
			continue; /* Skip to next top-level test*/
		}

		if (msetoffset(ms, m, &bb, b, offset, cont_level) == -1)
			goto flush;
		ms->line = m->lineno;

		/* if main entry matches, print it... */
		switch (mget(ms, m, b, CAST(const unsigned char *, bb.fbuf),
		    bb.flen, offset, cont_level,
		    mode, text, flip, indir_count, name_count,
		    printed_something, need_separator, returnval, found_match))
		{
		case -1:
			return -1;
		case 0:
			flush = m->reln != '!';
			break;
		default:
			if (m->type == FILE_INDIRECT) {
				*found_match = 1;
				*returnval = 1;
			}

			switch (magiccheck(ms, m)) {
			case -1:
				return -1;
			case 0:
				flush++;
				break;
			default:
				flush = 0;
				break;
			}
			break;
		}
		if (flush) {
			/*
			 * main entry didn't match,
			 * flush its continuations
			 */
			goto flush;
		}

		if ((e = handle_annotation(ms, m, firstline)) != 0)
		{
			*found_match = 1;
			*need_separator = 1;
			*printed_something = 1;
			*returnval = 1;
			return e;
		}

		/*
		 * If we are going to print something, we'll need to print
		 * a blank before we print something else.
		 */
		if (*m->desc) {
			*found_match = 1;
			if (print) {
				*returnval = 1;
				*need_separator = 1;
				*printed_something = 1;
				if (print_sep(ms, firstline) == -1)
					return -1;
				if (mprint(ms, m) == -1)
					return -1;
			}
		}

		switch (moffset(ms, m, &bb, &ms->c.li[cont_level].off)) {
		case -1:
		case 0:
			goto flush;
		default:
			break;
		}

		/* and any continuations that match */
		if (file_check_mem(ms, ++cont_level) == -1)
			return -1;

		while (magindex + 1 < nmagic &&
		    magic[magindex + 1].cont_level != 0) {
			m = &magic[++magindex];
			ms->line = m->lineno; /* for messages */

			if (cont_level < m->cont_level)
				continue;
			if (cont_level > m->cont_level) {
				/*
				 * We're at the end of the level
				 * "cont_level" continuations.
				 */
				cont_level = m->cont_level;
			}
			if (msetoffset(ms, m, &bb, b, offset, cont_level) == -1)
				goto flush;
			if (m->flag & OFFADD) {
				if (cont_level == 0) {
					if ((ms->flags & MAGIC_DEBUG) != 0)
						fprintf(stderr,
						    "direct *zero*"
						    " cont_level\n");
					return 0;
				}
				ms->offset +=
				    ms->c.li[cont_level - 1].off;
			}

#ifdef ENABLE_CONDITIONALS
			if (m->cond == COND_ELSE ||
			    m->cond == COND_ELIF) {
				if (ms->c.li[cont_level].last_match == 1)
					continue;
			}
#endif
			switch (mget(ms, m, b, CAST(const unsigned char *,
			    bb.fbuf), bb.flen, offset,
			    cont_level, mode, text, flip, indir_count,
			    name_count, printed_something, need_separator,
			    returnval, found_match)) {
			case -1:
				return -1;
			case 0:
				if (m->reln != '!')
					continue;
				flush = 1;
				break;
			default:
				if (m->type == FILE_INDIRECT) {
					*found_match = 1;
					*returnval = 1;
				}
				flush = 0;
				break;
			}

			switch (flush ? 1 : magiccheck(ms, m)) {
			case -1:
				return -1;
			case 0:
#ifdef ENABLE_CONDITIONALS
				ms->c.li[cont_level].last_match = 0;
#endif
				break;
			default:
#ifdef ENABLE_CONDITIONALS
				ms->c.li[cont_level].last_match = 1;
#endif
				if (m->type == FILE_CLEAR)
					ms->c.li[cont_level].got_match = 0;
				else if (ms->c.li[cont_level].got_match) {
					if (m->type == FILE_DEFAULT)
						break;
				} else
					ms->c.li[cont_level].got_match = 1;

				if ((e = handle_annotation(ms, m, firstline))
				    != 0) {
					*found_match = 1;
					*need_separator = 1;
					*printed_something = 1;
					*returnval = 1;
					return e;
				}
				if (*m->desc) {
					*found_match = 1;
				}
				if (print && *m->desc) {
					*returnval = 1;
					/*
					 * This continuation matched.  Print
					 * its message, with a blank before it
					 * if the previous item printed and
					 * this item isn't empty.
					 */
					/*
					 * If we are going to print something,
					 * make sure that we have a separator
					 * first.
					 */
					if (!*printed_something) {
						*printed_something = 1;
						if (print_sep(ms, firstline)
						    == -1)
							return -1;
					}
					/* space if previous printed */
					if (*need_separator
					    && (m->flag & NOSPACE) == 0) {
						if (file_printf(ms, " ") == -1)
							return -1;
					}
					if (mprint(ms, m) == -1)
						return -1;
					*need_separator = 1;
				}

				switch (moffset(ms, m, &bb,
				    &ms->c.li[cont_level].off)) {
				case -1:
				case 0:
					cont_level--;
					break;
				default:
					break;
				}

				/*
				 * If we see any continuations
				 * at a higher level,
				 * process them.
				 */
				if (file_check_mem(ms, ++cont_level) == -1)
					return -1;
				break;
			}
		}
		if (*printed_something) {
			firstline = 0;
		}
		if (*found_match) {
			if ((ms->flags & MAGIC_CONTINUE) == 0)
				return *returnval;
			// So that we print a separator
			*printed_something = 0;
			firstline = 0;
		}
		cont_level = 0;
	}
	return *returnval;
}

private int
check_fmt(struct magic_set *ms, const char *fmt)
{
	pcre_cache_entry *pce;
	int rv = -1;
	zend_string *pattern;

	if (strchr(fmt, '%') == NULL)
		return 0;

	pattern = ZSTR_INIT_LITERAL("~%[-0-9\\.]*s~", 0);
	if ((pce = pcre_get_compiled_regex_cache_ex(pattern, 0)) == NULL) {
		rv = -1;
	} else {
		pcre2_code *re = php_pcre_pce_re(pce);
		pcre2_match_data *match_data = php_pcre_create_match_data(0, re);
		if (match_data) {
			rv = pcre2_match(re, (PCRE2_SPTR)fmt, strlen(fmt), 0, 0, match_data, php_pcre_mctx()) > 0;
			php_pcre_free_match_data(match_data);
		}
	}
	zend_string_release(pattern);
	return rv;
}

#if !defined(HAVE_STRNDUP) || defined(__aiws__) || defined(_AIX)
# if defined(__aiws__) || defined(_AIX)
#  define strndup aix_strndup	/* aix is broken */
# endif
char *strndup(const char *, size_t);

char *
strndup(const char *str, size_t n)
{
	size_t len;
	char *copy;

	for (len = 0; len < n && str[len]; len++)
		continue;
	if ((copy = CAST(char *, emalloc(len + 1))) == NULL)
		return NULL;
	(void)memcpy(copy, str, len);
	copy[len] = '\0';
	return copy;
}
#endif /* HAVE_STRNDUP */

static int
varexpand(struct magic_set *ms, char *buf, size_t len, const char *str)
{
	const char *ptr, *sptr, *e, *t, *ee, *et;
	size_t l;

	for (sptr = str; (ptr = strstr(sptr, "${")) != NULL;) {
		l = CAST(size_t, ptr - sptr);
		if (l >= len)
			return -1;
		memcpy(buf, sptr, l);
		buf += l;
		len -= l;
		ptr += 2;
		if (!*ptr || ptr[1] != '?')
			return -1;
		for (et = t = ptr + 2; *et && *et != ':'; et++)
			continue;
		if (*et != ':')
			return -1;
		for (ee = e = et + 1; *ee && *ee != '}'; ee++)
			continue;
		if (*ee != '}')
			return -1;
		switch (*ptr) {
		case 'x':
			if (ms->mode & 0111) {
				ptr = t;
				l = et - t;
			} else {
				ptr = e;
				l = ee - e;
			}
			break;
		default:
			return -1;
		}
		if (l >= len)
			return -1;
		memcpy(buf, ptr, l);
		buf += l;
		len -= l;
		sptr = ee + 1;
	}

	l = strlen(sptr);
	if (l >= len)
		return -1;

	memcpy(buf, sptr, l);
	buf[l] = '\0';
	return 0;
}


private int
mprint(struct magic_set *ms, struct magic *m)
{
	uint64_t v;
	float vf;
	double vd;
 	char buf[128], tbuf[26], sbuf[512], ebuf[512];
	const char *desc;
	union VALUETYPE *p = &ms->ms_value;

	if (varexpand(ms, ebuf, sizeof(ebuf), m->desc) == -1)
		desc = m->desc;
	else
		desc = ebuf;

#define	PRINTER(value, format, stype, utype)	\
	v = file_signextend(ms, m, CAST(uint64_t, value)); \
	switch (check_fmt(ms, desc)) { \
	case -1: \
		return -1; \
	case 1: \
		if (m->flag & UNSIGNED) { \
			(void)snprintf(buf, sizeof(buf), "%" format "u", \
			    CAST(utype, v)); \
		} else { \
			(void)snprintf(buf, sizeof(buf), "%" format "d", \
			    CAST(stype, v)); \
		} \
		if (file_printf(ms, F(ms, desc, "%s"), buf) == -1) \
			return -1; \
		break; \
	default: \
		if (m->flag & UNSIGNED) { \
		       if (file_printf(ms, F(ms, desc, "%" format "u"), \
			   CAST(utype, v)) == -1) \
			   return -1; \
		} else { \
		       if (file_printf(ms, F(ms, desc, "%" format "d"), \
			   CAST(stype, v)) == -1) \
			   return -1; \
		} \
		break; \
	} \
	break

  	switch (m->type) {
  	case FILE_BYTE:
		PRINTER(p->b, "", int8_t, uint8_t);

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		PRINTER(p->h, "", int16_t, uint16_t);

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		PRINTER(p->l, "", int32_t, uint32_t);

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
	case FILE_OFFSET:
		PRINTER(p->q, INT64_T_FORMAT, long long, unsigned long long);

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!') {
			if (file_printf(ms, F(ms, desc, "%s"),
			    file_printable(ms, sbuf, sizeof(sbuf), m->value.s,
			    sizeof(m->value.s))) == -1)
				return -1;
		}
		else {
			char *str = p->s;

			/* compute t before we mangle the string? */

			if (*m->value.s == '\0')
				str[strcspn(str, "\r\n")] = '\0';

			if (m->str_flags & STRING_TRIM)
				str = file_strtrim(str);

			if (file_printf(ms, F(ms, desc, "%s"),
			    file_printable(ms, sbuf, sizeof(sbuf), str,
				sizeof(p->s) - (str - p->s))) == -1)
				return -1;

			if (m->type == FILE_PSTRING) {
				size_t l = file_pstring_length_size(ms, m);
				if (l == FILE_BADSIZE)
					return -1;
			}
		}
		break;

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdatetime(tbuf, sizeof(tbuf), p->l, 0)) == -1)
			return -1;
		break;

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdatetime(tbuf, sizeof(tbuf), p->l, FILE_T_LOCAL))
			== -1)
			return -1;
		break;

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdatetime(tbuf, sizeof(tbuf), p->q, 0)) == -1)
			return -1;
		break;

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdatetime(tbuf, sizeof(tbuf), p->q, FILE_T_LOCAL)) == -1)
			return -1;
		break;

	case FILE_QWDATE:
	case FILE_BEQWDATE:
	case FILE_LEQWDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdatetime(tbuf, sizeof(tbuf), p->q, FILE_T_WINDOWS))
		    == -1)
			return -1;
		break;

	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		vf = p->f;
		switch (check_fmt(ms, desc)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vf);
			if (file_printf(ms, F(ms, desc, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, desc, "%g"), vf) == -1)
				return -1;
			break;
		}
  		break;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		vd = p->d;
		switch (check_fmt(ms, desc)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vd);
			if (file_printf(ms, F(ms, desc, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, desc, "%g"), vd) == -1)
				return -1;
			break;
		}
  		break;

	case FILE_SEARCH:
	case FILE_REGEX: {
		char *cp, *scp;
		int rval;

		cp = estrndup(RCAST(const char *, ms->search.s),
		    ms->search.rm_len);
		if (cp == NULL) {
			file_oomem(ms, ms->search.rm_len);
			return -1;
		}
		scp = (m->str_flags & STRING_TRIM) ? file_strtrim(cp) : cp;

		rval = file_printf(ms, F(ms, desc, "%s"), file_printable(ms,
		    sbuf, sizeof(sbuf), scp, ms->search.rm_len));
		efree(cp);

		if (rval == -1)
			return -1;
		break;
	}

	case FILE_DEFAULT:
	case FILE_CLEAR:
	  	if (file_printf(ms, "%s", m->desc) == -1)
			return -1;
		break;

	case FILE_INDIRECT:
	case FILE_USE:
	case FILE_NAME:
		break;
	case FILE_DER:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_printable(ms, sbuf, sizeof(sbuf), ms->ms_value.s,
			sizeof(ms->ms_value.s))) == -1)
			return -1;
		break;
	case FILE_GUID:
		(void) file_print_guid(buf, sizeof(buf), ms->ms_value.guid);
		if (file_printf(ms, F(ms, desc, "%s"), buf) == -1)
			return -1;
		break;
	case FILE_MSDOSDATE:
	case FILE_BEMSDOSDATE:
	case FILE_LEMSDOSDATE:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmtdate(tbuf, sizeof(tbuf), p->h)) == -1)
			return -1;
		break;
	case FILE_MSDOSTIME:
	case FILE_BEMSDOSTIME:
	case FILE_LEMSDOSTIME:
		if (file_printf(ms, F(ms, desc, "%s"),
		    file_fmttime(tbuf, sizeof(tbuf), p->h)) == -1)
			return -1;
		break;
	case FILE_OCTAL:
		file_fmtnum(buf, sizeof(buf), m->value.s, 8);
		if (file_printf(ms, F(ms, desc, "%s"), buf) == -1)
			return -1;
		break;
	default:
		file_magerror(ms, "invalid m->type (%d) in mprint()", m->type);
		return -1;
	}
	return 0;
}

private int
moffset(struct magic_set *ms, struct magic *m, const struct buffer *b,
    int32_t *op)
{
	size_t nbytes = b->flen;
	int32_t o;

  	switch (m->type) {
  	case FILE_BYTE:
		o = CAST(int32_t, (ms->offset + sizeof(char)));
		break;

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
	case FILE_MSDOSDATE:
	case FILE_LEMSDOSDATE:
	case FILE_BEMSDOSDATE:
	case FILE_MSDOSTIME:
	case FILE_LEMSDOSTIME:
	case FILE_BEMSDOSTIME:
		o = CAST(int32_t, (ms->offset + sizeof(short)));
		break;

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		o = CAST(int32_t, (ms->offset + sizeof(int32_t)));
		break;

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		o = CAST(int32_t, (ms->offset + sizeof(int64_t)));
		break;

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
	case FILE_OCTAL:
		if (m->reln == '=' || m->reln == '!') {
			o = ms->offset + m->vallen;
		} else {
			union VALUETYPE *p = &ms->ms_value;

			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\r\n")] = '\0';
			o = CAST(uint32_t, (ms->offset + strlen(p->s)));
			if (m->type == FILE_PSTRING) {
				size_t l = file_pstring_length_size(ms, m);
				if (l == FILE_BADSIZE)
					return -1;
				o += CAST(uint32_t, l);
			}
		}
		break;

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint32_t)));
		break;

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint32_t)));
		break;

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint64_t)));
		break;

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint64_t)));
		break;

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		o = CAST(int32_t, (ms->offset + sizeof(float)));
		break;

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		o = CAST(int32_t, (ms->offset + sizeof(double)));
		break;

	case FILE_REGEX:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			o = CAST(int32_t, ms->search.offset);
		else
			o = CAST(int32_t,
			    (ms->search.offset + ms->search.rm_len));
		break;

	case FILE_SEARCH:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			o = CAST(int32_t, ms->search.offset);
		else
			o = CAST(int32_t, (ms->search.offset + m->vallen));
		break;

	case FILE_CLEAR:
	case FILE_DEFAULT:
	case FILE_INDIRECT:
	case FILE_OFFSET:
	case FILE_USE:
		o = ms->offset;
		break;

	case FILE_DER:
		o = der_offs(ms, m, nbytes);
		if (o == -1 || CAST(size_t, o) > nbytes) {
			if ((ms->flags & MAGIC_DEBUG) != 0) {
				(void)fprintf(stderr,
				    "Bad DER offset %d nbytes=%"
				    SIZE_T_FORMAT "u", o, nbytes);
			}
			*op = 0;
			return 0;
		}
		break;

	case FILE_GUID:
		o = CAST(int32_t, (ms->offset + 2 * sizeof(uint64_t)));
		break;

	default:
		o = 0;
		break;
	}

	if (CAST(size_t, o) > nbytes) {
#if 0
		file_error(ms, 0, "Offset out of range %" SIZE_T_FORMAT
		    "u > %" SIZE_T_FORMAT "u", (size_t)o, nbytes);
#endif
		return -1;
	}
	*op = o;
	return 1;
}

private uint32_t
cvt_id3(struct magic_set *ms, uint32_t v)
{
	v = ((((v >>  0) & 0x7f) <<  0) |
	     (((v >>  8) & 0x7f) <<  7) |
	     (((v >> 16) & 0x7f) << 14) |
	     (((v >> 24) & 0x7f) << 21));
	if ((ms->flags & MAGIC_DEBUG) != 0)
		fprintf(stderr, "id3 offs=%u\n", v);
	return v;
}

private int
cvt_flip(int type, int flip)
{
	if (flip == 0)
		return type;
	switch (type) {
	case FILE_BESHORT:
		return FILE_LESHORT;
	case FILE_BELONG:
		return FILE_LELONG;
	case FILE_BEDATE:
		return FILE_LEDATE;
	case FILE_BELDATE:
		return FILE_LELDATE;
	case FILE_BEQUAD:
		return FILE_LEQUAD;
	case FILE_BEQDATE:
		return FILE_LEQDATE;
	case FILE_BEQLDATE:
		return FILE_LEQLDATE;
	case FILE_BEQWDATE:
		return FILE_LEQWDATE;
	case FILE_LESHORT:
		return FILE_BESHORT;
	case FILE_LELONG:
		return FILE_BELONG;
	case FILE_LEDATE:
		return FILE_BEDATE;
	case FILE_LELDATE:
		return FILE_BELDATE;
	case FILE_LEQUAD:
		return FILE_BEQUAD;
	case FILE_LEQDATE:
		return FILE_BEQDATE;
	case FILE_LEQLDATE:
		return FILE_BEQLDATE;
	case FILE_LEQWDATE:
		return FILE_BEQWDATE;
	case FILE_BEFLOAT:
		return FILE_LEFLOAT;
	case FILE_LEFLOAT:
		return FILE_BEFLOAT;
	case FILE_BEDOUBLE:
		return FILE_LEDOUBLE;
	case FILE_LEDOUBLE:
		return FILE_BEDOUBLE;
	default:
		return type;
	}
}
#define DO_CVT(fld, type) \
	if (m->num_mask) \
		switch (m->mask_op & FILE_OPS_MASK) { \
		case FILE_OPAND: \
			p->fld &= CAST(type, m->num_mask); \
			break; \
		case FILE_OPOR: \
			p->fld |= CAST(type, m->num_mask); \
			break; \
		case FILE_OPXOR: \
			p->fld ^= CAST(type, m->num_mask); \
			break; \
		case FILE_OPADD: \
			p->fld += CAST(type, m->num_mask); \
			break; \
		case FILE_OPMINUS: \
			p->fld -= CAST(type, m->num_mask); \
			break; \
		case FILE_OPMULTIPLY: \
			p->fld *= CAST(type, m->num_mask); \
			break; \
		case FILE_OPDIVIDE: \
			if (CAST(type, m->num_mask) == 0) \
				return -1; \
			p->fld /= CAST(type, m->num_mask); \
			break; \
		case FILE_OPMODULO: \
			if (CAST(type, m->num_mask) == 0) \
				return -1; \
			p->fld %= CAST(type, m->num_mask); \
			break; \
		} \
	if (m->mask_op & FILE_OPINVERSE) \
		p->fld = ~p->fld \

private int
cvt_8(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(b, uint8_t);
	return 0;
}

private int
cvt_16(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(h, uint16_t);
	return 0;
}

private int
cvt_32(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(l, uint32_t);
	return 0;
}

private int
cvt_64(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(q, uint64_t);
	return 0;
}

#define DO_CVT2(fld, type) \
	if (m->num_mask) \
		switch (m->mask_op & FILE_OPS_MASK) { \
		case FILE_OPADD: \
			p->fld += CAST(type, m->num_mask); \
			break; \
		case FILE_OPMINUS: \
			p->fld -= CAST(type, m->num_mask); \
			break; \
		case FILE_OPMULTIPLY: \
			p->fld *= CAST(type, m->num_mask); \
			break; \
		case FILE_OPDIVIDE: \
			if (CAST(type, m->num_mask) == 0) \
				return -1; \
			p->fld /= CAST(type, m->num_mask); \
			break; \
		} \

private int
cvt_float(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT2(f, float);
	return 0;
}

private int
cvt_double(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT2(d, double);
	return 0;
}

/*
 * Convert the byte order of the data we are looking at
 * While we're here, let's apply the mask operation
 * (unless you have a better idea)
 */
private int
mconvert(struct magic_set *ms, struct magic *m, int flip)
{
	union VALUETYPE *p = &ms->ms_value;

	switch (cvt_flip(m->type, flip)) {
	case FILE_BYTE:
		if (cvt_8(p, m) == -1)
			goto out;
		return 1;
	case FILE_SHORT:
	case FILE_MSDOSDATE:
	case FILE_LEMSDOSDATE:
	case FILE_BEMSDOSDATE:
	case FILE_MSDOSTIME:
	case FILE_LEMSDOSTIME:
	case FILE_BEMSDOSTIME:
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_LONG:
	case FILE_DATE:
	case FILE_LDATE:
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_QUAD:
	case FILE_QDATE:
	case FILE_QLDATE:
	case FILE_QWDATE:
	case FILE_OFFSET:
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_STRING:
	case FILE_BESTRING16:
	case FILE_LESTRING16:
	case FILE_OCTAL: {
		/* Null terminate and eat *trailing* return */
		p->s[sizeof(p->s) - 1] = '\0';
		return 1;
	}
	case FILE_PSTRING: {
		char *ptr1, *ptr2;
		size_t len, sz = file_pstring_length_size(ms, m);
		if (sz == FILE_BADSIZE)
			return 0;
		ptr1 = p->s;
		ptr2 = ptr1 + sz;
		len = file_pstring_get_length(ms, m, ptr1);
		if (len == FILE_BADSIZE)
			return 0;
		sz = sizeof(p->s) - sz; /* maximum length of string */
		if (len >= sz) {
			/*
			 * The size of the pascal string length (sz)
			 * is 1, 2, or 4. We need at least 1 byte for NUL
			 * termination, but we've already truncated the
			 * string by p->s, so we need to deduct sz.
			 * Because we can use one of the bytes of the length
			 * after we shifted as NUL termination.
			 */
			len = sz;
		}
		while (len--)
			*ptr1++ = *ptr2++;
		*ptr1 = '\0';
		return 1;
	}
	case FILE_BESHORT:
		p->h = CAST(short, BE16(p));
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_BELONG:
	case FILE_BEDATE:
	case FILE_BELDATE:
		p->l = CAST(int32_t, BE32(p));
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEQUAD:
	case FILE_BEQDATE:
	case FILE_BEQLDATE:
	case FILE_BEQWDATE:
		p->q = CAST(uint64_t, BE64(p));
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_LESHORT:
		p->h = CAST(short, LE16(p));
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_LELONG:
	case FILE_LEDATE:
	case FILE_LELDATE:
		p->l = CAST(int32_t, LE32(p));
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEQUAD:
	case FILE_LEQDATE:
	case FILE_LEQLDATE:
	case FILE_LEQWDATE:
		p->q = CAST(uint64_t, LE64(p));
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_MELONG:
	case FILE_MEDATE:
	case FILE_MELDATE:
		p->l = CAST(int32_t, ME32(p));
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_FLOAT:
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEFLOAT:
		p->l = BE32(p);
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEFLOAT:
		p->l = LE32(p);
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_DOUBLE:
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEDOUBLE:
		p->q = BE64(p);
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEDOUBLE:
		p->q = LE64(p);
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_REGEX:
	case FILE_SEARCH:
	case FILE_DEFAULT:
	case FILE_CLEAR:
	case FILE_NAME:
	case FILE_USE:
	case FILE_DER:
	case FILE_GUID:
		return 1;
	default:
		file_magerror(ms, "invalid type %d in mconvert()", m->type);
		return 0;
	}
out:
	file_magerror(ms, "zerodivide in mconvert()");
	return 0;
}


private void
mdebug(uint32_t offset, const char *str, size_t len)
{
	(void) fprintf(stderr, "mget/%" SIZE_T_FORMAT "u @%d: ", len, offset);
	file_showstr(stderr, str, len);
	(void) fputc('\n', stderr);
	(void) fputc('\n', stderr);
}

private int
mcopy(struct magic_set *ms, union VALUETYPE *p, int type, int indir,
    const unsigned char *s, uint32_t offset, size_t nbytes, struct magic *m)
{
	/*
	 * Note: FILE_SEARCH and FILE_REGEX do not actually copy
	 * anything, but setup pointers into the source
	 */
	if (indir == 0) {
		switch (type) {
		case FILE_DER:
		case FILE_SEARCH:
			if (offset > nbytes)
				offset = CAST(uint32_t, nbytes);
			ms->search.s = RCAST(const char *, s) + offset;
			ms->search.s_len = nbytes - offset;
			ms->search.offset = offset;
			return 0;

		case FILE_REGEX: {
			const char *b;
			const char *c;
			const char *last;	/* end of search region */
			const char *buf;	/* start of search region */
			const char *end;
			size_t lines, linecnt, bytecnt;

			if (s == NULL || nbytes < offset) {
				ms->search.s_len = 0;
				ms->search.s = NULL;
				return 0;
			}

			if (m->str_flags & REGEX_LINE_COUNT) {
				linecnt = m->str_range;
				bytecnt = linecnt * 80;
			} else {
				linecnt = 0;
				bytecnt = m->str_range;
			}

			if (bytecnt == 0 || bytecnt > nbytes - offset)
				bytecnt = nbytes - offset;
			if (bytecnt > ms->regex_max)
				bytecnt = ms->regex_max;

			buf = RCAST(const char *, s) + offset;
			end = last = RCAST(const char *, s) + bytecnt + offset;
			/* mget() guarantees buf <= last */
			for (lines = linecnt, b = buf; lines && b < end &&
			     ((b = CAST(const char *,
				 memchr(c = b, '\n', CAST(size_t, (end - b)))))
			     || (b = CAST(const char *,
				 memchr(c, '\r', CAST(size_t, (end - c))))));
			     lines--, b++) {
				if (b < end - 1 && b[0] == '\r' && b[1] == '\n')
					b++;
				if (b < end - 1 && b[0] == '\n')
					b++;
				last = b;
			}
			if (lines)
				last = end;

			ms->search.s = buf;
			ms->search.s_len = last - buf;
			ms->search.offset = offset;
			ms->search.rm_len = 0;
			return 0;
		}
		case FILE_BESTRING16:
		case FILE_LESTRING16: {
			const unsigned char *src = s + offset;
			const unsigned char *esrc = s + nbytes;
			char *dst = p->s;
			char *edst = &p->s[sizeof(p->s) - 1];

			if (type == FILE_BESTRING16)
				src++;

			/* check that offset is within range */
			if (offset >= nbytes)
				break;
			for (/*EMPTY*/; src < esrc; src += 2, dst++) {
				if (dst < edst)
					*dst = *src;
				else
					break;
				if (*dst == '\0') {
					if (type == FILE_BESTRING16 ?
					    *(src - 1) != '\0' :
					    ((src + 1 < esrc) &&
					    *(src + 1) != '\0'))
						*dst = ' ';
				}
			}
			*edst = '\0';
			return 0;
		}
		case FILE_STRING:	/* XXX - these two should not need */
		case FILE_PSTRING:	/* to copy anything, but do anyway. */
		default:
			break;
		}
	}

	if (type == FILE_OFFSET) {
		(void)memset(p, '\0', sizeof(*p));
		p->q = offset;
		return 0;
	}

	if (offset >= nbytes) {
		(void)memset(p, '\0', sizeof(*p));
		return 0;
	}
	if (nbytes - offset < sizeof(*p))
		nbytes = nbytes - offset;
	else
		nbytes = sizeof(*p);

	(void)memcpy(p, s + offset, nbytes);

	/*
	 * the usefulness of padding with zeroes eludes me, it
	 * might even cause problems
	 */
	if (nbytes < sizeof(*p))
		(void)memset(RCAST(char *, RCAST(void *, p)) + nbytes, '\0',
		    sizeof(*p) - nbytes);
	return 0;
}

private uint32_t
do_ops(struct magic *m, intmax_t lhs, intmax_t off)
{
	intmax_t offset;
	if (off) {
		switch (m->in_op & FILE_OPS_MASK) {
		case FILE_OPAND:
			offset = lhs & off;
			break;
		case FILE_OPOR:
			offset = lhs | off;
			break;
		case FILE_OPXOR:
			offset = lhs ^ off;
			break;
		case FILE_OPADD:
			offset = lhs + off;
			break;
		case FILE_OPMINUS:
			offset = lhs - off;
			break;
		case FILE_OPMULTIPLY:
			offset = lhs * off;
			break;
		case FILE_OPDIVIDE:
			offset = lhs / off;
			break;
		case FILE_OPMODULO:
			offset = lhs % off;
			break;
		}
	} else
		offset = lhs;
	if (m->in_op & FILE_OPINVERSE)
		offset = ~offset;

	return CAST(uint32_t, offset);
}

private int
msetoffset(struct magic_set *ms, struct magic *m, struct buffer *bb,
    const struct buffer *b, size_t o, unsigned int cont_level)
{
	int32_t offset;
	if (m->flag & OFFNEGATIVE) {
		offset = -m->offset;
		if (cont_level > 0) {
			if (m->flag & (OFFADD|INDIROFFADD))
				goto normal;
#if 0
			file_error(ms, 0, "negative offset %d at continuation"
			    "level %u", m->offset, cont_level);
			return -1;
#endif
		}
		if (buffer_fill(b) == -1)
			return -1;
		if (o != 0) {
			// Not yet!
			file_magerror(ms, "non zero offset %" SIZE_T_FORMAT
			    "u at level %u", o, cont_level);
			return -1;
		}
		if (CAST(size_t, m->offset) > b->elen)
			return -1;
		buffer_init(bb, -1, NULL, b->ebuf, b->elen);
		ms->eoffset = ms->offset = CAST(int32_t, b->elen - m->offset);
	} else {
		offset = m->offset;
		if (cont_level == 0) {
normal:
			// XXX: Pass real fd, then who frees bb?
			buffer_init(bb, -1, NULL, b->fbuf, b->flen);
			ms->offset = offset;
			ms->eoffset = 0;
		} else {
			ms->offset = ms->eoffset + offset;
		}
	}
	if ((ms->flags & MAGIC_DEBUG) != 0) {
		fprintf(stderr, "bb=[%p,%" SIZE_T_FORMAT "u,%"
		    SIZE_T_FORMAT "u], %d [b=%p,%"
		    SIZE_T_FORMAT "u,%" SIZE_T_FORMAT "u], [o=%#x, c=%d]\n",
		    bb->fbuf, bb->flen, bb->elen, ms->offset, b->fbuf,
		    b->flen, b->elen, offset, cont_level);
	}
	return 0;
}

private int
save_cont(struct magic_set *ms, struct cont *c)
{
	size_t len;
	*c = ms->c;
	len = c->len * sizeof(*c->li);
	ms->c.li = CAST(struct level_info *, emalloc(len));
	if (ms->c.li == NULL) {
		ms->c = *c;
		return -1;
	}
	memcpy(ms->c.li, c->li, len);
	return 0;
}

private void
restore_cont(struct magic_set *ms, struct cont *c)
{
	efree(ms->c.li);
	ms->c = *c;
}

private int
mget(struct magic_set *ms, struct magic *m, const struct buffer *b,
    const unsigned char *s, size_t nbytes, size_t o, unsigned int cont_level,
    int mode, int text, int flip, uint16_t *indir_count, uint16_t *name_count,
    int *printed_something, int *need_separator, int *returnval,
    int *found_match)
{
	uint32_t eoffset, offset = ms->offset;
	struct buffer bb;
	intmax_t lhs;
	file_pushbuf_t *pb;
	int rv, oneed_separator, in_type, nfound_match;
	char *rbuf;
	union VALUETYPE *p = &ms->ms_value;
	struct mlist ml, *mlp;
	struct cont c;

	if (*indir_count >= ms->indir_max) {
		file_error(ms, 0, "indirect count (%hu) exceeded",
		    *indir_count);
		return -1;
	}

	if (*name_count >= ms->name_max) {
		file_error(ms, 0, "name use count (%hu) exceeded",
		    *name_count);
		return -1;
	}



	if (mcopy(ms, p, m->type, m->flag & INDIR, s,
	    CAST(uint32_t, offset + o), CAST(uint32_t, nbytes), m) == -1)
		return -1;

	if ((ms->flags & MAGIC_DEBUG) != 0) {
		fprintf(stderr, "mget(type=%d, flag=%#x, offset=%u, o=%"
		    SIZE_T_FORMAT "u, " "nbytes=%" SIZE_T_FORMAT
		    "u, il=%hu, nc=%hu)\n",
		    m->type, m->flag, offset, o, nbytes,
		    *indir_count, *name_count);
		mdebug(offset, RCAST(char *, RCAST(void *, p)),
		    sizeof(union VALUETYPE));
#ifndef COMPILE_ONLY
		file_mdump(m);
#endif
	}

	if (m->flag & INDIR) {
		intmax_t off = m->in_offset;
		const int sgn = m->in_op & FILE_OPSIGNED;
		if (m->in_op & FILE_OPINDIRECT) {
			const union VALUETYPE *q = CAST(const union VALUETYPE *,
			    RCAST(const void *, s + offset + off));
			int op;
			switch (op = cvt_flip(m->in_type, flip)) {
			case FILE_BYTE:
				if (OFFSET_OOB(nbytes, offset + off, 1))
					return 0;
				off = SEXT(sgn,8,q->b);
				break;
			case FILE_SHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,q->h);
				break;
			case FILE_BESHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,BE16(q));
				break;
			case FILE_LESHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,LE16(q));
				break;
			case FILE_LONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,q->l);
				break;
			case FILE_BELONG:
			case FILE_BEID3:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,BE32(q));
				break;
			case FILE_LEID3:
			case FILE_LELONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,LE32(q));
				break;
			case FILE_MELONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,ME32(q));
				break;
			case FILE_BEQUAD:
				if (OFFSET_OOB(nbytes, offset + off, 8))
					return 0;
				off = SEXT(sgn,64,BE64(q));
				break;
			case FILE_LEQUAD:
				if (OFFSET_OOB(nbytes, offset + off, 8))
					return 0;
				off = SEXT(sgn,64,LE64(q));
				break;
			case FILE_OCTAL:
				if (OFFSET_OOB(nbytes, offset, m->vallen))
					return 0;
				off = SEXT(sgn,64,strtoull(p->s, NULL, 8));
				break;
			default:
				if ((ms->flags & MAGIC_DEBUG) != 0)
					fprintf(stderr, "bad op=%d\n", op);
				return 0;
			}
			if ((ms->flags & MAGIC_DEBUG) != 0)
				fprintf(stderr, "indirect offs=%jd\n", off);
		}
		switch (in_type = cvt_flip(m->in_type, flip)) {
		case FILE_BYTE:
			if (OFFSET_OOB(nbytes, offset, 1))
				return 0;
			offset = do_ops(m, SEXT(sgn,8,p->b), off);
			break;
		case FILE_BESHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,BE16(p)), off);
			break;
		case FILE_LESHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,LE16(p)), off);
			break;
		case FILE_SHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,p->h), off);
			break;
		case FILE_BELONG:
		case FILE_BEID3:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			lhs = BE32(p);
			if (in_type == FILE_BEID3)
				lhs = cvt_id3(ms, CAST(uint32_t, lhs));
			offset = do_ops(m, SEXT(sgn,32,lhs), off);
			break;
		case FILE_LELONG:
		case FILE_LEID3:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			lhs = LE32(p);
			if (in_type == FILE_LEID3)
				lhs = cvt_id3(ms, CAST(uint32_t, lhs));
			offset = do_ops(m, SEXT(sgn,32,lhs), off);
			break;
		case FILE_MELONG:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			offset = do_ops(m, SEXT(sgn,32,ME32(p)), off);
			break;
		case FILE_LONG:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			offset = do_ops(m, SEXT(sgn,32,p->l), off);
			break;
		case FILE_LEQUAD:
			if (OFFSET_OOB(nbytes, offset, 8))
				return 0;
			offset = do_ops(m, SEXT(sgn,64,LE64(p)), off);
			break;
		case FILE_BEQUAD:
			if (OFFSET_OOB(nbytes, offset, 8))
				return 0;
			offset = do_ops(m, SEXT(sgn,64,BE64(p)), off);
			break;
		case FILE_OCTAL:
			if (OFFSET_OOB(nbytes, offset, m->vallen))
				return 0;
			offset = do_ops(m,
			    SEXT(sgn,64,strtoull(p->s, NULL, 8)), off);
			break;
		default:
			if ((ms->flags & MAGIC_DEBUG) != 0)
				fprintf(stderr, "bad in_type=%d\n", in_type);
			return 0;
		}

		if (m->flag & INDIROFFADD) {
			if (cont_level == 0) {
				if ((ms->flags & MAGIC_DEBUG) != 0)
					fprintf(stderr,
					    "indirect *zero* cont_level\n");
				return 0;
			}
			offset += ms->c.li[cont_level - 1].off;
			if (offset == 0) {
				if ((ms->flags & MAGIC_DEBUG) != 0)
					fprintf(stderr,
					    "indirect *zero* offset\n");
				return 0;
			}
			if ((ms->flags & MAGIC_DEBUG) != 0)
				fprintf(stderr, "indirect +offs=%u\n", offset);
		}
		if (mcopy(ms, p, m->type, 0, s, offset, nbytes, m) == -1)
			return -1;
		ms->offset = offset;

		if ((ms->flags & MAGIC_DEBUG) != 0) {
			mdebug(offset, RCAST(char *, RCAST(void *, p)),
			    sizeof(union VALUETYPE));
#ifndef COMPILE_ONLY
			file_mdump(m);
#endif
		}
	}

	/* Verify we have enough data to match magic type */
	switch (m->type) {
	case FILE_BYTE:
		if (OFFSET_OOB(nbytes, offset, 1))
			return 0;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
		if (OFFSET_OOB(nbytes, offset, 2))
			return 0;
		break;

	case FILE_LONG:
	case FILE_BELONG:
	case FILE_LELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		if (OFFSET_OOB(nbytes, offset, 4))
			return 0;
		break;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		if (OFFSET_OOB(nbytes, offset, 8))
			return 0;
		break;

	case FILE_GUID:
		if (OFFSET_OOB(nbytes, offset, 16))
			return 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_SEARCH:
	case FILE_OCTAL:
		if (OFFSET_OOB(nbytes, offset, m->vallen))
			return 0;
		break;

	case FILE_REGEX:
		if (nbytes < offset)
			return 0;
		break;

	case FILE_INDIRECT:
		if (m->str_flags & INDIRECT_RELATIVE)
			offset += CAST(uint32_t, o);
		if (offset == 0)
			return 0;

		if (nbytes < offset)
			return 0;

		if ((pb = file_push_buffer(ms)) == NULL)
			return -1;

		(*indir_count)++;
		bb = *b;
		bb.fbuf = s + offset;
		bb.flen = nbytes - offset;
		rv = -1;
		for (mlp = ms->mlist[0]->next; mlp != ms->mlist[0];
		    mlp = mlp->next)
		{
			if ((rv = match(ms, mlp->magic,
			    mlp->nmagic, &bb, 0, BINTEST, text, 0, indir_count,
			    name_count, printed_something, need_separator,
			    NULL, NULL)) != 0)
				break;
		}

		if ((ms->flags & MAGIC_DEBUG) != 0)
			fprintf(stderr, "indirect @offs=%u[%d]\n", offset, rv);

		rbuf = file_pop_buffer(ms, pb);
		if (rbuf == NULL && ms->event_flags & EVENT_HAD_ERR)
			return -1;

		if (rv == 1) {
			if ((ms->flags & MAGIC_NODESC) == 0 &&
			    file_printf(ms, F(ms, m->desc, "%u"), offset) == -1)
			{
				if (rbuf) efree(rbuf);
				return -1;
			}
			if (file_printf(ms, "%s", rbuf) == -1) {
				if (rbuf) efree(rbuf);
				return -1;
			}
		}
		if (rbuf) efree(rbuf);
		return rv;

	case FILE_USE:
		if (nbytes < offset)
			return 0;
		rbuf = m->value.s;
		if (*rbuf == '^') {
			rbuf++;
			flip = !flip;
		}
		if (file_magicfind(ms, rbuf, &ml) == -1) {
			file_error(ms, 0, "cannot find entry `%s'", rbuf);
			return -1;
		}
		if (save_cont(ms, &c) == -1) {
			file_error(ms, errno, "can't allocate continuation");
			return -1;
		}

		oneed_separator = *need_separator;
		if (m->flag & NOSPACE)
			*need_separator = 0;

		nfound_match = 0;
		(*name_count)++;
		eoffset = ms->eoffset;
		rv = match(ms, ml.magic, ml.nmagic, b,
		    offset + o, mode, text, flip, indir_count, name_count,
		    printed_something, need_separator, returnval,
		    &nfound_match);
		ms->ms_value.q = nfound_match;
		(*name_count)--;
		*found_match |= nfound_match;

		restore_cont(ms, &c);

		if (rv != 1)
		    *need_separator = oneed_separator;
		ms->offset = offset;
		ms->eoffset = eoffset;
		return rv;

	case FILE_NAME:
		if (ms->flags & MAGIC_NODESC)
			return 1;
		if (file_printf(ms, "%s", m->desc) == -1)
			return -1;
		return 1;
	case FILE_DER:
	case FILE_DEFAULT:	/* nothing to check */
	case FILE_CLEAR:
	default:
		break;
	}
	if (!mconvert(ms, m, flip))
		return 0;
	return 1;
}

private uint64_t
file_strncmp(const char *s1, const char *s2, size_t len, size_t maxlen,
    uint32_t flags)
{
	/*
	 * Convert the source args to unsigned here so that (1) the
	 * compare will be unsigned as it is in strncmp() and (2) so
	 * the ctype functions will work correctly without extra
	 * casting.
	 */
	const unsigned char *a = RCAST(const unsigned char *, s1);
	const unsigned char *b = RCAST(const unsigned char *, s2);
	uint32_t ws = flags & (STRING_COMPACT_WHITESPACE |
	    STRING_COMPACT_OPTIONAL_WHITESPACE);
	const unsigned char *eb = b + (ws ? maxlen : len);
	uint64_t v;

	/*
	 * What we want here is v = strncmp(s1, s2, len),
	 * but ignoring any nulls.
	 */
	v = 0;
	len++;
	if (0L == flags) { /* normal string: do it fast */
		while (--len > 0)
			if ((v = *b++ - *a++) != '\0')
				break;
	}
	else { /* combine the others */
		while (--len > 0) {
			if (b >= eb) {
				v = 1;
				break;
			}
			if ((flags & STRING_IGNORE_LOWERCASE) &&
			    islower(*a)) {
				if ((v = tolower(*b++) - *a++) != '\0')
					break;
			}
			else if ((flags & STRING_IGNORE_UPPERCASE) &&
			    isupper(*a)) {
				if ((v = toupper(*b++) - *a++) != '\0')
					break;
			}
			else if ((flags & STRING_COMPACT_WHITESPACE) &&
			    isspace(*a)) {
				/* XXX Dirty. The data and the pattern is what is causing this.
				       Revert _i for the next port and see if it still matters. */
				uint32_t _i = 0;
				a++;
				if (isspace(*b++)) {
					if (!isspace(*a))
						while (EXPECTED(_i++ < 2048) && b < eb && isspace(*b))
							b++;
				}
				else {
					v = 1;
					break;
				}
			}
			else if ((flags & STRING_COMPACT_OPTIONAL_WHITESPACE) &&
			    isspace(*a)) {
				a++;
				while (b < eb && isspace(*b))
					b++;
			}
			else {
				if ((v = *b++ - *a++) != '\0')
					break;
			}
		}
		if (len == 0 && v == 0 && (flags & STRING_FULL_WORD)) {
			if (*b && !isspace(*b))
				v = 1;
		}
	}
	return v;
}

private uint64_t
file_strncmp16(const char *a, const char *b, size_t len, size_t maxlen,
    uint32_t flags)
{
	/*
	 * XXX - The 16-bit string compare probably needs to be done
	 * differently, especially if the flags are to be supported.
	 * At the moment, I am unsure.
	 */
	flags = 0;
	return file_strncmp(a, b, len, maxlen, flags);
}

private int
magiccheck(struct magic_set *ms, struct magic *m)
{
	uint64_t l = m->value.q;
	uint64_t v;
	float fl, fv;
	double dl, dv;
	int matched;
	union VALUETYPE *p = &ms->ms_value;

	switch (m->type) {
	case FILE_BYTE:
		v = p->b;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
	case FILE_MSDOSDATE:
	case FILE_LEMSDOSDATE:
	case FILE_BEMSDOSDATE:
	case FILE_MSDOSTIME:
	case FILE_LEMSDOSTIME:
	case FILE_BEMSDOSTIME:
		v = p->h;
		break;

	case FILE_LONG:
	case FILE_BELONG:
	case FILE_LELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		v = p->l;
		break;

	case FILE_QUAD:
	case FILE_LEQUAD:
	case FILE_BEQUAD:
	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
	case FILE_QWDATE:
	case FILE_BEQWDATE:
	case FILE_LEQWDATE:
	case FILE_OFFSET:
		v = p->q;
		break;

	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		fl = m->value.f;
		fv = p->f;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = fv != fl;
			break;

		case '=':
			matched = fv == fl;
			break;

		case '>':
			matched = fv > fl;
			break;

		case '<':
			matched = fv < fl;
			break;

		default:
			file_magerror(ms, "cannot happen with float: "
			    "invalid relation `%c'", m->reln);
			return -1;
		}
		return matched;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		dl = m->value.d;
		dv = p->d;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = dv != dl;
			break;

		case '=':
			matched = dv == dl;
			break;

		case '>':
			matched = dv > dl;
			break;

		case '<':
			matched = dv < dl;
			break;

		default:
			file_magerror(ms, "cannot happen with double: "
			    "invalid relation `%c'", m->reln);
			return -1;
		}
		return matched;

	case FILE_DEFAULT:
	case FILE_CLEAR:
		l = 0;
		v = 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_OCTAL:
		l = 0;
		v = file_strncmp(m->value.s, p->s, CAST(size_t, m->vallen),
		    sizeof(p->s), m->str_flags);
		break;

	case FILE_BESTRING16:
	case FILE_LESTRING16:
		l = 0;
		v = file_strncmp16(m->value.s, p->s, CAST(size_t, m->vallen),
		    sizeof(p->s), m->str_flags);
		break;

	case FILE_SEARCH: { /* search ms->search.s for the string m->value.s */
		size_t slen;
		size_t idx;

		if (ms->search.s == NULL)
			return 0;

		slen = MIN(m->vallen, sizeof(m->value.s));
		l = 0;
		v = 0;
#ifdef HAVE_MEMMEM
		if (slen > 0 && m->str_flags == 0) {
			const char *found;
			idx = m->str_range + slen;
			if (m->str_range == 0 || ms->search.s_len < idx)
				idx = ms->search.s_len;
			found = CAST(const char *, php_memnstr(ms->search.s,
			    m->value.s, slen, ms->search.s + idx));
			if (!found) {
				v = 1;
				break;
			}
			idx = found - ms->search.s;
			ms->search.offset += idx;
			ms->search.rm_len = ms->search.s_len - idx;
			break;
		}
#endif

		for (idx = 0; m->str_range == 0 || idx < m->str_range; idx++) {
			if (slen + idx > ms->search.s_len) {
				v = 1;
				break;
			}

			v = file_strncmp(m->value.s, ms->search.s + idx, slen,
			    ms->search.s_len - idx, m->str_flags);
			if (v == 0) {	/* found match */
				ms->search.offset += idx;
				ms->search.rm_len = ms->search.s_len - idx;
				break;
			}
		}
		break;
	}
	case FILE_REGEX: {
		zend_string *pattern;
		uint32_t options = 0;
		pcre_cache_entry *pce;

		options |= PCRE2_MULTILINE;

		if (m->str_flags & STRING_IGNORE_CASE) {
			options |= PCRE2_CASELESS;
		}

		pattern = convert_libmagic_pattern((char *)m->value.s, m->vallen, options);

		l = v = 0;
		if ((pce = pcre_get_compiled_regex_cache(pattern)) == NULL) {
			zend_string_release(pattern);
			return -1;
		} else {
			/* pce now contains the compiled regex */
			zval retval;
			zval subpats;
			zend_string *haystack;

			ZVAL_NULL(&retval);
			ZVAL_NULL(&subpats);

			/* Cut the search len from haystack, equals to REG_STARTEND */
			haystack = zend_string_init(ms->search.s, ms->search.s_len, 0);

			/* match v = 0, no match v = 1 */
			php_pcre_match_impl(pce, haystack, &retval, &subpats, 0, 1, PREG_OFFSET_CAPTURE, 0);
			/* Free haystack */
			zend_string_release(haystack);

			if (Z_LVAL(retval) < 0) {
				zval_ptr_dtor(&subpats);
				zend_string_release(pattern);
				return -1;
			} else if ((Z_LVAL(retval) > 0) && (Z_TYPE(subpats) == IS_ARRAY)) {
				/* Need to fetch global match which equals pmatch[0] */
				zval *pzval;
				HashTable *ht = Z_ARRVAL(subpats);
				if ((pzval = zend_hash_index_find(ht, 0)) != NULL && Z_TYPE_P(pzval) == IS_ARRAY) {
					/* If everything goes according to the master plan
					   tmpcopy now contains two elements:
					   0 = the match
					   1 = starting position of the match */
					zval *match, *offset;
					if ((match = zend_hash_index_find(Z_ARRVAL_P(pzval), 0)) &&
							(offset = zend_hash_index_find(Z_ARRVAL_P(pzval), 1))) {
						if (Z_TYPE_P(match) != IS_STRING && Z_TYPE_P(offset) != IS_LONG) {
							goto error_out;
						}
						ms->search.s += Z_LVAL_P(offset); /* this is where the match starts */
						ms->search.offset += Z_LVAL_P(offset); /* this is where the match starts as size_t */
						ms->search.rm_len = Z_STRLEN_P(match) /* This is the length of the matched pattern */;
						v = 0;
					} else {
						goto error_out;
					}
				} else {
error_out:
					zval_ptr_dtor(&subpats);
					zend_string_release(pattern);
					return -1;
				}
			} else {
				v = 1;
			}
			zval_ptr_dtor(&subpats);
			zend_string_release(pattern);
		}
		break;
	}
	case FILE_USE:
		return ms->ms_value.q != 0;
	case FILE_NAME:
	case FILE_INDIRECT:
		return 1;
	case FILE_DER:
		matched = der_cmp(ms, m);
		if (matched == -1) {
			if ((ms->flags & MAGIC_DEBUG) != 0) {
				(void) fprintf(stderr,
				    "EOF comparing DER entries\n");
			}
			return 0;
		}
		return matched;
	case FILE_GUID:
		l = 0;
		v = memcmp(m->value.guid, p->guid, sizeof(p->guid));
		break;
	default:
		file_magerror(ms, "invalid type %d in magiccheck()", m->type);
		return -1;
	}

	v = file_signextend(ms, m, v);

	switch (m->reln) {
	case 'x':
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT
			    "u == *any* = 1\n", CAST(unsigned long long, v));
		matched = 1;
		break;

	case '!':
		matched = v != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u != %"
			    INT64_T_FORMAT "u = %d\n",
			    CAST(unsigned long long, v),
			    CAST(unsigned long long, l), matched);
		break;

	case '=':
		matched = v == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u == %"
			    INT64_T_FORMAT "u = %d\n",
			    CAST(unsigned long long, v),
			    CAST(unsigned long long, l), matched);
		break;

	case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u > %" INT64_T_FORMAT "u = %d\n",
				    CAST(unsigned long long, v),
				    CAST(unsigned long long, l), matched);
		}
		else {
			matched = CAST(int64_t, v) > CAST(int64_t, l);
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d > %" INT64_T_FORMAT "d = %d\n",
				    CAST(long long, v),
				    CAST(long long, l), matched);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u < %" INT64_T_FORMAT "u = %d\n",
				    CAST(unsigned long long, v),
				    CAST(unsigned long long, l), matched);
		}
		else {
			matched = CAST(int64_t, v) < CAST(int64_t, l);
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d < %" INT64_T_FORMAT "d = %d\n",
				     CAST(long long, v),
				     CAST(long long, l), matched);
		}
		break;

	case '&':
		matched = (v & l) == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) == %" INT64_T_FORMAT
			    "x) = %d\n", CAST(unsigned long long, v),
			    CAST(unsigned long long, l),
			    CAST(unsigned long long, l),
			    matched);
		break;

	case '^':
		matched = (v & l) != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) != %" INT64_T_FORMAT
			    "x) = %d\n", CAST(unsigned long long, v),
			    CAST(unsigned long long, l),
			    CAST(unsigned long long, l), matched);
		break;

	default:
		file_magerror(ms, "cannot happen: invalid relation `%c'",
		    m->reln);
		return -1;
	}

	return matched;
}

private int
handle_annotation(struct magic_set *ms, struct magic *m, int firstline)
{
	if ((ms->flags & MAGIC_APPLE) && m->apple[0]) {
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (file_printf(ms, "%.8s", m->apple) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_EXTENSION) && m->ext[0]) {
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (file_printf(ms, "%s", m->ext) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_MIME_TYPE) && m->mimetype[0]) {
		char buf[1024];
		const char *p;
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (varexpand(ms, buf, sizeof(buf), m->mimetype) == -1)
			p = m->mimetype;
		else
			p = buf;
		if (file_printf(ms, "%s", p) == -1)
			return -1;
		return 1;
	}
	return 0;
}

private int
print_sep(struct magic_set *ms, int firstline)
{
	if (firstline)
		return 0;
	/*
	 * we found another match
	 * put a newline and '-' to do some simple formatting
	 */
	return file_separator(ms);
}
