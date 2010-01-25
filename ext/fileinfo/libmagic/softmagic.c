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
FILE_RCSID("@(#)$File: softmagic.c,v 1.135 2009/03/27 22:42:49 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#ifndef PREG_OFFSET_CAPTURE
# define PREG_OFFSET_CAPTURE                 (1<<8)
#endif



private int match(struct magic_set *, struct magic *, uint32_t,
    const unsigned char *, size_t, int);
private int mget(struct magic_set *, const unsigned char *,
    struct magic *, size_t, unsigned int);
private int magiccheck(struct magic_set *, struct magic *);
private int32_t mprint(struct magic_set *, struct magic *);
private int32_t moffset(struct magic_set *, struct magic *);
private void mdebug(uint32_t, const char *, size_t);
private int mcopy(struct magic_set *, union VALUETYPE *, int, int,
    const unsigned char *, uint32_t, size_t, size_t);
private int mconvert(struct magic_set *, struct magic *);
private int print_sep(struct magic_set *, int);
private int handle_annotation(struct magic_set *, struct magic *);
private void cvt_8(union VALUETYPE *, const struct magic *);
private void cvt_16(union VALUETYPE *, const struct magic *);
private void cvt_32(union VALUETYPE *, const struct magic *);
private void cvt_64(union VALUETYPE *, const struct magic *);

/*
 * softmagic - lookup one file in parsed, in-memory copy of database
 * Passed the name and FILE * of one file to be typed.
 */
/*ARGSUSED1*/		/* nbytes passed for regularity, maybe need later */
protected int
file_softmagic(struct magic_set *ms, const unsigned char *buf, size_t nbytes, int mode)
{
	struct mlist *ml;
	int rv;
	for (ml = ms->mlist->next; ml != ms->mlist; ml = ml->next)
		if ((rv = match(ms, ml->magic, ml->nmagic, buf, nbytes, mode)) != 0)
			return rv;

	return 0;
}

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
match(struct magic_set *ms, struct magic *magic, uint32_t nmagic,
    const unsigned char *s, size_t nbytes, int mode)
{
	uint32_t magindex = 0;
	unsigned int cont_level = 0;
	int need_separator = 0;
	int returnval = 0, e; /* if a match is found it is set to 1*/
	int firstline = 1; /* a flag to print X\n  X\n- X */
	int printed_something = 0;
	int print = (ms->flags & (MAGIC_MIME|MAGIC_APPLE)) == 0;

	if (file_check_mem(ms, cont_level) == -1)
		return -1;

	for (magindex = 0; magindex < nmagic; magindex++) {
		int flush = 0;
		struct magic *m = &magic[magindex];

		if ((m->flag & BINTEST) != mode) {
			/* Skip sub-tests */
			while (magindex < nmagic - 1 && magic[magindex + 1].cont_level != 0) {
				magindex++;
			}
			continue; /* Skip to next top-level test*/
		}

		ms->offset = m->offset;
		ms->line = m->lineno;

		/* if main entry matches, print it... */
		switch (mget(ms, s, m, nbytes, cont_level)) {
		case -1:
			return -1;
		case 0:
			flush = m->reln != '!';
			break;
		default:
			if (m->type == FILE_INDIRECT)
				returnval = 1;
				
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
			while (magindex < nmagic - 1 && magic[magindex + 1].cont_level != 0) {
				magindex++;
			}
			continue;
		}

		/*
		 * If we are going to print something, we'll need to print
		 * a blank before we print something else.
		 */
		if (*m->desc) {
			need_separator = 1;
			printed_something = 1;
			if ((e = handle_annotation(ms, m)) != 0)
				return e;
			if (print_sep(ms, firstline) == -1)
				return -1;
		}


		if (print && mprint(ms, m) == -1)
			return -1;

		ms->c.li[cont_level].off = moffset(ms, m);

		/* and any continuations that match */
		if (file_check_mem(ms, ++cont_level) == -1)
			return -1;

		while (magindex < nmagic - 1 && magic[magindex + 1].cont_level != 0) {
			magindex++;
			m = &magic[magindex];
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
			ms->offset = m->offset;
			if (m->flag & OFFADD) {
				ms->offset += ms->c.li[cont_level - 1].off;
			}

#ifdef ENABLE_CONDITIONALS
			if (m->cond == COND_ELSE ||
			    m->cond == COND_ELIF) {
				if (ms->c.li[cont_level].last_match == 1)
					continue;
			}
#endif
			switch (mget(ms, s, m, nbytes, cont_level)) {
			case -1:
				return -1;
			case 0:
				if (m->reln != '!')
					continue;
				flush = 1;
				break;
			default:
				if (m->type == FILE_INDIRECT)
					returnval = 1;
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
				if (m->type != FILE_DEFAULT)
					ms->c.li[cont_level].got_match = 1;
				else if (ms->c.li[cont_level].got_match) {
					ms->c.li[cont_level].got_match = 0;
					break;
				}
				/*
				 * If we are going to print something,
				 * make sure that we have a separator first.
				 */
				if (*m->desc) {
					if ((e = handle_annotation(ms, m)) != 0)
						return e;
					if (!printed_something) {
						printed_something = 1;
						if (print_sep(ms, firstline)
						    == -1)
							return -1;
					}
				}
				/*
				 * This continuation matched.  Print
				 * its message, with a blank before it
				 * if the previous item printed and
				 * this item isn't empty.
				 */
				/* space if previous printed */
				if (need_separator
				    && ((m->flag & NOSPACE) == 0)
				    && *m->desc) {
					if (print &&
					    file_printf(ms, " ") == -1)
						return -1;
					need_separator = 0;
				}
				if (print && mprint(ms, m) == -1)
					return -1;

				ms->c.li[cont_level].off = moffset(ms, m);

				if (*m->desc)
					need_separator = 1;

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
		if (printed_something) {
			firstline = 0;
			if (print)
				returnval = 1;
		}
		if ((ms->flags & MAGIC_CONTINUE) == 0 && printed_something) {
			return returnval; /* don't keep searching */
		}
	}
	return returnval;  /* This is hit if -k is set or there is no match */
}

private int
check_fmt(struct magic_set *ms, struct magic *m)
{
	pcre *pce;
	int re_options;
	pcre_extra *re_extra;
	TSRMLS_FETCH();
	
	if (strchr(m->desc, '%') == NULL) {
		return 0;
	}
	
	if ((pce = pcre_get_compiled_regex("~%[-0-9.]*s~", &re_extra, &re_options TSRMLS_CC)) == NULL) {
		return -1;
	} else {
	 	return !pcre_exec(pce, re_extra, m->desc, strlen(m->desc), 0, re_options, NULL, 0);
	}
}

private int32_t
mprint(struct magic_set *ms, struct magic *m)
{
	uint64_t v;
	float vf;
	double vd;
	int64_t t = 0;
 	char buf[128];
	union VALUETYPE *p = &ms->ms_value;

  	switch (m->type) {
  	case FILE_BYTE:
		v = file_signextend(ms, m, (uint64_t)p->b);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%c",
			    (unsigned char)v);
			if (file_printf(ms, m->desc, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, m->desc, (unsigned char) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(char);
		break;

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		v = file_signextend(ms, m, (uint64_t)p->h);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%hu",
			    (unsigned short)v);
			if (file_printf(ms, m->desc, buf) == -1)
				return -1;
			break;
		default:
			if (
			    file_printf(ms, m->desc, (unsigned short) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(short);
		break;

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		v = file_signextend(ms, m, (uint64_t)p->l);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%u", (uint32_t)v);
			if (file_printf(ms, m->desc, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, m->desc, (uint32_t) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(int32_t);
  		break;

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		v = file_signextend(ms, m, p->q);
		if (file_printf(ms, m->desc, (uint64_t) v) == -1)
			return -1;
		t = ms->offset + sizeof(int64_t);
  		break;

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!') {
			if (file_printf(ms, m->desc, m->value.s) == -1)
				return -1;
			t = ms->offset + m->vallen;
		}
		else {
			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\n")] = '\0';
			if (file_printf(ms, m->desc, p->s) == -1)
				return -1;
			t = ms->offset + strlen(p->s);
			if (m->type == FILE_PSTRING)
				t++;
		}
		break;

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		if (file_printf(ms, m->desc, file_fmttime(p->l, 1)) == -1)
			return -1;
		t = ms->offset + sizeof(time_t);
		break;

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		if (file_printf(ms, m->desc, file_fmttime(p->l, 0)) == -1)
			return -1;
		t = ms->offset + sizeof(time_t);
		break;

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		if (file_printf(ms, m->desc, file_fmttime((uint32_t)p->q,
		    1)) == -1)
			return -1;
		t = ms->offset + sizeof(uint64_t);
		break;

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		if (file_printf(ms, m->desc, file_fmttime((uint32_t)p->q,
		    0)) == -1)
			return -1;
		t = ms->offset + sizeof(uint64_t);
		break;

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		vf = p->f;
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vf);
			if (file_printf(ms, m->desc, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, m->desc, vf) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(float);
  		break;

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		vd = p->d;
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vd);
			if (file_printf(ms, m->desc, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, m->desc, vd) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(double);
  		break;

	case FILE_REGEX: {
		char *cp;
		int rval;

		cp = estrndup((const char *)ms->search.s, ms->search.rm_len);

		rval = file_printf(ms, m->desc, cp);
		efree(cp);

		if (rval == -1)
			return -1;

		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + ms->search.rm_len;
		break;
	}

	case FILE_SEARCH:
	  	if (file_printf(ms, m->desc, m->value.s) == -1)
			return -1;
		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + m->vallen;
		break;

	case FILE_DEFAULT:
	  	if (file_printf(ms, m->desc, m->value.s) == -1)
			return -1;
		t = ms->offset;
		break;

	case FILE_INDIRECT:
		t = ms->offset;
		break;

	default:
		file_magerror(ms, "invalid m->type (%d) in mprint()", m->type);
		return -1;
	}
	return (int32_t)t;
}

private int32_t
moffset(struct magic_set *ms, struct magic *m)
{
  	switch (m->type) {
  	case FILE_BYTE:
		return ms->offset + sizeof(char);

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		return ms->offset + sizeof(short);

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		return ms->offset + sizeof(int32_t);

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		return ms->offset + sizeof(int64_t);

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!')
			return ms->offset + m->vallen;
		else {
			union VALUETYPE *p = &ms->ms_value;
			uint32_t t;

			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\n")] = '\0';
			t = ms->offset + strlen(p->s);
			if (m->type == FILE_PSTRING)
				t++;
			return t;
		}

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		return ms->offset + sizeof(time_t);

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		return ms->offset + sizeof(time_t);

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		return ms->offset + sizeof(uint64_t);

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		return ms->offset + sizeof(uint64_t);

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		return ms->offset + sizeof(float);

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		return ms->offset + sizeof(double);
  		break;

	case FILE_REGEX:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			return ms->search.offset;
		else
			return ms->search.offset + ms->search.rm_len;

	case FILE_SEARCH:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			return ms->search.offset;
		else
			return ms->search.offset + m->vallen;

	case FILE_DEFAULT:
		return ms->offset;

	case FILE_INDIRECT:
		return ms->offset;

	default:
		return 0;
	}
}

#define DO_CVT(fld, cast) \
	if (m->num_mask) \
		switch (m->mask_op & FILE_OPS_MASK) { \
		case FILE_OPAND: \
			p->fld &= cast m->num_mask; \
			break; \
		case FILE_OPOR: \
			p->fld |= cast m->num_mask; \
			break; \
		case FILE_OPXOR: \
			p->fld ^= cast m->num_mask; \
			break; \
		case FILE_OPADD: \
			p->fld += cast m->num_mask; \
			break; \
		case FILE_OPMINUS: \
			p->fld -= cast m->num_mask; \
			break; \
		case FILE_OPMULTIPLY: \
			p->fld *= cast m->num_mask; \
			break; \
		case FILE_OPDIVIDE: \
			p->fld /= cast m->num_mask; \
			break; \
		case FILE_OPMODULO: \
			p->fld %= cast m->num_mask; \
			break; \
		} \
	if (m->mask_op & FILE_OPINVERSE) \
		p->fld = ~p->fld \

private void
cvt_8(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(b, (uint8_t));
}

private void
cvt_16(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(h, (uint16_t));
}

private void
cvt_32(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(l, (uint32_t));
}

private void
cvt_64(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT(q, (uint64_t));
}

#define DO_CVT2(fld, cast) \
	if (m->num_mask) \
		switch (m->mask_op & FILE_OPS_MASK) { \
		case FILE_OPADD: \
			p->fld += cast (int64_t)m->num_mask; \
			break; \
		case FILE_OPMINUS: \
			p->fld -= cast (int64_t)m->num_mask; \
			break; \
		case FILE_OPMULTIPLY: \
			p->fld *= cast (int64_t)m->num_mask; \
			break; \
		case FILE_OPDIVIDE: \
			p->fld /= cast (int64_t)m->num_mask; \
			break; \
		} \

private void
cvt_float(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT2(f, (float));
}

private void
cvt_double(union VALUETYPE *p, const struct magic *m)
{
	DO_CVT2(d, (double));
}

/*
 * Convert the byte order of the data we are looking at
 * While we're here, let's apply the mask operation
 * (unless you have a better idea)
 */
private int
mconvert(struct magic_set *ms, struct magic *m)
{
	union VALUETYPE *p = &ms->ms_value;

	switch (m->type) {
	case FILE_BYTE:
		cvt_8(p, m);
		return 1;
	case FILE_SHORT:
		cvt_16(p, m);
		return 1;
	case FILE_LONG:
	case FILE_DATE:
	case FILE_LDATE:
		cvt_32(p, m);
		return 1;
	case FILE_QUAD:
	case FILE_QDATE:
	case FILE_QLDATE:
		cvt_64(p, m);
		return 1;
	case FILE_STRING:
	case FILE_BESTRING16:
	case FILE_LESTRING16: {
		/* Null terminate and eat *trailing* return */
		p->s[sizeof(p->s) - 1] = '\0';
#if 0
		/* Why? breaks magic numbers that end with \xa */
		len = strlen(p->s);
		if (len-- && p->s[len] == '\n')
			p->s[len] = '\0';
#endif
		return 1;
	}
	case FILE_PSTRING: {
		char *ptr1 = p->s, *ptr2 = ptr1 + 1;
		size_t len = *p->s;
		if (len >= sizeof(p->s))
			len = sizeof(p->s) - 1;
		while (len--)
			*ptr1++ = *ptr2++;
		*ptr1 = '\0';
#if 0
		/* Why? breaks magic numbers that end with \xa */
		len = strlen(p->s);
		if (len-- && p->s[len] == '\n')
			p->s[len] = '\0';
#endif
		return 1;
	}
	case FILE_BESHORT:
		p->h = (short)((p->hs[0]<<8)|(p->hs[1]));
		cvt_16(p, m);
		return 1;
	case FILE_BELONG:
	case FILE_BEDATE:
	case FILE_BELDATE:
		p->l = (int32_t)
		    ((p->hl[0]<<24)|(p->hl[1]<<16)|(p->hl[2]<<8)|(p->hl[3]));
		cvt_32(p, m);
		return 1;
	case FILE_BEQUAD:
	case FILE_BEQDATE:
	case FILE_BEQLDATE:
		p->q = (uint64_t)
		    (((uint64_t)p->hq[0]<<56)|((uint64_t)p->hq[1]<<48)|
		     ((uint64_t)p->hq[2]<<40)|((uint64_t)p->hq[3]<<32)|
		     ((uint64_t)p->hq[4]<<24)|((uint64_t)p->hq[5]<<16)|
		     ((uint64_t)p->hq[6]<<8)|((uint64_t)p->hq[7]));
		cvt_64(p, m);
		return 1;
	case FILE_LESHORT:
		p->h = (short)((p->hs[1]<<8)|(p->hs[0]));
		cvt_16(p, m);
		return 1;
	case FILE_LELONG:
	case FILE_LEDATE:
	case FILE_LELDATE:
		p->l = (int32_t)
		    ((p->hl[3]<<24)|(p->hl[2]<<16)|(p->hl[1]<<8)|(p->hl[0]));
		cvt_32(p, m);
		return 1;
	case FILE_LEQUAD:
	case FILE_LEQDATE:
	case FILE_LEQLDATE:
		p->q = (uint64_t)
		    (((uint64_t)p->hq[7]<<56)|((uint64_t)p->hq[6]<<48)|
		     ((uint64_t)p->hq[5]<<40)|((uint64_t)p->hq[4]<<32)|
		     ((uint64_t)p->hq[3]<<24)|((uint64_t)p->hq[2]<<16)|
		     ((uint64_t)p->hq[1]<<8)|((uint64_t)p->hq[0]));
		cvt_64(p, m);
		return 1;
	case FILE_MELONG:
	case FILE_MEDATE:
	case FILE_MELDATE:
		p->l = (int32_t)
		    ((p->hl[1]<<24)|(p->hl[0]<<16)|(p->hl[3]<<8)|(p->hl[2]));
		cvt_32(p, m);
		return 1;
	case FILE_FLOAT:
		cvt_float(p, m);
		return 1;
	case FILE_BEFLOAT:
		p->l =  ((uint32_t)p->hl[0]<<24)|((uint32_t)p->hl[1]<<16)|
			((uint32_t)p->hl[2]<<8) |((uint32_t)p->hl[3]);
		cvt_float(p, m);
		return 1;
	case FILE_LEFLOAT:
		p->l =  ((uint32_t)p->hl[3]<<24)|((uint32_t)p->hl[2]<<16)|
			((uint32_t)p->hl[1]<<8) |((uint32_t)p->hl[0]);
		cvt_float(p, m);
		return 1;
	case FILE_DOUBLE:
		cvt_double(p, m);
		return 1;
	case FILE_BEDOUBLE:
		p->q =  ((uint64_t)p->hq[0]<<56)|((uint64_t)p->hq[1]<<48)|
			((uint64_t)p->hq[2]<<40)|((uint64_t)p->hq[3]<<32)|
			((uint64_t)p->hq[4]<<24)|((uint64_t)p->hq[5]<<16)|
			((uint64_t)p->hq[6]<<8) |((uint64_t)p->hq[7]);
		cvt_double(p, m);
		return 1;
	case FILE_LEDOUBLE:
		p->q =  ((uint64_t)p->hq[7]<<56)|((uint64_t)p->hq[6]<<48)|
			((uint64_t)p->hq[5]<<40)|((uint64_t)p->hq[4]<<32)|
			((uint64_t)p->hq[3]<<24)|((uint64_t)p->hq[2]<<16)|
			((uint64_t)p->hq[1]<<8) |((uint64_t)p->hq[0]);
		cvt_double(p, m);
		return 1;
	case FILE_REGEX:
	case FILE_SEARCH:
	case FILE_DEFAULT:
		return 1;
	default:
		file_magerror(ms, "invalid type %d in mconvert()", m->type);
		return 0;
	}
}


private void
mdebug(uint32_t offset, const char *str, size_t len)
{
	(void) fprintf(stderr, "mget @%d: ", offset);
	file_showstr(stderr, str, len);
	(void) fputc('\n', stderr);
	(void) fputc('\n', stderr);
}

private int
mcopy(struct magic_set *ms, union VALUETYPE *p, int type, int indir,
    const unsigned char *s, uint32_t offset, size_t nbytes, size_t linecnt)
{
	/*
	 * Note: FILE_SEARCH and FILE_REGEX do not actually copy
	 * anything, but setup pointers into the source
	 */
	if (indir == 0) {
		switch (type) {
		case FILE_SEARCH:
			ms->search.s = (const char *)s + offset;
			ms->search.s_len = nbytes - offset;
			ms->search.offset = offset;
			return 0;

		case FILE_REGEX: {
			const char *b;
			const char *c;
			const char *last;	/* end of search region */
			const char *buf;	/* start of search region */
			const char *end;
			size_t lines;

			if (s == NULL) {
				ms->search.s_len = 0;
				ms->search.s = NULL;
				return 0;
			}
			buf = (const char *)s + offset;
			end = last = (const char *)s + nbytes;
			/* mget() guarantees buf <= last */
			for (lines = linecnt, b = buf;
			     lines && ((b = memchr(c = b, '\n', end - b)) || (b = memchr(c, '\r', end - c)));
			     lines--, b++) {
				last = b;
				if (b[0] == '\r' && b[1] == '\n')
					b++;
			}
			if (lines)
				last = (const char *)s + nbytes;

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

			/* check for pointer overflow */
			if (src < s) {
				file_magerror(ms, "invalid offset %u in mcopy()",
				    offset);
				return -1;
			}
			for (/*EMPTY*/; src < esrc; src += 2, dst++) {
				if (dst < edst)
					*dst = *src;
				else
					break;
				if (*dst == '\0') {
					if (type == FILE_BESTRING16 ?
					    *(src - 1) != '\0' :
					    *(src + 1) != '\0')
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
		(void)memset(((char *)(void *)p) + nbytes, '\0',
		    sizeof(*p) - nbytes);
	return 0;
}

private int
mget(struct magic_set *ms, const unsigned char *s,
    struct magic *m, size_t nbytes, unsigned int cont_level)
{
	uint32_t offset = ms->offset;
	uint32_t count = m->str_range;
	union VALUETYPE *p = &ms->ms_value;

	if (mcopy(ms, p, m->type, m->flag & INDIR, s, offset, nbytes, count) == -1)
		return -1;

	if ((ms->flags & MAGIC_DEBUG) != 0) {
		mdebug(offset, (char *)(void *)p, sizeof(union VALUETYPE));
	}

	if (m->flag & INDIR) {
		int off = m->in_offset;
		if (m->in_op & FILE_OPINDIRECT) {
			const union VALUETYPE *q =
			    ((const void *)(s + offset + off));
			switch (m->in_type) {
			case FILE_BYTE:
				off = q->b;
				break;
			case FILE_SHORT:
				off = q->h;
				break;
			case FILE_BESHORT:
				off = (short)((q->hs[0]<<8)|(q->hs[1]));
				break;
			case FILE_LESHORT:
				off = (short)((q->hs[1]<<8)|(q->hs[0]));
				break;
			case FILE_LONG:
				off = q->l;
				break;
			case FILE_BELONG:
			case FILE_BEID3:
				off = (int32_t)((q->hl[0]<<24)|(q->hl[1]<<16)|
						 (q->hl[2]<<8)|(q->hl[3]));
				break;
			case FILE_LEID3:
			case FILE_LELONG:
				off = (int32_t)((q->hl[3]<<24)|(q->hl[2]<<16)|
						 (q->hl[1]<<8)|(q->hl[0]));
				break;
			case FILE_MELONG:
				off = (int32_t)((q->hl[1]<<24)|(q->hl[0]<<16)|
						 (q->hl[3]<<8)|(q->hl[2]));
				break;
			}
		}
		switch (m->in_type) {
		case FILE_BYTE:
			if (nbytes < (offset + 1))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = p->b & off;
					break;
				case FILE_OPOR:
					offset = p->b | off;
					break;
				case FILE_OPXOR:
					offset = p->b ^ off;
					break;
				case FILE_OPADD:
					offset = p->b + off;
					break;
				case FILE_OPMINUS:
					offset = p->b - off;
					break;
				case FILE_OPMULTIPLY:
					offset = p->b * off;
					break;
				case FILE_OPDIVIDE:
					offset = p->b / off;
					break;
				case FILE_OPMODULO:
					offset = p->b % off;
					break;
				}
			} else
				offset = p->b;
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_BESHORT:
			if (nbytes < (offset + 2))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) &
						 off;
					break;
				case FILE_OPOR:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) |
						 off;
					break;
				case FILE_OPXOR:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) ^
						 off;
					break;
				case FILE_OPADD:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) +
						 off;
					break;
				case FILE_OPMINUS:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) -
						 off;
					break;
				case FILE_OPMULTIPLY:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) *
						 off;
					break;
				case FILE_OPDIVIDE:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) /
						 off;
					break;
				case FILE_OPMODULO:
					offset = (short)((p->hs[0]<<8)|
							 (p->hs[1])) %
						 off;
					break;
				}
			} else
				offset = (short)((p->hs[0]<<8)|
						 (p->hs[1]));
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_LESHORT:
			if (nbytes < (offset + 2))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) &
						 off;
					break;
				case FILE_OPOR:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) |
						 off;
					break;
				case FILE_OPXOR:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) ^
						 off;
					break;
				case FILE_OPADD:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) +
						 off;
					break;
				case FILE_OPMINUS:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) -
						 off;
					break;
				case FILE_OPMULTIPLY:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) *
						 off;
					break;
				case FILE_OPDIVIDE:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) /
						 off;
					break;
				case FILE_OPMODULO:
					offset = (short)((p->hs[1]<<8)|
							 (p->hs[0])) %
						 off;
					break;
				}
			} else
				offset = (short)((p->hs[1]<<8)|
						 (p->hs[0]));
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_SHORT:
			if (nbytes < (offset + 2))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = p->h & off;
					break;
				case FILE_OPOR:
					offset = p->h | off;
					break;
				case FILE_OPXOR:
					offset = p->h ^ off;
					break;
				case FILE_OPADD:
					offset = p->h + off;
					break;
				case FILE_OPMINUS:
					offset = p->h - off;
					break;
				case FILE_OPMULTIPLY:
					offset = p->h * off;
					break;
				case FILE_OPDIVIDE:
					offset = p->h / off;
					break;
				case FILE_OPMODULO:
					offset = p->h % off;
					break;
				}
			}
			else
				offset = p->h;
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_BELONG:
		case FILE_BEID3:
			if (nbytes < (offset + 4))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) &
						 off;
					break;
				case FILE_OPOR:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) |
						 off;
					break;
				case FILE_OPXOR:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) ^
						 off;
					break;
				case FILE_OPADD:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) +
						 off;
					break;
				case FILE_OPMINUS:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) -
						 off;
					break;
				case FILE_OPMULTIPLY:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) *
						 off;
					break;
				case FILE_OPDIVIDE:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) /
						 off;
					break;
				case FILE_OPMODULO:
					offset = (int32_t)((p->hl[0]<<24)|
							 (p->hl[1]<<16)|
							 (p->hl[2]<<8)|
							 (p->hl[3])) %
						 off;
					break;
				}
			} else
				offset = (int32_t)((p->hl[0]<<24)|
						 (p->hl[1]<<16)|
						 (p->hl[2]<<8)|
						 (p->hl[3]));
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_LELONG:
		case FILE_LEID3:
			if (nbytes < (offset + 4))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) &
						 off;
					break;
				case FILE_OPOR:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) |
						 off;
					break;
				case FILE_OPXOR:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) ^
						 off;
					break;
				case FILE_OPADD:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) +
						 off;
					break;
				case FILE_OPMINUS:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) -
						 off;
					break;
				case FILE_OPMULTIPLY:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) *
						 off;
					break;
				case FILE_OPDIVIDE:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) /
						 off;
					break;
				case FILE_OPMODULO:
					offset = (int32_t)((p->hl[3]<<24)|
							 (p->hl[2]<<16)|
							 (p->hl[1]<<8)|
							 (p->hl[0])) %
						 off;
					break;
				}
			} else
				offset = (int32_t)((p->hl[3]<<24)|
						 (p->hl[2]<<16)|
						 (p->hl[1]<<8)|
						 (p->hl[0]));
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_MELONG:
			if (nbytes < (offset + 4))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) &
						 off;
					break;
				case FILE_OPOR:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) |
						 off;
					break;
				case FILE_OPXOR:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) ^
						 off;
					break;
				case FILE_OPADD:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) +
						 off;
					break;
				case FILE_OPMINUS:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) -
						 off;
					break;
				case FILE_OPMULTIPLY:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) *
						 off;
					break;
				case FILE_OPDIVIDE:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) /
						 off;
					break;
				case FILE_OPMODULO:
					offset = (int32_t)((p->hl[1]<<24)|
							 (p->hl[0]<<16)|
							 (p->hl[3]<<8)|
							 (p->hl[2])) %
						 off;
					break;
				}
			} else
				offset = (int32_t)((p->hl[1]<<24)|
						 (p->hl[0]<<16)|
						 (p->hl[3]<<8)|
						 (p->hl[2]));
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		case FILE_LONG:
			if (nbytes < (offset + 4))
				return 0;
			if (off) {
				switch (m->in_op & FILE_OPS_MASK) {
				case FILE_OPAND:
					offset = p->l & off;
					break;
				case FILE_OPOR:
					offset = p->l | off;
					break;
				case FILE_OPXOR:
					offset = p->l ^ off;
					break;
				case FILE_OPADD:
					offset = p->l + off;
					break;
				case FILE_OPMINUS:
					offset = p->l - off;
					break;
				case FILE_OPMULTIPLY:
					offset = p->l * off;
					break;
				case FILE_OPDIVIDE:
					offset = p->l / off;
					break;
				case FILE_OPMODULO:
					offset = p->l % off;
					break;
				}
			} else
				offset = p->l;
			if (m->in_op & FILE_OPINVERSE)
				offset = ~offset;
			break;
		}

		switch (m->in_type) {
		case FILE_LEID3:
		case FILE_BEID3:
			offset = ((((offset >>  0) & 0x7f) <<  0) |
				 (((offset >>  8) & 0x7f) <<  7) |
				 (((offset >> 16) & 0x7f) << 14) |
				 (((offset >> 24) & 0x7f) << 21)) + 10;
			break;
		default:
			break;
		}

		if (m->flag & INDIROFFADD) {
			offset += ms->c.li[cont_level-1].off;
		}
		if (mcopy(ms, p, m->type, 0, s, offset, nbytes, count) == -1)
			return -1;
		ms->offset = offset;

		if ((ms->flags & MAGIC_DEBUG) != 0) {
			mdebug(offset, (char *)(void *)p,
			    sizeof(union VALUETYPE));
		}
	}

	/* Verify we have enough data to match magic type */
	switch (m->type) {
	case FILE_BYTE:
		if (nbytes < (offset + 1)) /* should alway be true */
			return 0;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
		if (nbytes < (offset + 2))
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
		if (nbytes < (offset + 4))
			return 0;
		break;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		if (nbytes < (offset + 8))
			return 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_SEARCH:
		if (nbytes < (offset + m->vallen))
			return 0;
		break;

	case FILE_REGEX:
		if (nbytes < offset)
			return 0;
		break;

	case FILE_INDIRECT:
	  	if ((ms->flags & (MAGIC_MIME|MAGIC_APPLE)) == 0 &&
		    file_printf(ms, m->desc) == -1)
			return -1;
		if (nbytes < offset)
			return 0;
		return file_softmagic(ms, s + offset, nbytes - offset,
		    BINTEST);

	case FILE_DEFAULT:	/* nothing to check */
	default:
		break;
	}
	if (!mconvert(ms, m))
		return 0;
	return 1;
}

private uint64_t
file_strncmp(const char *s1, const char *s2, size_t len, uint32_t flags)
{
	/*
	 * Convert the source args to unsigned here so that (1) the
	 * compare will be unsigned as it is in strncmp() and (2) so
	 * the ctype functions will work correctly without extra
	 * casting.
	 */
	const unsigned char *a = (const unsigned char *)s1;
	const unsigned char *b = (const unsigned char *)s2;
	uint64_t v;

	/*
	 * What we want here is v = strncmp(s1, s2, len),
	 * but ignoring any nulls.
	 */
	v = 0;
	if (0L == flags) { /* normal string: do it fast */
		while (len-- > 0)
			if ((v = *b++ - *a++) != '\0')
				break;
	}
	else { /* combine the others */
		while (len-- > 0) {
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
			else if ((flags & STRING_COMPACT_BLANK) &&
			    isspace(*a)) {
				a++;
				if (isspace(*b++)) {
					while (isspace(*b))
						b++;
				}
				else {
					v = 1;
					break;
				}
			}
			else if ((flags & STRING_COMPACT_OPTIONAL_BLANK) &&
			    isspace(*a)) {
				a++;
				while (isspace(*b))
					b++;
			}
			else {
				if ((v = *b++ - *a++) != '\0')
					break;
			}
		}
	}
	return v;
}

private uint64_t
file_strncmp16(const char *a, const char *b, size_t len, uint32_t flags)
{
	/*
	 * XXX - The 16-bit string compare probably needs to be done
	 * differently, especially if the flags are to be supported.
	 * At the moment, I am unsure.
	 */
	flags = 0;
	return file_strncmp(a, b, len, flags);
}

private void
convert_libmagic_pattern(zval *pattern, int options)
{
		int i, j=0;
		char *t;

		t = (char *) safe_emalloc(Z_STRLEN_P(pattern), 2, 5);
		memset(t, '\0', sizeof(t));
		
		t[j++] = '~';
		
		for (i=0; i<Z_STRLEN_P(pattern); i++, j++) {
			switch (Z_STRVAL_P(pattern)[i]) {
				case '?':
					t[j] = '.';
					break;
				case '*':
					t[j++] = '.';
					t[j] = '*';
					break;
				case '.':
					t[j++] = '\\';
					t[j] = '.';
					break;
				case '\\':
					t[j++] = '\\';
					t[j] = '\\';
					break;
				case '(':
					t[j++] = '\\';
					t[j] = '(';
					break;
				case ')':
					t[j++] = '\\';
					t[j] = ')';
					break;
				case '~':
					t[j++] = '\\';
					t[j] = '~';
					break;
				default:
					t[j] = Z_STRVAL_P(pattern)[i];
					break;
			}
		}
		t[j++] = '~';
	
		if (options & PCRE_CASELESS) 
			t[j++] = 'm';
	
		if (options & PCRE_MULTILINE)
			t[j++] = 'i';

		t[j]=0;
	
		Z_STRVAL_P(pattern) = t;
		Z_STRLEN_P(pattern) = j;

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
			matched = 0;
			file_magerror(ms, "cannot happen with float: invalid relation `%c'",
			    m->reln);
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
			matched = 0;
			file_magerror(ms, "cannot happen with double: invalid relation `%c'", m->reln);
			return -1;
		}
		return matched;

	case FILE_DEFAULT:
		l = 0;
		v = 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
		l = 0;
		v = file_strncmp(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_BESTRING16:
	case FILE_LESTRING16:
		l = 0;
		v = file_strncmp16(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_SEARCH: { /* search ms->search.s for the string m->value.s */
		size_t slen;
		size_t idx;

		if (ms->search.s == NULL)
			return 0;

		slen = MIN(m->vallen, sizeof(m->value.s));
		l = 0;
		v = 0;

		for (idx = 0; m->str_range == 0 || idx < m->str_range; idx++) {
			if (slen + idx > ms->search.s_len)
				break;

			v = file_strncmp(m->value.s, ms->search.s + idx, slen, m->str_flags);
			if (v == 0) {	/* found match */
				ms->search.offset += idx;
				break;
			}
		}
		break;
	}
	case FILE_REGEX: {
		zval *pattern;
		int options = 0;
		pcre_cache_entry *pce;
		TSRMLS_FETCH();
		
		MAKE_STD_ZVAL(pattern);
		ZVAL_STRINGL(pattern, (char *)m->value.s, m->vallen, 0);
	
		options |= PCRE_MULTILINE;
		
		if (m->str_flags & STRING_IGNORE_CASE) {
			options |= PCRE_CASELESS;
		}
		
		convert_libmagic_pattern(pattern, options);
		
#if (PHP_MAJOR_VERSION < 6)
		if ((pce = pcre_get_compiled_regex_cache(Z_STRVAL_P(pattern), Z_STRLEN_P(pattern) TSRMLS_CC)) == NULL) {
#else
		if ((pce = pcre_get_compiled_regex_cache(IS_STRING, Z_STRVAL_P(pattern), Z_STRLEN_P(pattern) TSRMLS_CC)) == NULL) {
#endif
			zval_dtor(pattern);
			FREE_ZVAL(pattern);
			return -1;
		} else {
			/* pce now contains the compiled regex */
			zval *retval;
			zval *subpats;
			char *haystack;
			
			MAKE_STD_ZVAL(retval);
			ALLOC_INIT_ZVAL(subpats);
			
			/* Cut the search len from haystack, equals to REG_STARTEND */
			haystack = estrndup(ms->search.s, ms->search.s_len);

			/* match v = 0, no match v = 1 */
#if (PHP_MAJOR_VERSION < 6)
			php_pcre_match_impl(pce, haystack, ms->search.s_len, retval, subpats, 1, 1, PREG_OFFSET_CAPTURE, 0 TSRMLS_CC);
#else			
			php_pcre_match_impl(pce, IS_STRING, haystack, ms->search.s_len, retval, subpats, 1, 1, PREG_OFFSET_CAPTURE, 0 TSRMLS_CC);
#endif
			/* Free haystack */
			efree(haystack);
			
			if (Z_LVAL_P(retval) < 0) {
				zval_ptr_dtor(&subpats);
				FREE_ZVAL(retval);
				zval_dtor(pattern);
				FREE_ZVAL(pattern);
				return -1;
			} else if ((Z_LVAL_P(retval) > 0) && (Z_TYPE_P(subpats) == IS_ARRAY)) {
				
				/* Need to fetch global match which equals pmatch[0] */
				HashTable *ht = Z_ARRVAL_P(subpats);
				HashPosition outer_pos;
				zval *pattern_match = NULL, *pattern_offset = NULL;
				
				zend_hash_internal_pointer_reset_ex(ht, &outer_pos); 
				
				if (zend_hash_has_more_elements_ex(ht, &outer_pos) == SUCCESS &&
					zend_hash_move_forward_ex(ht, &outer_pos)) {
					
					zval **ppzval;
					
					/* The first element (should be) is the global match 
					   Need to move to the inner array to get the global match */
					
					if (zend_hash_get_current_data_ex(ht, (void**)&ppzval, &outer_pos) != FAILURE) { 
						
						HashTable *inner_ht;
						HashPosition inner_pos;
						zval **match, **offset;
						zval tmpcopy = **ppzval, matchcopy, offsetcopy;
						
						zval_copy_ctor(&tmpcopy); 
						INIT_PZVAL(&tmpcopy);
						
						inner_ht = Z_ARRVAL(tmpcopy);
						
						/* If everything goes according to the master plan
						   tmpcopy now contains two elements:
						   0 = the match
						   1 = starting position of the match */
						zend_hash_internal_pointer_reset_ex(inner_ht, &inner_pos); 
						
						if (zend_hash_has_more_elements_ex(inner_ht, &inner_pos) == SUCCESS &&
							zend_hash_move_forward_ex(inner_ht, &inner_pos)) {
						
							if (zend_hash_get_current_data_ex(inner_ht, (void**)&match, &inner_pos) != FAILURE) { 
									
								matchcopy = **match;
								zval_copy_ctor(&matchcopy);
								INIT_PZVAL(&matchcopy);
								convert_to_string(&matchcopy); 
								
								MAKE_STD_ZVAL(pattern_match);
								Z_STRVAL_P(pattern_match) = (char *)Z_STRVAL(matchcopy);
								Z_STRLEN_P(pattern_match) = Z_STRLEN(matchcopy);
								Z_TYPE_P(pattern_match) = IS_STRING; 

								zval_dtor(&matchcopy);
							}
						}
						
						if (zend_hash_has_more_elements_ex(inner_ht, &inner_pos) == SUCCESS &&
							zend_hash_move_forward_ex(inner_ht, &inner_pos)) {
							
							if (zend_hash_get_current_data_ex(inner_ht, (void**)&offset, &inner_pos) != FAILURE) { 
								
								offsetcopy = **offset;
								zval_copy_ctor(&offsetcopy);
								INIT_PZVAL(&offsetcopy);
								convert_to_long(&offsetcopy); 
								
								MAKE_STD_ZVAL(pattern_offset);
								Z_LVAL_P(pattern_offset) = Z_LVAL(offsetcopy);
								Z_TYPE_P(pattern_offset) = IS_LONG;
								
								zval_dtor(&offsetcopy);
							}
						}
						zval_dtor(&tmpcopy); 	
					}
					
					if ((pattern_match != NULL) && (pattern_offset != NULL)) {
						ms->search.s += (int)Z_LVAL_P(pattern_offset); /* this is where the match starts */
						ms->search.offset += (size_t)Z_LVAL_P(pattern_offset); /* this is where the match starts as size_t */
						ms->search.rm_len = Z_STRLEN_P(pattern_match) /* This is the length of the matched pattern */;
						v = 0;
						
						efree(pattern_match);
						efree(pattern_offset);
						
					} else {
						zval_ptr_dtor(&subpats);
						FREE_ZVAL(retval);
						zval_dtor(pattern);
						FREE_ZVAL(pattern);
						return -1;
					}					
				}

				
			} else {
				v = 1;
			}
			zval_ptr_dtor(&subpats);
			FREE_ZVAL(retval);
		}
		zval_dtor(pattern);
		FREE_ZVAL(pattern);
		break;	
	}
	case FILE_INDIRECT:
		return 1;	 
	default:
		file_magerror(ms, "invalid type %d in magiccheck()", m->type);
		return -1;
	}

	v = file_signextend(ms, m, v);

	switch (m->reln) {
	case 'x':
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu == *any* = 1\n",
			    (uint64_t)v);
		matched = 1;
		break;

	case '!':
		matched = v != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu != %llu = %d\n",
			    (uint64_t)v, (uint64_t)l,
			    matched);
		break;

	case '=':
		matched = v == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu == %llu = %d\n",
			    (uint64_t)v, (uint64_t)l,
			    matched);
		break;

	case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%llu > %llu = %d\n",
				    (uint64_t)v,
				    (uint64_t)l, matched);
		}
		else {
			matched = (int64_t) v > (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%lld > %lld = %d\n",
				    (uint64_t)v, (uint64_t)l, matched);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%llu < %llu = %d\n",
				    (uint64_t)v,
				    (uint64_t)l, matched);
		}
		else {
			matched = (int64_t) v < (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%lld < %lld = %d\n",
				       (int64_t)v, (int64_t)l, matched);
		}
		break;

	case '&':
		matched = (v & l) == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%llx & %llx) == %llx) = %d\n",
			    (uint64_t)v, (uint64_t)l,
			    (uint64_t)l, matched);
		break;

	case '^':
		matched = (v & l) != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%llx & %llx) != %llx) = %d\n",
			    (uint64_t)v, (uint64_t)l,
			    (uint64_t)l, matched);
		break;

	default:
		matched = 0;
		file_magerror(ms, "cannot happen: invalid relation `%c'",
		    m->reln);
		return -1;
	}

	return matched;
}

private int
handle_annotation(struct magic_set *ms, struct magic *m)
{
	if (ms->flags & MAGIC_APPLE) {
		if (file_printf(ms, "%.8s", m->apple) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_MIME_TYPE) && m->mimetype[0]) {
		if (file_printf(ms, "%s", m->mimetype) == -1)
			return -1;
		return 1;
	}
	return 0;
}

private int
print_sep(struct magic_set *ms, int firstline)
{
	if (ms->flags & MAGIC_MIME)
		return 0;
	if (firstline)
		return 0;
	/*
	 * we found another match
	 * put a newline and '-' to do some simple formatting
	 */
	return file_printf(ms, "\n- ");
}
