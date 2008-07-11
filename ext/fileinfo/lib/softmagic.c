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
#include "magic.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>


#ifndef	lint
FILE_RCSID("@(#)$File: softmagic.c,v 1.117 2008/03/01 22:21:49 rrt Exp $")
#endif	/* lint */

private int match(struct magic_set *, struct magic *, uint32_t,
    const unsigned char *, size_t, int);
private int mget(struct magic_set *, const unsigned char *,
    struct magic *, size_t, unsigned int);
private int magiccheck(struct magic_set *, struct magic *);
private int32_t mprint(struct magic_set *, struct magic *);
private void mdebug(uint32_t, const char *, size_t);
private int mcopy(struct magic_set *, union VALUETYPE *, int, int,
    const unsigned char *, uint32_t, size_t, size_t);
private int mconvert(struct magic_set *, struct magic *);
private int print_sep(struct magic_set *, int);
private void cvt_8(union VALUETYPE *, const struct magic *);
private void cvt_16(union VALUETYPE *, const struct magic *);
private void cvt_32(union VALUETYPE *, const struct magic *);
private void cvt_64(union VALUETYPE *, const struct magic *);

/*
 * Macro to give description string according to whether we want plain
 * text or MIME type
 */
#define MAGIC_DESC ((ms->flags & MAGIC_MIME) ? m->mimetype : m->desc)

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
	int returnval = 0; /* if a match is found it is set to 1*/
	int firstline = 1; /* a flag to print X\n  X\n- X */
	int printed_something = 0;

	if (file_check_mem(ms, cont_level) == -1)
		return -1;

	for (magindex = 0; magindex < nmagic; magindex++) {
		int flush;
		struct magic *m = &magic[magindex];

		if ((m->flag & BINTEST) != mode) {
			/* Skip sub-tests */
			while (magic[magindex + 1].cont_level != 0 &&
			       ++magindex < nmagic)
				continue;
			continue; /* Skip to next top-level test*/
		}

		ms->offset = m->offset;
		ms->line = m->lineno;

		/* if main entry matches, print it... */
		flush = !mget(ms, s, m, nbytes, cont_level);
		if (flush) {
			if (m->reln == '!')
				flush = 0;
		} else {	
			switch (magiccheck(ms, m)) {
			case -1:
				return -1;
			case 0:
				flush++;
				break;
			default:
				break;
			}
		}
		if (flush) {
			/* 
			 * main entry didn't match,
			 * flush its continuations
			 */
			while (magindex < nmagic - 1 &&
			    magic[magindex + 1].cont_level != 0)
				magindex++;
			continue;
		}

		/*
		 * If we are going to print something, we'll need to print
		 * a blank before we print something else.
		 */
		if (*MAGIC_DESC) {
			need_separator = 1;
			printed_something = 1;
			if (print_sep(ms, firstline) == -1)
				return -1;
		}

		if ((ms->c.li[cont_level].off = mprint(ms, m)) == -1)
			return -1;

		/* and any continuations that match */
		if (file_check_mem(ms, ++cont_level) == -1)
			return -1;

		while (magic[magindex+1].cont_level != 0 &&
		    ++magindex < nmagic) {
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
			flush = !mget(ms, s, m, nbytes, cont_level);
			if (flush && m->reln != '!')
				continue;
				
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
				if (*MAGIC_DESC) {
					printed_something = 1;
					if (print_sep(ms, firstline) == -1)
						return -1;
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
				    && *MAGIC_DESC) {
					if (file_printf(ms, " ") == -1)
						return -1;
					need_separator = 0;
				}
				if ((ms->c.li[cont_level].off = mprint(ms, m)) == -1)
					return -1;
				if (*MAGIC_DESC)
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
			returnval = 1;
		}
		if ((ms->flags & MAGIC_CONTINUE) == 0 && printed_something) {
			return 1; /* don't keep searching */
		}			
	}
	return returnval;  /* This is hit if -k is set or there is no match */
}

private int
check_fmt(struct magic_set *ms, struct magic *m)
{
	regex_t rx;
	int rc;

	if (strchr(MAGIC_DESC, '%') == NULL)
		return 0;

	rc = regcomp(&rx, "%[-0-9\\.]*s", REG_EXTENDED|REG_NOSUB);
	if (rc) {
		char errmsg[512];
		(void)regerror(rc, &rx, errmsg, sizeof(errmsg));
		file_magerror(ms, "regex error %d, (%s)", rc, errmsg);
		return -1;
	} else {
		rc = regexec(&rx, MAGIC_DESC, 0, 0, 0);
		regfree(&rx);
		return !rc;
	}
}

#ifndef HAVE_STRNDUP
char * strndup(const char *, size_t);

char *
strndup(const char *str, size_t n)
{
	size_t len;
	char *copy;

	len = strlen(str);
	if (len > n)
		len = n;
	if (!(copy = malloc(len + 1)))
		return (NULL);
	(void) memcpy(copy, str, len + 1);
	copy[len] = '\0';
	return (copy);
}
#endif /* HAVE_STRNDUP */

private int32_t
mprint(struct magic_set *ms, struct magic *m)
{
	uint64_t v;
	float vf;
	double vd;
	int64_t t = 0;
 	char *buf;
	union VALUETYPE *p = &ms->ms_value;

  	switch (m->type) {
  	case FILE_BYTE:
		v = file_signextend(ms, m, (uint64_t)p->b);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			if (asprintf(&buf, "%c", (unsigned char)v) < 0)
				return -1;
			if (file_printf(ms, MAGIC_DESC, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, MAGIC_DESC, (unsigned char) v) == -1)
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
			if (asprintf(&buf, "%hu", (unsigned short)v) < 0)
				return -1;
			if (file_printf(ms, MAGIC_DESC, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, MAGIC_DESC, (unsigned short) v) == -1)
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
			if (asprintf(&buf, "%u", (uint32_t)v) < 0)
				return -1;
			if (file_printf(ms, MAGIC_DESC, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, MAGIC_DESC, (uint32_t) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(int32_t);
  		break;

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		v = file_signextend(ms, m, p->q);
		if (file_printf(ms, MAGIC_DESC, (uint64_t) v) == -1)
			return -1;
		t = ms->offset + sizeof(int64_t);
  		break;

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!') {
			if (file_printf(ms, MAGIC_DESC, m->value.s) == -1)
				return -1;
			t = ms->offset + m->vallen;
		}
		else {
			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\n")] = '\0';
			if (file_printf(ms, MAGIC_DESC, p->s) == -1)
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
		if (file_printf(ms, MAGIC_DESC, file_fmttime(p->l, 1)) == -1)
			return -1;
		t = ms->offset + sizeof(time_t);
		break;

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		if (file_printf(ms, MAGIC_DESC, file_fmttime(p->l, 0)) == -1)
			return -1;
		t = ms->offset + sizeof(time_t);
		break;

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		if (file_printf(ms, MAGIC_DESC, file_fmttime((uint32_t)p->q, 1))
		    == -1)
			return -1;
		t = ms->offset + sizeof(uint64_t);
		break;

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		if (file_printf(ms, MAGIC_DESC, file_fmttime((uint32_t)p->q, 0))
		    == -1)
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
			if (asprintf(&buf, "%g", vf) < 0)
				return -1;
			if (file_printf(ms, MAGIC_DESC, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, MAGIC_DESC, vf) == -1)
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
			if (asprintf(&buf, "%g", vd) < 0)
				return -1;
			if (file_printf(ms, MAGIC_DESC, buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, MAGIC_DESC, vd) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(double);
  		break;

	case FILE_REGEX: {
		char *cp;
		int rval;

		cp = strndup((const char *)ms->search.s, ms->search.rm_len);
		if (cp == NULL) {
			file_oomem(ms, ms->search.rm_len);
			return -1;
		}
		rval = file_printf(ms, MAGIC_DESC, cp);
		free(cp);

		if (rval == -1)
			return -1;

		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + ms->search.rm_len;
		break;
	}

	case FILE_SEARCH:
	  	if (file_printf(ms, MAGIC_DESC, m->value.s) == -1)
			return -1;
		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + m->vallen;
		break;

	case FILE_DEFAULT:
	  	if (file_printf(ms, MAGIC_DESC, m->value.s) == -1)
			return -1;
		t = ms->offset;
		break;

	default:
		file_magerror(ms, "invalid m->type (%d) in mprint()", m->type);
		return -1;
	}
	return(t);
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
		size_t len;
		
		/* Null terminate and eat *trailing* return */
		p->s[sizeof(p->s) - 1] = '\0';
		len = strlen(p->s);
		if (len-- && p->s[len] == '\n')
			p->s[len] = '\0';
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
		len = strlen(p->s);
		if (len-- && p->s[len] == '\n')
			p->s[len] = '\0';
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
			size_t lines;

			if (s == NULL) {
				ms->search.s_len = 0;
				ms->search.s = NULL;
				return 0;
			}
			buf = (const char *)s + offset;
			last = (const char *)s + nbytes;
			/* mget() guarantees buf <= last */
			for (lines = linecnt, b = buf;
			     lines && ((b = strchr(c = b, '\n')) || (b = strchr(c, '\r')));
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
				file_magerror(ms, "invalid offset %zu in mcopy()",
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
		file_mdump(m);
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
				off = (int32_t)((q->hl[0]<<24)|(q->hl[1]<<16)|
						 (q->hl[2]<<8)|(q->hl[3]));
				break;
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

		if (m->flag & INDIROFFADD)
			offset += ms->c.li[cont_level-1].off;
		if (mcopy(ms, p, m->type, 0, s, offset, nbytes, count) == -1)
			return -1;
		ms->offset = offset;

		if ((ms->flags & MAGIC_DEBUG) != 0) {
			mdebug(offset, (char *)(void *)p,
			    sizeof(union VALUETYPE));
			file_mdump(m);
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
			file_magerror(ms, "cannot happen with float: invalid relation `%c'", m->reln);
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
		int rc;
		regex_t rx;
		char errmsg[512];

		if (ms->search.s == NULL)
			return 0;

		l = 0;
		rc = regcomp(&rx, m->value.s,
		    REG_EXTENDED|REG_NEWLINE|
		    ((m->str_flags & STRING_IGNORE_CASE) ? REG_ICASE : 0));
		if (rc) {
			(void)regerror(rc, &rx, errmsg, sizeof(errmsg));
			file_magerror(ms, "regex error %d, (%s)",
			    rc, errmsg);
			v = (uint64_t)-1;
		}
		else {
			regmatch_t pmatch[1];
#ifndef REG_STARTEND
#define	REG_STARTEND	0
			size_t l = ms->search.s_len - 1;
			char c = ms->search.s[l];
			((char *)(intptr_t)ms->search.s)[l] = '\0';
#else
			pmatch[0].rm_so = 0;
			pmatch[0].rm_eo = ms->search.s_len;
#endif
			rc = regexec(&rx, (const char *)ms->search.s,
			    1, pmatch, REG_STARTEND);
#if REG_STARTEND == 0
			((char *)(intptr_t)ms->search.s)[l] = c;
#endif
			switch (rc) {
			case 0:
				ms->search.s += (int)pmatch[0].rm_so;
				ms->search.offset += (size_t)pmatch[0].rm_so;
				ms->search.rm_len =
				    (size_t)(pmatch[0].rm_eo - pmatch[0].rm_so);
				v = 0;
				break;

			case REG_NOMATCH:
				v = 1;
				break;

			default:
				(void)regerror(rc, &rx, errmsg, sizeof(errmsg));
				file_magerror(ms, "regexec error %d, (%s)",
				    rc, errmsg);
				v = (uint64_t)-1;
				break;
			}
			regfree(&rx);
		}
		if (v == (uint64_t)-1)
			return -1;
		break;
	}
	default:
		file_magerror(ms, "invalid type %d in magiccheck()", m->type);
		return -1;
	}

	v = file_signextend(ms, m, v);

	switch (m->reln) {
	case 'x':
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu == *any* = 1\n",
			    (unsigned long long)v);
		matched = 1;
		break;

	case '!':
		matched = v != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu != %llu = %d\n",
			    (unsigned long long)v, (unsigned long long)l,
			    matched);
		break;

	case '=':
		matched = v == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%llu == %llu = %d\n",
			    (unsigned long long)v, (unsigned long long)l,
			    matched);
		break;

	case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%llu > %llu = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v > (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%lld > %lld = %d\n",
				    (long long)v, (long long)l, matched);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%llu < %llu = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v < (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%lld < %lld = %d\n",
				       (long long)v, (long long)l, matched);
		}
		break;

	case '&':
		matched = (v & l) == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%llx & %llx) == %llx) = %d\n",
			    (unsigned long long)v, (unsigned long long)l,
			    (unsigned long long)l, matched);
		break;

	case '^':
		matched = (v & l) != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%llx & %llx) != %llx) = %d\n",
			    (unsigned long long)v, (unsigned long long)l,
			    (unsigned long long)l, matched);
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
print_sep(struct magic_set *ms, int firstline)
{
	if (firstline)
		return 0;
	/*
	 * we found another match 
	 * put a newline and '-' to do some simple formatting
	 */
	return file_printf(ms, "\n- ");
}
