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
 * apprentice - make one pass through /etc/magic, learning its secrets.
 */

#include "php.h"

#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: apprentice.c,v 1.301 2021/02/23 00:51:11 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <stdlib.h>

#if defined(__hpux) && !defined(HAVE_STRTOULL)
#if SIZEOF_LONG == 8
# define strtoull strtoul
#else
# define strtoull __strtoull
#endif
#endif

#ifdef PHP_WIN32
#include "win32/unistd.h"
#define strtoull _strtoui64
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

#ifndef SSIZE_MAX
#define MAXMAGIC_SIZE        ((ssize_t)0x7fffffff)
#else
#define MAXMAGIC_SIZE        SSIZE_MAX
#endif


#define	EATAB {while (isascii(CAST(unsigned char, *l)) && \
		      isspace(CAST(unsigned char, *l)))  ++l;}
#define LOWCASE(l) (isupper(CAST(unsigned char, l)) ? \
			tolower(CAST(unsigned char, l)) : (l))
/*
 * Work around a bug in headers on Digital Unix.
 * At least confirmed for: OSF1 V4.0 878
 */
#if defined(__osf__) && defined(__DECC)
#ifdef MAP_FAILED
#undef MAP_FAILED
#endif
#endif

#ifndef offsetof
#define offsetof(STRUCTURE,FIELD) ((int)((char*)&((STRUCTURE*)0)->FIELD))
#endif

#ifndef MAP_FAILED
#define MAP_FAILED (void *) -1
#endif

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#define ALLOC_CHUNK	CAST(size_t, 10)
#define ALLOC_INCR	CAST(size_t, 200)

#define MAP_TYPE_USER	0
#define MAP_TYPE_MALLOC	1
#define MAP_TYPE_MMAP	2

struct magic_entry {
	struct magic *mp;
	uint32_t cont_count;
	uint32_t max_count;
};

struct magic_entry_set {
	struct magic_entry *me;
	uint32_t count;
	uint32_t max;
};

struct magic_map {
	void *p;
	size_t len;
	int type;
	struct magic *magic[MAGIC_SETS];
	uint32_t nmagic[MAGIC_SETS];
};

int file_formats[FILE_NAMES_SIZE];
const size_t file_nformats = FILE_NAMES_SIZE;
const char *file_names[FILE_NAMES_SIZE];
const size_t file_nnames = FILE_NAMES_SIZE;

private int getvalue(struct magic_set *ms, struct magic *, const char **, int);
private int hextoint(int);
private const char *getstr(struct magic_set *, struct magic *, const char *,
    int);
private int parse(struct magic_set *, struct magic_entry *, const char *,
    size_t, int);
private void eatsize(const char **);
private int apprentice_1(struct magic_set *, const char *, int);
private size_t apprentice_magic_strength(const struct magic *);
private int apprentice_sort(const void *, const void *);
private void apprentice_list(struct mlist *, int );
private struct magic_map *apprentice_load(struct magic_set *,
    const char *, int);
private struct mlist *mlist_alloc(void);
private void mlist_free_all(struct magic_set *);
private void mlist_free(struct mlist *);
private void byteswap(struct magic *, uint32_t);
private void bs1(struct magic *);
private uint16_t swap2(uint16_t);
private uint32_t swap4(uint32_t);
private uint64_t swap8(uint64_t);
private char *mkdbname(struct magic_set *, const char *, int);
private struct magic_map *apprentice_map(struct magic_set *, const char *);
private void apprentice_unmap(struct magic_map *);
private int apprentice_compile(struct magic_set *, struct magic_map *,
    const char *);
private int check_format_type(const char *, int, const char **);
private int check_format(struct magic_set *, struct magic *);
private int get_op(char);
private int parse_mime(struct magic_set *, struct magic_entry *, const char *,
    size_t);
private int parse_strength(struct magic_set *, struct magic_entry *,
    const char *, size_t);
private int parse_apple(struct magic_set *, struct magic_entry *, const char *,
    size_t);
private int parse_ext(struct magic_set *, struct magic_entry *, const char *,
    size_t);


private size_t magicsize = sizeof(struct magic);

private const char usg_hdr[] = "cont\toffset\ttype\topcode\tmask\tvalue\tdesc";

private struct {
	const char *name;
	size_t len;
	int (*fun)(struct magic_set *, struct magic_entry *, const char *,
	    size_t);
} bang[] = {
#define	DECLARE_FIELD(name) { # name, sizeof(# name) - 1, parse_ ## name }
	DECLARE_FIELD(mime),
	DECLARE_FIELD(apple),
	DECLARE_FIELD(ext),
	DECLARE_FIELD(strength),
#undef	DECLARE_FIELD
	{ NULL, 0, NULL }
};

#include "../data_file.c"

struct type_tbl_s {
	const char name[16];
	const size_t len;
	const int type;
	const int format;
};

/*
 * XXX - the actual Single UNIX Specification says that "long" means "long",
 * as in the C data type, but we treat it as meaning "4-byte integer".
 * Given that the OS X version of file 5.04 did the same, I guess that passes
 * the actual test; having "long" be dependent on how big a "long" is on
 * the machine running "file" is silly.
 */
static const struct type_tbl_s type_tbl[] = {
# define XX(s)		s, (sizeof(s) - 1)
# define XX_NULL	"", 0
	{ XX("invalid"),	FILE_INVALID,		FILE_FMT_NONE },
	{ XX("byte"),		FILE_BYTE,		FILE_FMT_NUM },
	{ XX("short"),		FILE_SHORT,		FILE_FMT_NUM },
	{ XX("default"),	FILE_DEFAULT,		FILE_FMT_NONE },
	{ XX("long"),		FILE_LONG,		FILE_FMT_NUM },
	{ XX("string"),		FILE_STRING,		FILE_FMT_STR },
	{ XX("date"),		FILE_DATE,		FILE_FMT_STR },
	{ XX("beshort"),	FILE_BESHORT,		FILE_FMT_NUM },
	{ XX("belong"),		FILE_BELONG,		FILE_FMT_NUM },
	{ XX("bedate"),		FILE_BEDATE,		FILE_FMT_STR },
	{ XX("leshort"),	FILE_LESHORT,		FILE_FMT_NUM },
	{ XX("lelong"),		FILE_LELONG,		FILE_FMT_NUM },
	{ XX("ledate"),		FILE_LEDATE,		FILE_FMT_STR },
	{ XX("pstring"),	FILE_PSTRING,		FILE_FMT_STR },
	{ XX("ldate"),		FILE_LDATE,		FILE_FMT_STR },
	{ XX("beldate"),	FILE_BELDATE,		FILE_FMT_STR },
	{ XX("leldate"),	FILE_LELDATE,		FILE_FMT_STR },
	{ XX("regex"),		FILE_REGEX,		FILE_FMT_STR },
	{ XX("bestring16"),	FILE_BESTRING16,	FILE_FMT_STR },
	{ XX("lestring16"),	FILE_LESTRING16,	FILE_FMT_STR },
	{ XX("search"),		FILE_SEARCH,		FILE_FMT_STR },
	{ XX("medate"),		FILE_MEDATE,		FILE_FMT_STR },
	{ XX("meldate"),	FILE_MELDATE,		FILE_FMT_STR },
	{ XX("melong"),		FILE_MELONG,		FILE_FMT_NUM },
	{ XX("quad"),		FILE_QUAD,		FILE_FMT_QUAD },
	{ XX("lequad"),		FILE_LEQUAD,		FILE_FMT_QUAD },
	{ XX("bequad"),		FILE_BEQUAD,		FILE_FMT_QUAD },
	{ XX("qdate"),		FILE_QDATE,		FILE_FMT_STR },
	{ XX("leqdate"),	FILE_LEQDATE,		FILE_FMT_STR },
	{ XX("beqdate"),	FILE_BEQDATE,		FILE_FMT_STR },
	{ XX("qldate"),		FILE_QLDATE,		FILE_FMT_STR },
	{ XX("leqldate"),	FILE_LEQLDATE,		FILE_FMT_STR },
	{ XX("beqldate"),	FILE_BEQLDATE,		FILE_FMT_STR },
	{ XX("float"),		FILE_FLOAT,		FILE_FMT_FLOAT },
	{ XX("befloat"),	FILE_BEFLOAT,		FILE_FMT_FLOAT },
	{ XX("lefloat"),	FILE_LEFLOAT,		FILE_FMT_FLOAT },
	{ XX("double"),		FILE_DOUBLE,		FILE_FMT_DOUBLE },
	{ XX("bedouble"),	FILE_BEDOUBLE,		FILE_FMT_DOUBLE },
	{ XX("ledouble"),	FILE_LEDOUBLE,		FILE_FMT_DOUBLE },
	{ XX("leid3"),		FILE_LEID3,		FILE_FMT_NUM },
	{ XX("beid3"),		FILE_BEID3,		FILE_FMT_NUM },
	{ XX("indirect"),	FILE_INDIRECT,		FILE_FMT_NUM },
	{ XX("qwdate"),		FILE_QWDATE,		FILE_FMT_STR },
	{ XX("leqwdate"),	FILE_LEQWDATE,		FILE_FMT_STR },
	{ XX("beqwdate"),	FILE_BEQWDATE,		FILE_FMT_STR },
	{ XX("name"),		FILE_NAME,		FILE_FMT_NONE },
	{ XX("use"),		FILE_USE,		FILE_FMT_NONE },
	{ XX("clear"),		FILE_CLEAR,		FILE_FMT_NONE },
	{ XX("der"),		FILE_DER,		FILE_FMT_STR },
	{ XX("guid"),		FILE_GUID,		FILE_FMT_STR },
	{ XX("offset"),		FILE_OFFSET,		FILE_FMT_QUAD },
	{ XX_NULL,		FILE_INVALID,		FILE_FMT_NONE },
};

/*
 * These are not types, and cannot be preceded by "u" to make them
 * unsigned.
 */
static const struct type_tbl_s special_tbl[] = {
	{ XX("der"),		FILE_DER,		FILE_FMT_STR },
	{ XX("name"),		FILE_NAME,		FILE_FMT_STR },
	{ XX("use"),		FILE_USE,		FILE_FMT_STR },
	{ XX_NULL,		FILE_INVALID,		FILE_FMT_NONE },
};
# undef XX
# undef XX_NULL

private int
get_type(const struct type_tbl_s *tbl, const char *l, const char **t)
{
	const struct type_tbl_s *p;

	for (p = tbl; p->len; p++) {
		if (strncmp(l, p->name, p->len) == 0) {
			if (t)
				*t = l + p->len;
			break;
		}
	}
	return p->type;
}

private off_t
maxoff_t(void) {
	if (/*CONSTCOND*/sizeof(off_t) == sizeof(int))
		return CAST(off_t, INT_MAX);
	if (/*CONSTCOND*/sizeof(off_t) == sizeof(long))
		return CAST(off_t, LONG_MAX);
	return 0x7fffffff;
}

private int
get_standard_integer_type(const char *l, const char **t)
{
	int type;

	if (isalpha(CAST(unsigned char, l[1]))) {
		switch (l[1]) {
		case 'C':
			/* "dC" and "uC" */
			type = FILE_BYTE;
			break;
		case 'S':
			/* "dS" and "uS" */
			type = FILE_SHORT;
			break;
		case 'I':
		case 'L':
			/*
			 * "dI", "dL", "uI", and "uL".
			 *
			 * XXX - the actual Single UNIX Specification says
			 * that "L" means "long", as in the C data type,
			 * but we treat it as meaning "4-byte integer".
			 * Given that the OS X version of file 5.04 did
			 * the same, I guess that passes the actual SUS
			 * validation suite; having "dL" be dependent on
			 * how big a "long" is on the machine running
			 * "file" is silly.
			 */
			type = FILE_LONG;
			break;
		case 'Q':
			/* "dQ" and "uQ" */
			type = FILE_QUAD;
			break;
		default:
			/* "d{anything else}", "u{anything else}" */
			return FILE_INVALID;
		}
		l += 2;
	} else if (isdigit(CAST(unsigned char, l[1]))) {
		/*
		 * "d{num}" and "u{num}"; we only support {num} values
		 * of 1, 2, 4, and 8 - the Single UNIX Specification
		 * doesn't say anything about whether arbitrary
		 * values should be supported, but both the Solaris 10
		 * and OS X Mountain Lion versions of file passed the
		 * Single UNIX Specification validation suite, and
		 * neither of them support values bigger than 8 or
		 * non-power-of-2 values.
		 */
		if (isdigit(CAST(unsigned char, l[2]))) {
			/* Multi-digit, so > 9 */
			return FILE_INVALID;
		}
		switch (l[1]) {
		case '1':
			type = FILE_BYTE;
			break;
		case '2':
			type = FILE_SHORT;
			break;
		case '4':
			type = FILE_LONG;
			break;
		case '8':
			type = FILE_QUAD;
			break;
		default:
			/* XXX - what about 3, 5, 6, or 7? */
			return FILE_INVALID;
		}
		l += 2;
	} else {
		/*
		 * "d" or "u" by itself.
		 */
		type = FILE_LONG;
		++l;
	}
	if (t)
		*t = l;
	return type;
}

private void
init_file_tables(void)
{
	static int done = 0;
	const struct type_tbl_s *p;

	if (done)
		return;
	done++;

	for (p = type_tbl; p->len; p++) {
		assert(p->type < FILE_NAMES_SIZE);
		file_names[p->type] = p->name;
		file_formats[p->type] = p->format;
	}
	assert(p - type_tbl == FILE_NAMES_SIZE);
}

private int
add_mlist(struct mlist *mlp, struct magic_map *map, size_t idx)
{
	struct mlist *ml;

	mlp->map = NULL;
	if ((ml = CAST(struct mlist *, emalloc(sizeof(*ml)))) == NULL)
		return -1;

	ml->map = idx == 0 ? map : NULL;
	ml->magic = map->magic[idx];
	ml->nmagic = map->nmagic[idx];

	mlp->prev->next = ml;
	ml->prev = mlp->prev;
	ml->next = mlp;
	mlp->prev = ml;
	return 0;
}

/*
 * Handle one file or directory.
 */
private int
apprentice_1(struct magic_set *ms, const char *fn, int action)
{
	struct magic_map *map;
#ifndef COMPILE_ONLY
	struct mlist *ml;
	size_t i;
#endif

	if (magicsize != FILE_MAGICSIZE) {
		file_error(ms, 0, "magic element size %lu != %lu",
		    CAST(unsigned long, sizeof(*map->magic[0])),
		    CAST(unsigned long, FILE_MAGICSIZE));
		return -1;
	}

	if (action == FILE_COMPILE) {
		map = apprentice_load(ms, fn, action);
		if (map == NULL)
			return -1;
		return apprentice_compile(ms, map, fn);
	}

#ifndef COMPILE_ONLY
	map = apprentice_map(ms, fn);
	if (map == NULL) {
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "using regular magic file `%s'", fn);
		map = apprentice_load(ms, fn, action);
		if (map == NULL)
			return -1;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		if (add_mlist(ms->mlist[i], map, i) == -1) {
			/* failed to add to any list, free explicitly */
			if (i == 0)
				apprentice_unmap(map);
			else
				mlist_free_all(ms);
			file_oomem(ms, sizeof(*ml));
			return -1;
		}
	}

	if (action == FILE_LIST) {
		for (i = 0; i < MAGIC_SETS; i++) {
			printf("Set %" SIZE_T_FORMAT "u:\nBinary patterns:\n",
			    i);
			apprentice_list(ms->mlist[i], BINTEST);
			printf("Text patterns:\n");
			apprentice_list(ms->mlist[i], TEXTTEST);
		}
	}
	return 0;
#else
	return 0;
#endif /* COMPILE_ONLY */
}

protected void
file_ms_free(struct magic_set *ms)
{
	size_t i;
	if (ms == NULL)
		return;
	for (i = 0; i < MAGIC_SETS; i++)
		mlist_free(ms->mlist[i]);
	if (ms->o.pbuf) {
		efree(ms->o.pbuf);
	}
	if (ms->o.buf) {
		efree(ms->o.buf);
	}
	if (ms->c.li) {
		efree(ms->c.li);
	}
	efree(ms);
}

protected struct magic_set *
file_ms_alloc(int flags)
{
	struct magic_set *ms;
	size_t i, len;

	if ((ms = CAST(struct magic_set *, ecalloc(CAST(size_t, 1u),
	    sizeof(struct magic_set)))) == NULL)
		return NULL;

	if (magic_setflags(ms, flags) == -1) {
		errno = EINVAL;
		goto free;
	}

	ms->o.buf = ms->o.pbuf = NULL;
	ms->o.blen = 0;
	len = (ms->c.len = 10) * sizeof(*ms->c.li);

	if ((ms->c.li = CAST(struct level_info *, emalloc(len))) == NULL)
		goto free;

	ms->event_flags = 0;
	ms->error = -1;
	for (i = 0; i < MAGIC_SETS; i++)
		ms->mlist[i] = NULL;
	ms->file = "unknown";
	ms->line = 0;
	ms->indir_max = FILE_INDIR_MAX;
	ms->name_max = FILE_NAME_MAX;
	ms->elf_shnum_max = FILE_ELF_SHNUM_MAX;
	ms->elf_phnum_max = FILE_ELF_PHNUM_MAX;
	ms->elf_notes_max = FILE_ELF_NOTES_MAX;
	ms->regex_max = FILE_REGEX_MAX;
	ms->bytes_max = FILE_BYTES_MAX;
	ms->encoding_max = FILE_ENCODING_MAX;
	return ms;
free:
	efree(ms);
	return NULL;
}

private void
apprentice_unmap(struct magic_map *map)
{
	if (map == NULL)
		return;
	if (map->p != php_magic_database) {
		if (map->p == NULL) {
			int j;
			for (j = 0; j < MAGIC_SETS; j++) {
				if (map->magic[j]) {
					efree(map->magic[j]);
				}
			}
		} else {
			efree(map->p);
		}
	}
	efree(map);
}

private struct mlist *
mlist_alloc(void)
{
	struct mlist *mlist;
	if ((mlist = CAST(struct mlist *, ecalloc(1, sizeof(*mlist)))) == NULL) {
		return NULL;
	}
	mlist->next = mlist->prev = mlist;
	return mlist;
}

private void
mlist_free_all(struct magic_set *ms)
{
	size_t i;

	for (i = 0; i < MAGIC_SETS; i++) {
		mlist_free(ms->mlist[i]);
		ms->mlist[i] = NULL;
	}
}

private void
mlist_free_one(struct mlist *ml)
{
	if (ml->map)
		apprentice_unmap(CAST(struct magic_map *, ml->map));
	efree(ml);
}

private void
mlist_free(struct mlist *mlist)
{
	struct mlist *ml, *next;

	if (mlist == NULL)
		return;

	for (ml = mlist->next; ml != mlist;) {
		next = ml->next;
		mlist_free_one(ml);
		ml = next;
	}
	mlist_free_one(mlist);
}

/* const char *fn: list of magic files and directories */
protected int
file_apprentice(struct magic_set *ms, const char *fn, int action)
{
	char *p, *mfn;
	int fileerr, errs = -1;
	size_t i, j;

	(void)file_reset(ms, 0);

/* XXX disabling default magic loading so the compiled in data is used */
#if 0
	if ((fn = magic_getpath(fn, action)) == NULL)
		return -1;
#endif

	init_file_tables();

	if (fn == NULL)
		fn = getenv("MAGIC");
	if (fn == NULL) {
		for (i = 0; i < MAGIC_SETS; i++) {
			mlist_free(ms->mlist[i]);
			if ((ms->mlist[i] = mlist_alloc()) == NULL) {
				file_oomem(ms, sizeof(*ms->mlist[i]));
				return -1;
			}
		}
		return apprentice_1(ms, fn, action);
	}

	if ((mfn = estrdup(fn)) == NULL) {
		file_oomem(ms, strlen(fn));
		return -1;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		mlist_free(ms->mlist[i]);
		if ((ms->mlist[i] = mlist_alloc()) == NULL) {
			file_oomem(ms, sizeof(*ms->mlist[i]));
			for (j = 0; j < i; j++) {
				mlist_free(ms->mlist[j]);
				ms->mlist[j] = NULL;
			}
			efree(mfn);
			return -1;
		}
	}
	fn = mfn;

	while (fn) {
		p = strchr(fn, PATHSEP);
		if (p)
			*p++ = '\0';
		if (*fn == '\0')
			break;
		fileerr = apprentice_1(ms, fn, action);
		errs = MAX(errs, fileerr);
		fn = p;
	}

	efree(mfn);

	if (errs == -1) {
		for (i = 0; i < MAGIC_SETS; i++) {
			mlist_free(ms->mlist[i]);
			ms->mlist[i] = NULL;
		}
		file_error(ms, 0, "could not find any valid magic files!");
		return -1;
	}

#if 0
	/*
	 * Always leave the database loaded
	 */
	if (action == FILE_LOAD)
		return 0;

	for (i = 0; i < MAGIC_SETS; i++) {
		mlist_free(ms->mlist[i]);
		ms->mlist[i] = NULL;
	}
#endif

	switch (action) {
	case FILE_LOAD:
	case FILE_COMPILE:
	case FILE_CHECK:
	case FILE_LIST:
		return 0;
	default:
		file_error(ms, 0, "Invalid action %d", action);
		return -1;
	}
}

/*
 * Compute the real length of a magic expression, for the purposes
 * of determining how "strong" a magic expression is (approximating
 * how specific its matches are):
 *	- magic characters count 0 unless escaped.
 *	- [] expressions count 1
 *	- {} expressions count 0
 *	- regular characters or escaped magic characters count 1
 *	- 0 length expressions count as one
 */
private size_t
nonmagic(const char *str)
{
	const char *p;
	size_t rv = 0;

	for (p = str; *p; p++)
		switch (*p) {
		case '\\':	/* Escaped anything counts 1 */
			if (!*++p)
				p--;
			rv++;
			continue;
		case '?':	/* Magic characters count 0 */
		case '*':
		case '.':
		case '+':
		case '^':
		case '$':
			continue;
		case '[':	/* Bracketed expressions count 1 the ']' */
			while (*p && *p != ']')
				p++;
			p--;
			continue;
		case '{':	/* Braced expressions count 0 */
			while (*p && *p != '}')
				p++;
			if (!*p)
				p--;
			continue;
		default:	/* Anything else counts 1 */
			rv++;
			continue;
		}

	return rv == 0 ? 1 : rv;	/* Return at least 1 */
}


private size_t
typesize(int type)
{
	switch (type) {
	case FILE_BYTE:
		return 1;

	case FILE_SHORT:
	case FILE_LESHORT:
	case FILE_BESHORT:
		return 2;

	case FILE_LONG:
	case FILE_LELONG:
	case FILE_BELONG:
	case FILE_MELONG:
		return 4;

	case FILE_DATE:
	case FILE_LEDATE:
	case FILE_BEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_LELDATE:
	case FILE_BELDATE:
	case FILE_MELDATE:
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		return 4;

	case FILE_QUAD:
	case FILE_BEQUAD:
	case FILE_LEQUAD:
	case FILE_QDATE:
	case FILE_LEQDATE:
	case FILE_BEQDATE:
	case FILE_QLDATE:
	case FILE_LEQLDATE:
	case FILE_BEQLDATE:
	case FILE_QWDATE:
	case FILE_LEQWDATE:
	case FILE_BEQWDATE:
	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
	case FILE_OFFSET:
		return 8;

	case FILE_GUID:
		return 16;

	default:
		return FILE_BADSIZE;
	}
}

/*
 * Get weight of this magic entry, for sorting purposes.
 */
private size_t
apprentice_magic_strength(const struct magic *m)
{
#define MULT 10U
	size_t ts, v;
	ssize_t val = 2 * MULT;	/* baseline strength */

	switch (m->type) {
	case FILE_DEFAULT:	/* make sure this sorts last */
		if (m->factor_op != FILE_FACTOR_OP_NONE)
			abort();
		return 0;

	case FILE_BYTE:
	case FILE_SHORT:
	case FILE_LESHORT:
	case FILE_BESHORT:
	case FILE_LONG:
	case FILE_LELONG:
	case FILE_BELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_LEDATE:
	case FILE_BEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_LELDATE:
	case FILE_BELDATE:
	case FILE_MELDATE:
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
	case FILE_QUAD:
	case FILE_BEQUAD:
	case FILE_LEQUAD:
	case FILE_QDATE:
	case FILE_LEQDATE:
	case FILE_BEQDATE:
	case FILE_QLDATE:
	case FILE_LEQLDATE:
	case FILE_BEQLDATE:
	case FILE_QWDATE:
	case FILE_LEQWDATE:
	case FILE_BEQWDATE:
	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
	case FILE_GUID:
	case FILE_OFFSET:
		ts = typesize(m->type);
		if (ts == FILE_BADSIZE)
			abort();
		val += ts * MULT;
		break;

	case FILE_PSTRING:
	case FILE_STRING:
		val += m->vallen * MULT;
		break;

	case FILE_BESTRING16:
	case FILE_LESTRING16:
		val += m->vallen * MULT / 2;
		break;

	case FILE_SEARCH:
		if (m->vallen == 0)
			break;
		val += m->vallen * MAX(MULT / m->vallen, 1);
		break;

	case FILE_REGEX:
		v = nonmagic(m->value.s);
		val += v * MAX(MULT / v, 1);
		break;

	case FILE_INDIRECT:
	case FILE_NAME:
	case FILE_USE:
		break;

	case FILE_DER:
		val += MULT;
		break;

	default:
		(void)fprintf(stderr, "Bad type %d\n", m->type);
		abort();
	}

	switch (m->reln) {
	case 'x':	/* matches anything penalize */
	case '!':       /* matches almost anything penalize */
		val = 0;
		break;

	case '=':	/* Exact match, prefer */
		val += MULT;
		break;

	case '>':
	case '<':	/* comparison match reduce strength */
		val -= 2 * MULT;
		break;

	case '^':
	case '&':	/* masking bits, we could count them too */
		val -= MULT;
		break;

	default:
		(void)fprintf(stderr, "Bad relation %c\n", m->reln);
		abort();
	}

	switch (m->factor_op) {
	case FILE_FACTOR_OP_NONE:
		break;
	case FILE_FACTOR_OP_PLUS:
		val += m->factor;
		break;
	case FILE_FACTOR_OP_MINUS:
		val -= m->factor;
		break;
	case FILE_FACTOR_OP_TIMES:
		val *= m->factor;
		break;
	case FILE_FACTOR_OP_DIV:
		val /= m->factor;
		break;
	default:
		abort();
	}

	if (val <= 0)	/* ensure we only return 0 for FILE_DEFAULT */
		val = 1;

	/*
	 * Magic entries with no description get a bonus because they depend
	 * on subsequent magic entries to print something.
	 */
	if (m->desc[0] == '\0')
		val++;
	return val;
}

/*
 * Sort callback for sorting entries by "strength" (basically length)
 */
private int
apprentice_sort(const void *a, const void *b)
{
	const struct magic_entry *ma = CAST(const struct magic_entry *, a);
	const struct magic_entry *mb = CAST(const struct magic_entry *, b);
	size_t sa = apprentice_magic_strength(ma->mp);
	size_t sb = apprentice_magic_strength(mb->mp);
	if (sa == sb)
		return 0;
	else if (sa > sb)
		return -1;
	else
		return 1;
}

/*
 * Shows sorted patterns list in the order which is used for the matching
 */
private void
apprentice_list(struct mlist *mlist, int mode)
{
	uint32_t magindex = 0;
	struct mlist *ml;
	for (ml = mlist->next; ml != mlist; ml = ml->next) {
		for (magindex = 0; magindex < ml->nmagic; magindex++) {
			struct magic *m = &ml->magic[magindex];
			if ((m->flag & mode) != mode) {
				/* Skip sub-tests */
				while (magindex + 1 < ml->nmagic &&
				       ml->magic[magindex + 1].cont_level != 0)
					++magindex;
				continue; /* Skip to next top-level test*/
			}

			/*
			 * Try to iterate over the tree until we find item with
			 * description/mimetype.
			 */
			while (magindex + 1 < ml->nmagic &&
			       ml->magic[magindex + 1].cont_level != 0 &&
			       *ml->magic[magindex].desc == '\0' &&
			       *ml->magic[magindex].mimetype == '\0')
				magindex++;

			printf("Strength = %3" SIZE_T_FORMAT "u@%u: %s [%s]\n",
			    apprentice_magic_strength(m),
			    ml->magic[magindex].lineno,
			    ml->magic[magindex].desc,
			    ml->magic[magindex].mimetype);
		}
	}
}

private void
set_test_type(struct magic *mstart, struct magic *m)
{
	switch (m->type) {
	case FILE_BYTE:
	case FILE_SHORT:
	case FILE_LONG:
	case FILE_DATE:
	case FILE_BESHORT:
	case FILE_BELONG:
	case FILE_BEDATE:
	case FILE_LESHORT:
	case FILE_LELONG:
	case FILE_LEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MEDATE:
	case FILE_MELDATE:
	case FILE_MELONG:
	case FILE_QUAD:
	case FILE_LEQUAD:
	case FILE_BEQUAD:
	case FILE_QDATE:
	case FILE_LEQDATE:
	case FILE_BEQDATE:
	case FILE_QLDATE:
	case FILE_LEQLDATE:
	case FILE_BEQLDATE:
	case FILE_QWDATE:
	case FILE_LEQWDATE:
	case FILE_BEQWDATE:
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
	case FILE_DER:
	case FILE_GUID:
	case FILE_OFFSET:
		mstart->flag |= BINTEST;
		break;
	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_BESTRING16:
	case FILE_LESTRING16:
		/* Allow text overrides */
		if (mstart->str_flags & STRING_TEXTTEST)
			mstart->flag |= TEXTTEST;
		else
			mstart->flag |= BINTEST;
		break;
	case FILE_REGEX:
	case FILE_SEARCH:
		/* Check for override */
		if (mstart->str_flags & STRING_BINTEST)
			mstart->flag |= BINTEST;
		if (mstart->str_flags & STRING_TEXTTEST)
			mstart->flag |= TEXTTEST;

		if (mstart->flag & (TEXTTEST|BINTEST))
			break;

		/* binary test if pattern is not text */
		if (file_looks_utf8(m->value.us, CAST(size_t, m->vallen), NULL,
		    NULL) <= 0)
			mstart->flag |= BINTEST;
		else
			mstart->flag |= TEXTTEST;
		break;
	case FILE_DEFAULT:
		/* can't deduce anything; we shouldn't see this at the
		   top level anyway */
		break;
	case FILE_INVALID:
	default:
		/* invalid search type, but no need to complain here */
		break;
	}
}

private int
addentry(struct magic_set *ms, struct magic_entry *me,
   struct magic_entry_set *mset)
{
	size_t i = me->mp->type == FILE_NAME ? 1 : 0;
	if (mset[i].count == mset[i].max) {
		struct magic_entry *mp;

		mset[i].max += ALLOC_INCR;
		if ((mp = CAST(struct magic_entry *,
		    erealloc(mset[i].me, sizeof(*mp) * mset[i].max))) ==
		    NULL) {
			file_oomem(ms, sizeof(*mp) * mset[i].max);
			return -1;
		}
		(void)memset(&mp[mset[i].count], 0, sizeof(*mp) *
		    ALLOC_INCR);
		mset[i].me = mp;
	}
	mset[i].me[mset[i].count++] = *me;
	memset(me, 0, sizeof(*me));
	return 0;
}

/*
 * Load and parse one file.
 */
private void
load_1(struct magic_set *ms, int action, const char *fn, int *errs,
   struct magic_entry_set *mset)
{
	char buffer[BUFSIZ + 1];
	char *line = NULL;
	size_t len;
	size_t lineno = 0;
	struct magic_entry me;

	php_stream *stream;


	ms->file = fn;
	stream = php_stream_open_wrapper((char *)fn, "rb", REPORT_ERRORS, NULL);

	if (stream == NULL) {
		if (errno != ENOENT)
			file_error(ms, errno, "cannot read magic file `%s'",
				   fn);
		(*errs)++;
		return;
	}

	memset(&me, 0, sizeof(me));
	/* read and parse this file */
	for (ms->line = 1; (line = php_stream_get_line(stream, buffer , BUFSIZ, &len)) != NULL; ms->line++) {
		if (len == 0) /* null line, garbage, etc */
			continue;
		if (line[len - 1] == '\n') {
			lineno++;
			line[len - 1] = '\0'; /* delete newline */
		}
		switch (line[0]) {
		case '\0':	/* empty, do not parse */
		case '#':	/* comment, do not parse */
			continue;
		case '!':
			if (line[1] == ':') {
				size_t i;

				for (i = 0; bang[i].name != NULL; i++) {
					if (CAST(size_t, len - 2) > bang[i].len &&
					    memcmp(bang[i].name, line + 2,
					    bang[i].len) == 0)
						break;
				}
				if (bang[i].name == NULL) {
					file_error(ms, 0,
					    "Unknown !: entry `%s'", line);
					(*errs)++;
					continue;
				}
				if (me.mp == NULL) {
					file_error(ms, 0,
					    "No current entry for :!%s type",
						bang[i].name);
					(*errs)++;
					continue;
				}
				if ((*bang[i].fun)(ms, &me,
				    line + bang[i].len + 2,
				    len - bang[i].len - 2) != 0) {
					(*errs)++;
					continue;
				}
				continue;
			}
			/*FALLTHROUGH*/
		default:
		again:
			switch (parse(ms, &me, line, lineno, action)) {
			case 0:
				continue;
			case 1:
				(void)addentry(ms, &me, mset);
				goto again;
			default:
				(*errs)++;
				break;
			}
		}
	}
	if (me.mp)
		(void)addentry(ms, &me, mset);
	efree(line);
	php_stream_close(stream);
}

/*
 * parse a file or directory of files
 * const char *fn: name of magic file or directory
 */
private int
cmpstrp(const void *p1, const void *p2)
{
        return strcmp(*RCAST(char *const *, p1), *RCAST(char *const *, p2));
}


private uint32_t
set_text_binary(struct magic_set *ms, struct magic_entry *me, uint32_t nme,
    uint32_t starttest)
{
	static const char text[] = "text";
	static const char binary[] = "binary";
	static const size_t len = sizeof(text);

	uint32_t i = starttest;

	do {
		set_test_type(me[starttest].mp, me[i].mp);
		if ((ms->flags & MAGIC_DEBUG) == 0)
			continue;
		(void)fprintf(stderr, "%s%s%s: %s\n",
		    me[i].mp->mimetype,
		    me[i].mp->mimetype[0] == '\0' ? "" : "; ",
		    me[i].mp->desc[0] ? me[i].mp->desc : "(no description)",
		    me[i].mp->flag & BINTEST ? binary : text);
		if (me[i].mp->flag & BINTEST) {
			char *p = strstr(me[i].mp->desc, text);
			if (p && (p == me[i].mp->desc ||
			    isspace(CAST(unsigned char, p[-1]))) &&
			    (p + len - me[i].mp->desc == MAXstring
			    || (p[len] == '\0' ||
			    isspace(CAST(unsigned char, p[len])))))
				(void)fprintf(stderr, "*** Possible "
				    "binary test for text type\n");
		}
	} while (++i < nme && me[i].mp->cont_level != 0);
	return i;
}

private void
set_last_default(struct magic_set *ms, struct magic_entry *me, uint32_t nme)
{
	uint32_t i;
	for (i = 0; i < nme; i++) {
		if (me[i].mp->cont_level == 0 &&
		    me[i].mp->type == FILE_DEFAULT) {
			while (++i < nme)
				if (me[i].mp->cont_level == 0)
					break;
			if (i != nme) {
				/* XXX - Ugh! */
				ms->line = me[i].mp->lineno;
				file_magwarn(ms,
				    "level 0 \"default\" did not sort last");
			}
			return;
		}
	}
}

private int
coalesce_entries(struct magic_set *ms, struct magic_entry *me, uint32_t nme,
    struct magic **ma, uint32_t *nma)
{
	uint32_t i, mentrycount = 0;
	size_t slen;

	for (i = 0; i < nme; i++)
		mentrycount += me[i].cont_count;

	slen = sizeof(**ma) * mentrycount;
	if ((*ma = CAST(struct magic *, emalloc(slen))) == NULL) {
		file_oomem(ms, slen);
		return -1;
	}

	mentrycount = 0;
	for (i = 0; i < nme; i++) {
		(void)memcpy(*ma + mentrycount, me[i].mp,
		    me[i].cont_count * sizeof(**ma));
		mentrycount += me[i].cont_count;
	}
	*nma = mentrycount;
	return 0;
}

private void
magic_entry_free(struct magic_entry *me, uint32_t nme)
{
	uint32_t i;
	if (me == NULL)
		return;
	for (i = 0; i < nme; i++)
		efree(me[i].mp);
	efree(me);
}

private struct magic_map *
apprentice_load(struct magic_set *ms, const char *fn, int action)
{
	int errs = 0;
	uint32_t i, j;
	size_t files = 0, maxfiles = 0;
	char **filearr = NULL;
	zend_stat_t st = {0};
	struct magic_map *map;
	struct magic_entry_set mset[MAGIC_SETS];
	php_stream *dir;
	php_stream_dirent d;


	memset(mset, 0, sizeof(mset));
	ms->flags |= MAGIC_CHECK;	/* Enable checks for parsed files */


	if ((map = CAST(struct magic_map *, ecalloc(1, sizeof(*map)))) == NULL)
	{
		file_oomem(ms, sizeof(*map));
		return NULL;
	}
	map->type = MAP_TYPE_MALLOC;

	/* print silly verbose header for USG compat. */
	if (action == FILE_CHECK)
		(void)fprintf(stderr, "%s\n", usg_hdr);

	/* load directory or file */
	/* FIXME: Read file names and sort them to prevent
	   non-determinism. See Debian bug #488562. */
	if (php_sys_stat(fn, &st) == 0 && S_ISDIR(st.st_mode)) {
		int mflen;
		char mfn[MAXPATHLEN];

		dir = php_stream_opendir((char *)fn, REPORT_ERRORS, NULL);
		if (!dir) {
			errs++;
			goto out;
		}
		while (php_stream_readdir(dir, &d)) {
			if ((mflen = snprintf(mfn, sizeof(mfn), "%s/%s", fn, d.d_name)) < 0) {
				file_oomem(ms,
				strlen(fn) + strlen(d.d_name) + 2);
				errs++;
				php_stream_closedir(dir);
				goto out;
			}
			if (zend_stat(mfn, &st) == -1 || !S_ISREG(st.st_mode)) {
				continue;
			}
			if (files >= maxfiles) {
				size_t mlen;
				maxfiles = (maxfiles + 1) * 2;
				mlen = maxfiles * sizeof(*filearr);
				if ((filearr = CAST(char **,
				    erealloc(filearr, mlen))) == NULL) {
					file_oomem(ms, mlen);
					php_stream_closedir(dir);
					errs++;
					goto out;
				}
			}
			filearr[files++] = estrndup(mfn, (mflen > sizeof(mfn) - 1)? sizeof(mfn) - 1: mflen);
		}
		php_stream_closedir(dir);
		if (filearr) {
			qsort(filearr, files, sizeof(*filearr), cmpstrp);
			for (i = 0; i < files; i++) {
				load_1(ms, action, filearr[i], &errs, mset);
				efree(filearr[i]);
			}
			efree(filearr);
		}
	} else
		load_1(ms, action, fn, &errs, mset);
	if (errs)
		goto out;

	for (j = 0; j < MAGIC_SETS; j++) {
		/* Set types of tests */
		for (i = 0; i < mset[j].count; ) {
			if (mset[j].me[i].mp->cont_level != 0) {
				i++;
				continue;
			}
			i = set_text_binary(ms, mset[j].me, mset[j].count, i);
		}
		if (mset[j].me)
			qsort(mset[j].me, mset[j].count, sizeof(*mset[j].me),
			    apprentice_sort);

		/*
		 * Make sure that any level 0 "default" line is last
		 * (if one exists).
		 */
		set_last_default(ms, mset[j].me, mset[j].count);

		/* coalesce per file arrays into a single one, if needed */
		if (mset[j].count == 0)
			continue;

		if (coalesce_entries(ms, mset[j].me, mset[j].count,
		    &map->magic[j], &map->nmagic[j]) == -1) {
			errs++;
			goto out;
		}
	}

out:
	for (j = 0; j < MAGIC_SETS; j++)
		magic_entry_free(mset[j].me, mset[j].count);

	if (errs) {
		apprentice_unmap(map);
		return NULL;
	}
	return map;
}

/*
 * extend the sign bit if the comparison is to be signed
 */
protected uint64_t
file_signextend(struct magic_set *ms, struct magic *m, uint64_t v)
{
	if (!(m->flag & UNSIGNED)) {
		switch(m->type) {
		/*
		 * Do not remove the casts below.  They are
		 * vital.  When later compared with the data,
		 * the sign extension must have happened.
		 */
		case FILE_BYTE:
			v = CAST(signed char,  v);
			break;
		case FILE_SHORT:
		case FILE_BESHORT:
		case FILE_LESHORT:
			v = CAST(short, v);
			break;
		case FILE_DATE:
		case FILE_BEDATE:
		case FILE_LEDATE:
		case FILE_MEDATE:
		case FILE_LDATE:
		case FILE_BELDATE:
		case FILE_LELDATE:
		case FILE_MELDATE:
		case FILE_LONG:
		case FILE_BELONG:
		case FILE_LELONG:
		case FILE_MELONG:
		case FILE_FLOAT:
		case FILE_BEFLOAT:
		case FILE_LEFLOAT:
			v = CAST(int32_t, v);
			break;
		case FILE_QUAD:
		case FILE_BEQUAD:
		case FILE_LEQUAD:
		case FILE_QDATE:
		case FILE_QLDATE:
		case FILE_QWDATE:
		case FILE_BEQDATE:
		case FILE_BEQLDATE:
		case FILE_BEQWDATE:
		case FILE_LEQDATE:
		case FILE_LEQLDATE:
		case FILE_LEQWDATE:
		case FILE_DOUBLE:
		case FILE_BEDOUBLE:
		case FILE_LEDOUBLE:
		case FILE_OFFSET:
			v = CAST(int64_t, v);
			break;
		case FILE_STRING:
		case FILE_PSTRING:
		case FILE_BESTRING16:
		case FILE_LESTRING16:
		case FILE_REGEX:
		case FILE_SEARCH:
		case FILE_DEFAULT:
		case FILE_INDIRECT:
		case FILE_NAME:
		case FILE_USE:
		case FILE_CLEAR:
		case FILE_DER:
		case FILE_GUID:
			break;
		default:
			if (ms->flags & MAGIC_CHECK)
			    file_magwarn(ms, "cannot happen: m->type=%d\n",
				    m->type);
			return FILE_BADSIZE;
		}
	}
	return v;
}

private int
string_modifier_check(struct magic_set *ms, struct magic *m)
{
	if ((ms->flags & MAGIC_CHECK) == 0)
		return 0;

	if ((m->type != FILE_REGEX || (m->str_flags & REGEX_LINE_COUNT) == 0) &&
	    (m->type != FILE_PSTRING && (m->str_flags & PSTRING_LEN) != 0)) {
		file_magwarn(ms,
		    "'/BHhLl' modifiers are only allowed for pascal strings\n");
		return -1;
	}
	switch (m->type) {
	case FILE_BESTRING16:
	case FILE_LESTRING16:
		if (m->str_flags != 0) {
			file_magwarn(ms,
			    "no modifiers allowed for 16-bit strings\n");
			return -1;
		}
		break;
	case FILE_STRING:
	case FILE_PSTRING:
		if ((m->str_flags & REGEX_OFFSET_START) != 0) {
			file_magwarn(ms,
			    "'/%c' only allowed on regex and search\n",
			    CHAR_REGEX_OFFSET_START);
			return -1;
		}
		break;
	case FILE_SEARCH:
		if (m->str_range == 0) {
			file_magwarn(ms,
			    "missing range; defaulting to %d\n",
                            STRING_DEFAULT_RANGE);
			m->str_range = STRING_DEFAULT_RANGE;
			return -1;
		}
		break;
	case FILE_REGEX:
		if ((m->str_flags & STRING_COMPACT_WHITESPACE) != 0) {
			file_magwarn(ms, "'/%c' not allowed on regex\n",
			    CHAR_COMPACT_WHITESPACE);
			return -1;
		}
		if ((m->str_flags & STRING_COMPACT_OPTIONAL_WHITESPACE) != 0) {
			file_magwarn(ms, "'/%c' not allowed on regex\n",
			    CHAR_COMPACT_OPTIONAL_WHITESPACE);
			return -1;
		}
		break;
	default:
		file_magwarn(ms, "coding error: m->type=%d\n",
		    m->type);
		return -1;
	}
	return 0;
}

private int
get_op(char c)
{
	switch (c) {
	case '&':
		return FILE_OPAND;
	case '|':
		return FILE_OPOR;
	case '^':
		return FILE_OPXOR;
	case '+':
		return FILE_OPADD;
	case '-':
		return FILE_OPMINUS;
	case '*':
		return FILE_OPMULTIPLY;
	case '/':
		return FILE_OPDIVIDE;
	case '%':
		return FILE_OPMODULO;
	default:
		return -1;
	}
}

#ifdef ENABLE_CONDITIONALS
private int
get_cond(const char *l, const char **t)
{
	static const struct cond_tbl_s {
		char name[8];
		size_t len;
		int cond;
	} cond_tbl[] = {
		{ "if",		2,	COND_IF },
		{ "elif",	4,	COND_ELIF },
		{ "else",	4,	COND_ELSE },
		{ "",		0,	COND_NONE },
	};
	const struct cond_tbl_s *p;

	for (p = cond_tbl; p->len; p++) {
		if (strncmp(l, p->name, p->len) == 0 &&
		    isspace(CAST(unsigned char, l[p->len]))) {
			if (t)
				*t = l + p->len;
			break;
		}
	}
	return p->cond;
}

private int
check_cond(struct magic_set *ms, int cond, uint32_t cont_level)
{
	int last_cond;
	last_cond = ms->c.li[cont_level].last_cond;

	switch (cond) {
	case COND_IF:
		if (last_cond != COND_NONE && last_cond != COND_ELIF) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "syntax error: `if'");
			return -1;
		}
		last_cond = COND_IF;
		break;

	case COND_ELIF:
		if (last_cond != COND_IF && last_cond != COND_ELIF) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "syntax error: `elif'");
			return -1;
		}
		last_cond = COND_ELIF;
		break;

	case COND_ELSE:
		if (last_cond != COND_IF && last_cond != COND_ELIF) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "syntax error: `else'");
			return -1;
		}
		last_cond = COND_NONE;
		break;

	case COND_NONE:
		last_cond = COND_NONE;
		break;
	}

	ms->c.li[cont_level].last_cond = last_cond;
	return 0;
}
#endif /* ENABLE_CONDITIONALS */

private int
parse_indirect_modifier(struct magic_set *ms, struct magic *m, const char **lp)
{
	const char *l = *lp;

	while (!isspace(CAST(unsigned char, *++l)))
		switch (*l) {
		case CHAR_INDIRECT_RELATIVE:
			m->str_flags |= INDIRECT_RELATIVE;
			break;
		default:
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "indirect modifier `%c' "
					"invalid", *l);
			*lp = l;
			return -1;
		}
	*lp = l;
	return 0;
}

private void
parse_op_modifier(struct magic_set *ms, struct magic *m, const char **lp,
    int op)
{
	const char *l = *lp;
	char *t;
	uint64_t val;

	++l;
	m->mask_op |= op;
	val = CAST(uint64_t, strtoull(l, &t, 0));
	l = t;
	m->num_mask = file_signextend(ms, m, val);
	eatsize(&l);
	*lp = l;
}

private int
parse_string_modifier(struct magic_set *ms, struct magic *m, const char **lp)
{
	const char *l = *lp;
	char *t;
	int have_range = 0;

	while (!isspace(CAST(unsigned char, *++l))) {
		switch (*l) {
		case '0':  case '1':  case '2':
		case '3':  case '4':  case '5':
		case '6':  case '7':  case '8':
		case '9':
			if (have_range && (ms->flags & MAGIC_CHECK))
				file_magwarn(ms, "multiple ranges");
			have_range = 1;
			m->str_range = CAST(uint32_t, strtoul(l, &t, 0));
			if (m->str_range == 0)
				file_magwarn(ms, "zero range");
			l = t - 1;
			break;
		case CHAR_COMPACT_WHITESPACE:
			m->str_flags |= STRING_COMPACT_WHITESPACE;
			break;
		case CHAR_COMPACT_OPTIONAL_WHITESPACE:
			m->str_flags |= STRING_COMPACT_OPTIONAL_WHITESPACE;
			break;
		case CHAR_IGNORE_LOWERCASE:
			m->str_flags |= STRING_IGNORE_LOWERCASE;
			break;
		case CHAR_IGNORE_UPPERCASE:
			m->str_flags |= STRING_IGNORE_UPPERCASE;
			break;
		case CHAR_REGEX_OFFSET_START:
			m->str_flags |= REGEX_OFFSET_START;
			break;
		case CHAR_BINTEST:
			m->str_flags |= STRING_BINTEST;
			break;
		case CHAR_TEXTTEST:
			m->str_flags |= STRING_TEXTTEST;
			break;
		case CHAR_TRIM:
			m->str_flags |= STRING_TRIM;
			break;
		case CHAR_PSTRING_1_LE:
#define SET_LENGTH(a) m->str_flags = (m->str_flags & ~PSTRING_LEN) | (a)
			if (m->type != FILE_PSTRING)
				goto bad;
			SET_LENGTH(PSTRING_1_LE);
			break;
		case CHAR_PSTRING_2_BE:
			if (m->type != FILE_PSTRING)
				goto bad;
			SET_LENGTH(PSTRING_2_BE);
			break;
		case CHAR_PSTRING_2_LE:
			if (m->type != FILE_PSTRING)
				goto bad;
			SET_LENGTH(PSTRING_2_LE);
			break;
		case CHAR_PSTRING_4_BE:
			if (m->type != FILE_PSTRING)
				goto bad;
			SET_LENGTH(PSTRING_4_BE);
			break;
		case CHAR_PSTRING_4_LE:
			switch (m->type) {
			case FILE_PSTRING:
			case FILE_REGEX:
				break;
			default:
				goto bad;
			}
			SET_LENGTH(PSTRING_4_LE);
			break;
		case CHAR_PSTRING_LENGTH_INCLUDES_ITSELF:
			if (m->type != FILE_PSTRING)
				goto bad;
			m->str_flags |= PSTRING_LENGTH_INCLUDES_ITSELF;
			break;
		default:
		bad:
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "string modifier `%c' "
					"invalid", *l);
			goto out;
		}
		/* allow multiple '/' for readability */
		if (l[1] == '/' && !isspace(CAST(unsigned char, l[2])))
			l++;
	}
	if (string_modifier_check(ms, m) == -1)
		goto out;
	*lp = l;
	return 0;
out:
	*lp = l;
	return -1;
}

/*
 * parse one line from magic file, put into magic[index++] if valid
 */
private int
parse(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t lineno, int action)
{
#ifdef ENABLE_CONDITIONALS
	static uint32_t last_cont_level = 0;
#endif
	size_t i;
	struct magic *m;
	const char *l = line;
	char *t;
	int op;
	uint32_t cont_level;
	int32_t diff;

	cont_level = 0;

	/*
	 * Parse the offset.
	 */
	while (*l == '>') {
		++l;		/* step over */
		cont_level++;
	}
#ifdef ENABLE_CONDITIONALS
	if (cont_level == 0 || cont_level > last_cont_level)
		if (file_check_mem(ms, cont_level) == -1)
			return -1;
	last_cont_level = cont_level;
#endif
	if (cont_level != 0) {
		if (me->mp == NULL) {
			file_magerror(ms, "No current entry for continuation");
			return -1;
		}
		if (me->cont_count == 0) {
			file_magerror(ms, "Continuations present with 0 count");
			return -1;
		}
		m = &me->mp[me->cont_count - 1];
		diff = CAST(int32_t, cont_level) - CAST(int32_t, m->cont_level);
		if (diff > 1)
			file_magwarn(ms, "New continuation level %u is more "
			    "than one larger than current level %u", cont_level,
			    m->cont_level);
		if (me->cont_count == me->max_count) {
			struct magic *nm;
			size_t cnt = me->max_count + ALLOC_CHUNK;
			if ((nm = CAST(struct magic *, erealloc(me->mp,
			    sizeof(*nm) * cnt))) == NULL) {
				file_oomem(ms, sizeof(*nm) * cnt);
				return -1;
			}
			me->mp = nm;
			me->max_count = CAST(uint32_t, cnt);
		}
		m = &me->mp[me->cont_count++];
		(void)memset(m, 0, sizeof(*m));
		m->cont_level = cont_level;
	} else {
		static const size_t len = sizeof(*m) * ALLOC_CHUNK;
		if (me->mp != NULL)
			return 1;
		if ((m = CAST(struct magic *, emalloc(len))) == NULL) {
			file_oomem(ms, len);
			return -1;
		}
		me->mp = m;
		me->max_count = ALLOC_CHUNK;
		(void)memset(m, 0, sizeof(*m));
		m->factor_op = FILE_FACTOR_OP_NONE;
		m->cont_level = 0;
		me->cont_count = 1;
	}
	m->lineno = CAST(uint32_t, lineno);

	if (*l == '&') {  /* m->cont_level == 0 checked below. */
                ++l;            /* step over */
                m->flag |= OFFADD;
        }
	if (*l == '(') {
		++l;		/* step over */
		m->flag |= INDIR;
		if (m->flag & OFFADD)
			m->flag = (m->flag & ~OFFADD) | INDIROFFADD;

		if (*l == '&') {  /* m->cont_level == 0 checked below */
			++l;            /* step over */
			m->flag |= OFFADD;
		}
	}
	/* Indirect offsets are not valid at level 0. */
	if (m->cont_level == 0 && (m->flag & (OFFADD | INDIROFFADD))) {
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "relative offset at level 0");
		return -1;
	}

	/* get offset, then skip over it */
	if (*l == '-') {
		++l;            /* step over */
		m->flag |= OFFNEGATIVE;
	}
	m->offset = CAST(int32_t, strtol(l, &t, 0));
        if (l == t) {
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "offset `%s' invalid", l);
		return -1;
	}

        l = t;

	if (m->flag & INDIR) {
		m->in_type = FILE_LONG;
		m->in_offset = 0;
		m->in_op = 0;
		/*
		 * read [.,lbs][+-]nnnnn)
		 */
		if (*l == '.' || *l == ',') {
			if (*l == ',')
				m->in_op |= FILE_OPSIGNED;
			l++;
			switch (*l) {
			case 'l':
				m->in_type = FILE_LELONG;
				break;
			case 'L':
				m->in_type = FILE_BELONG;
				break;
			case 'm':
				m->in_type = FILE_MELONG;
				break;
			case 'h':
			case 's':
				m->in_type = FILE_LESHORT;
				break;
			case 'H':
			case 'S':
				m->in_type = FILE_BESHORT;
				break;
			case 'c':
			case 'b':
			case 'C':
			case 'B':
				m->in_type = FILE_BYTE;
				break;
			case 'e':
			case 'f':
			case 'g':
				m->in_type = FILE_LEDOUBLE;
				break;
			case 'E':
			case 'F':
			case 'G':
				m->in_type = FILE_BEDOUBLE;
				break;
			case 'i':
				m->in_type = FILE_LEID3;
				break;
			case 'I':
				m->in_type = FILE_BEID3;
				break;
			case 'q':
				m->in_type = FILE_LEQUAD;
				break;
			case 'Q':
				m->in_type = FILE_BEQUAD;
				break;
			default:
				if (ms->flags & MAGIC_CHECK)
					file_magwarn(ms,
					    "indirect offset type `%c' invalid",
					    *l);
				return -1;
			}
			l++;
		}

		if (*l == '~') {
			m->in_op |= FILE_OPINVERSE;
			l++;
		}
		if ((op = get_op(*l)) != -1) {
			m->in_op |= op;
			l++;
		}
		if (*l == '(') {
			m->in_op |= FILE_OPINDIRECT;
			l++;
		}
		if (isdigit(CAST(unsigned char, *l)) || *l == '-') {
			m->in_offset = CAST(int32_t, strtol(l, &t, 0));
			if (l == t) {
				if (ms->flags & MAGIC_CHECK)
					file_magwarn(ms,
					    "in_offset `%s' invalid", l);
				return -1;
			}
			l = t;
		}
		if (*l++ != ')' ||
		    ((m->in_op & FILE_OPINDIRECT) && *l++ != ')')) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms,
				    "missing ')' in indirect offset");
			return -1;
		}
	}
	EATAB;

#ifdef ENABLE_CONDITIONALS
	m->cond = get_cond(l, &l);
	if (check_cond(ms, m->cond, cont_level) == -1)
		return -1;

	EATAB;
#endif

	/*
	 * Parse the type.
	 */
	if (*l == 'u') {
		/*
		 * Try it as a keyword type prefixed by "u"; match what
		 * follows the "u".  If that fails, try it as an SUS
		 * integer type.
		 */
		m->type = get_type(type_tbl, l + 1, &l);
		if (m->type == FILE_INVALID) {
			/*
			 * Not a keyword type; parse it as an SUS type,
			 * 'u' possibly followed by a number or C/S/L.
			 */
			m->type = get_standard_integer_type(l, &l);
		}
		/* It's unsigned. */
		if (m->type != FILE_INVALID)
			m->flag |= UNSIGNED;
	} else {
		/*
		 * Try it as a keyword type.  If that fails, try it as
		 * an SUS integer type if it begins with "d" or as an
		 * SUS string type if it begins with "s".  In any case,
		 * it's not unsigned.
		 */
		m->type = get_type(type_tbl, l, &l);
		if (m->type == FILE_INVALID) {
			/*
			 * Not a keyword type; parse it as an SUS type,
			 * either 'd' possibly followed by a number or
			 * C/S/L, or just 's'.
			 */
			if (*l == 'd')
				m->type = get_standard_integer_type(l, &l);
			else if (*l == 's'
			    && !isalpha(CAST(unsigned char, l[1]))) {
				m->type = FILE_STRING;
				++l;
			}
		}
	}

	if (m->type == FILE_INVALID) {
		/* Not found - try it as a special keyword. */
		m->type = get_type(special_tbl, l, &l);
	}

	if (m->type == FILE_INVALID) {
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "type `%s' invalid", l);
		return -1;
	}

	if (m->type == FILE_NAME && cont_level != 0) {
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "`name%s' entries can only be "
			    "declared at top level", l);
		return -1;
	}

	/* New-style anding: "0 byte&0x80 =0x80 dynamically linked" */
	/* New and improved: ~ & | ^ + - * / % -- exciting, isn't it? */

	m->mask_op = 0;
	if (*l == '~') {
		if (!IS_LIBMAGIC_STRING(m->type))
			m->mask_op |= FILE_OPINVERSE;
		else if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "'~' invalid for string types");
		++l;
	}
	m->str_range = 0;
	m->str_flags = m->type == FILE_PSTRING ? PSTRING_1_LE : 0;
	if ((op = get_op(*l)) != -1) {
		if (IS_LIBMAGIC_STRING(m->type)) {
			int r;

			if (op != FILE_OPDIVIDE) {
				if (ms->flags & MAGIC_CHECK)
					file_magwarn(ms,
					    "invalid string/indirect op: "
					    "`%c'", *t);
				return -1;
			}

			if (m->type == FILE_INDIRECT)
				r = parse_indirect_modifier(ms, m, &l);
			else
				r = parse_string_modifier(ms, m, &l);
			if (r == -1)
				return -1;
		} else
			parse_op_modifier(ms, m, &l, op);
	}

	/*
	 * We used to set mask to all 1's here, instead let's just not do
	 * anything if mask = 0 (unless you have a better idea)
	 */
	EATAB;

	switch (*l) {
	case '>':
	case '<':
  		m->reln = *l;
  		++l;
		if (*l == '=') {
			if (ms->flags & MAGIC_CHECK) {
				file_magwarn(ms, "%c= not supported",
				    m->reln);
				return -1;
			}
		   ++l;
		}
		break;
	/* Old-style anding: "0 byte &0x80 dynamically linked" */
	case '&':
	case '^':
	case '=':
  		m->reln = *l;
  		++l;
		if (*l == '=') {
		   /* HP compat: ignore &= etc. */
		   ++l;
		}
		break;
	case '!':
		m->reln = *l;
		++l;
		break;
	default:
  		m->reln = '=';	/* the default relation */
		if (*l == 'x' && ((isascii(CAST(unsigned char, l[1])) &&
		    isspace(CAST(unsigned char, l[1]))) || !l[1])) {
			m->reln = *l;
			++l;
		}
		break;
	}
	/*
	 * Grab the value part, except for an 'x' reln.
	 */
	if (m->reln != 'x' && getvalue(ms, m, &l, action))
		return -1;

	/*
	 * TODO finish this macro and start using it!
	 * #define offsetcheck {if (offset > ms->bytes_max -1)
	 *	magwarn("offset too big"); }
	 */

	/*
	 * Now get last part - the description
	 */
	EATAB;
	if (l[0] == '\b') {
		++l;
		m->flag |= NOSPACE;
	} else if ((l[0] == '\\') && (l[1] == 'b')) {
		++l;
		++l;
		m->flag |= NOSPACE;
	}
	for (i = 0; (m->desc[i++] = *l++) != '\0' && i < sizeof(m->desc); )
		continue;
	if (i == sizeof(m->desc)) {
		m->desc[sizeof(m->desc) - 1] = '\0';
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "description `%s' truncated", m->desc);
	}

        /*
	 * We only do this check while compiling, or if any of the magic
	 * files were not compiled.
         */
        if (ms->flags & MAGIC_CHECK) {
		if (check_format(ms, m) == -1)
			return -1;
	}
#ifndef COMPILE_ONLY
	if (action == FILE_CHECK) {
		file_mdump(m);
	}
#endif
	m->mimetype[0] = '\0';		/* initialise MIME type to none */
	return 0;
}

/*
 * parse a STRENGTH annotation line from magic file, put into magic[index - 1]
 * if valid
 */
private int
parse_strength(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t len)
{
	const char *l = line;
	char *el;
	unsigned long factor;
	struct magic *m = &me->mp[0];

	if (m->factor_op != FILE_FACTOR_OP_NONE) {
		file_magwarn(ms,
		    "Current entry already has a strength type: %c %d",
		    m->factor_op, m->factor);
		return -1;
	}
	if (m->type == FILE_NAME) {
		file_magwarn(ms, "%s: Strength setting is not supported in "
		    "\"name\" magic entries", m->value.s);
		return -1;
	}
	EATAB;
	switch (*l) {
	case FILE_FACTOR_OP_NONE:
	case FILE_FACTOR_OP_PLUS:
	case FILE_FACTOR_OP_MINUS:
	case FILE_FACTOR_OP_TIMES:
	case FILE_FACTOR_OP_DIV:
		m->factor_op = *l++;
		break;
	default:
		file_magwarn(ms, "Unknown factor op `%c'", *l);
		return -1;
	}
	EATAB;
	factor = strtoul(l, &el, 0);
	if (factor > 255) {
		file_magwarn(ms, "Too large factor `%lu'", factor);
		goto out;
	}
	if (*el && !isspace(CAST(unsigned char, *el))) {
		file_magwarn(ms, "Bad factor `%s'", l);
		goto out;
	}
	m->factor = CAST(uint8_t, factor);
	if (m->factor == 0 && m->factor_op == FILE_FACTOR_OP_DIV) {
		file_magwarn(ms, "Cannot have factor op `%c' and factor %u",
		    m->factor_op, m->factor);
		goto out;
	}
	return 0;
out:
	m->factor_op = FILE_FACTOR_OP_NONE;
	m->factor = 0;
	return -1;
}

private int
goodchar(unsigned char x, const char *extra)
{
	return (isascii(x) && isalnum(x)) || strchr(extra, x);
}

private int
parse_extra(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t llen, zend_off_t off, size_t len, const char *name, const char *extra, int nt)
{
	size_t i;
	const char *l = line;
	struct magic *m = &me->mp[me->cont_count == 0 ? 0 : me->cont_count - 1];
	char *buf = CAST(char *, CAST(void *, m)) + off;

	if (buf[0] != '\0') {
		len = nt ? strlen(buf) : len;
		file_magwarn(ms, "Current entry already has a %s type "
		    "`%.*s', new type `%s'", name, CAST(int, len), buf, l);
		return -1;
	}

	if (*m->desc == '\0') {
		file_magwarn(ms, "Current entry does not yet have a "
		    "description for adding a %s type", name);
		return -1;
	}

	EATAB;
	for (i = 0; *l && i < llen && i < len && goodchar(*l, extra);
	    buf[i++] = *l++)
		continue;

	if (i == len && *l) {
		if (nt)
			buf[len - 1] = '\0';
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "%s type `%s' truncated %"
			    SIZE_T_FORMAT "u", name, line, i);
	} else {
		if (!isspace(CAST(unsigned char, *l)) && !goodchar(*l, extra))
			file_magwarn(ms, "%s type `%s' has bad char '%c'",
			    name, line, *l);
		if (nt)
			buf[i] = '\0';
	}

	if (i > 0)
		return 0;

	file_magerror(ms, "Bad magic entry '%s'", line);
	return -1;
}

/*
 * Parse an Apple CREATOR/TYPE annotation from magic file and put it into
 * magic[index - 1]
 */
private int
parse_apple(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t len)
{
	struct magic *m = &me->mp[0];

	return parse_extra(ms, me, line, len,
	    CAST(off_t, offsetof(struct magic, apple)),
	    sizeof(m->apple), "APPLE", "!+-./?", 0);
}

/*
 * Parse a comma-separated list of extensions
 */
private int
parse_ext(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t len)
{
	struct magic *m = &me->mp[0];

	return parse_extra(ms, me, line, len,
	    CAST(off_t, offsetof(struct magic, ext)),
	    sizeof(m->ext), "EXTENSION", ",!+-/@?_$", 0);
}

/*
 * parse a MIME annotation line from magic file, put into magic[index - 1]
 * if valid
 */
private int
parse_mime(struct magic_set *ms, struct magic_entry *me, const char *line,
    size_t len)
{
	struct magic *m = &me->mp[0];

	return parse_extra(ms, me, line, len,
	    CAST(off_t, offsetof(struct magic, mimetype)),
	    sizeof(m->mimetype), "MIME", "+-/.$?:{}", 1);
}

private int
check_format_type(const char *ptr, int type, const char **estr)
{
	int quad = 0, h;
	size_t len, cnt;
	if (*ptr == '\0') {
		/* Missing format string; bad */
		*estr = "missing format spec";
		return -1;
	}

	switch (file_formats[type]) {
	case FILE_FMT_QUAD:
		quad = 1;
		/*FALLTHROUGH*/
	case FILE_FMT_NUM:
		if (quad == 0) {
			switch (type) {
			case FILE_BYTE:
				h = 2;
				break;
			case FILE_SHORT:
			case FILE_BESHORT:
			case FILE_LESHORT:
				h = 1;
				break;
			case FILE_LONG:
			case FILE_BELONG:
			case FILE_LELONG:
			case FILE_MELONG:
			case FILE_LEID3:
			case FILE_BEID3:
			case FILE_INDIRECT:
				h = 0;
				break;
			default:
				abort();
			}
		} else
			h = 0;
		if (*ptr == '-')
			ptr++;
		if (*ptr == '.')
			ptr++;
		if (*ptr == '#')
			ptr++;
#define CHECKLEN() do { \
	for (len = cnt = 0; isdigit(CAST(unsigned char, *ptr)); ptr++, cnt++) \
		len = len * 10 + (*ptr - '0'); \
	if (cnt > 5 || len > 1024) \
		goto toolong; \
} while (/*CONSTCOND*/0)

		CHECKLEN();
		if (*ptr == '.')
			ptr++;
		CHECKLEN();
		if (quad) {
			if (*ptr++ != 'l')
				goto invalid;
			if (*ptr++ != 'l')
				goto invalid;
		}

		switch (*ptr++) {
#ifdef STRICT_FORMAT 	/* "long" formats are int formats for us */
		/* so don't accept the 'l' modifier */
		case 'l':
			switch (*ptr++) {
			case 'i':
			case 'd':
			case 'u':
			case 'o':
			case 'x':
			case 'X':
				if (h == 0)
					return 0;
				/*FALLTHROUGH*/
			default:
				goto invalid;
			}

		/*
		 * Don't accept h and hh modifiers. They make writing
		 * magic entries more complicated, for very little benefit
		 */
		case 'h':
			if (h-- <= 0)
				goto invalid;
			switch (*ptr++) {
			case 'h':
				if (h-- <= 0)
					goto invalid;
				switch (*ptr++) {
				case 'i':
				case 'd':
				case 'u':
				case 'o':
				case 'x':
				case 'X':
					return 0;
				default:
					goto invalid;
				}
			case 'i':
			case 'd':
			case 'u':
			case 'o':
			case 'x':
			case 'X':
				if (h == 0)
					return 0;
				/*FALLTHROUGH*/
			default:
				goto invalid;
			}
#endif
		case 'c':
			if (h == 2)
				return 0;
			goto invalid;
		case 'i':
		case 'd':
		case 'u':
		case 'o':
		case 'x':
		case 'X':
#ifdef STRICT_FORMAT
			if (h == 0)
				return 0;
			/*FALLTHROUGH*/
#else
			return 0;
#endif
		default:
			goto invalid;
		}

	case FILE_FMT_FLOAT:
	case FILE_FMT_DOUBLE:
		if (*ptr == '-')
			ptr++;
		if (*ptr == '.')
			ptr++;
		CHECKLEN();
		if (*ptr == '.')
			ptr++;
		CHECKLEN();
		switch (*ptr++) {
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
			return 0;

		default:
			goto invalid;
		}


	case FILE_FMT_STR:
		if (*ptr == '-')
			ptr++;
		while (isdigit(CAST(unsigned char, *ptr)))
			ptr++;
		if (*ptr == '.') {
			ptr++;
			while (isdigit(CAST(unsigned char , *ptr)))
				ptr++;
		}

		switch (*ptr++) {
		case 's':
			return 0;
		default:
			goto invalid;
		}

	default:
		/* internal error */
		abort();
	}
invalid:
	*estr = "not valid";
toolong:
	*estr = "too long";
	return -1;
}

/*
 * Check that the optional printf format in description matches
 * the type of the magic.
 */
private int
check_format(struct magic_set *ms, struct magic *m)
{
	char *ptr;
	const char *estr;

	for (ptr = m->desc; *ptr; ptr++)
		if (*ptr == '%')
			break;
	if (*ptr == '\0') {
		/* No format string; ok */
		return 1;
	}

	assert(file_nformats == file_nnames);

	if (m->type >= file_nformats) {
		file_magwarn(ms, "Internal error inconsistency between "
		    "m->type and format strings");
		return -1;
	}
	if (file_formats[m->type] == FILE_FMT_NONE) {
		file_magwarn(ms, "No format string for `%s' with description "
		    "`%s'", m->desc, file_names[m->type]);
		return -1;
	}

	ptr++;
	if (check_format_type(ptr, m->type, &estr) == -1) {
		/*
		 * TODO: this error message is unhelpful if the format
		 * string is not one character long
		 */
		file_magwarn(ms, "Printf format is %s for type "
		    "`%s' in description `%s'", estr,
		    file_names[m->type], m->desc);
		return -1;
	}

	for (; *ptr; ptr++) {
		if (*ptr == '%') {
			file_magwarn(ms,
			    "Too many format strings (should have at most one) "
			    "for `%s' with description `%s'",
			    file_names[m->type], m->desc);
			return -1;
		}
	}
	return 0;
}

/*
 * Read a numeric value from a pointer, into the value union of a magic
 * pointer, according to the magic type.  Update the string pointer to point
 * just after the number read.  Return 0 for success, non-zero for failure.
 */
private int
getvalue(struct magic_set *ms, struct magic *m, const char **p, int action)
{
	char *ep;
	uint64_t ull;

	switch (m->type) {
	case FILE_BESTRING16:
	case FILE_LESTRING16:
	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_REGEX:
	case FILE_SEARCH:
	case FILE_NAME:
	case FILE_USE:
	case FILE_DER:
		*p = getstr(ms, m, *p, action == FILE_COMPILE);
		if (*p == NULL) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "cannot get string from `%s'",
				    m->value.s);
			return -1;
		}
		if (m->type == FILE_REGEX) {
			zend_string *pattern;
			int options = 0;
			pcre_cache_entry *pce;

			pattern = convert_libmagic_pattern(m->value.s, strlen(m->value.s), options);

			if ((pce = pcre_get_compiled_regex_cache(pattern)) == NULL) {
				zend_string_release(pattern);
				return -1;
			}
			zend_string_release(pattern);

			return 0;
		}
		return 0;
	default:
		if (m->reln == 'x')
			return 0;
		break;
	}

	switch (m->type) {
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		errno = 0;
#ifdef HAVE_STRTOF
		m->value.f = strtof(*p, &ep);
#else
		m->value.f = (float)strtod(*p, &ep);
#endif
		if (errno == 0)
			*p = ep;
		return 0;
	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		errno = 0;
		m->value.d = strtod(*p, &ep);
		if (errno == 0)
			*p = ep;
		return 0;
	case FILE_GUID:
		if (file_parse_guid(*p, m->value.guid) == -1)
			return -1;
		*p += FILE_GUID_SIZE - 1;
		return 0;
	default:
		errno = 0;
		ull = CAST(uint64_t, strtoull(*p, &ep, 0));
		m->value.q = file_signextend(ms, m, ull);
		if (*p == ep) {
			file_magwarn(ms, "Unparsable number `%s'", *p);
		} else {
			size_t ts = typesize(m->type);
			uint64_t x;
			const char *q;

			if (ts == FILE_BADSIZE) {
				file_magwarn(ms,
				    "Expected numeric type got `%s'",
				    type_tbl[m->type].name);
			}
			for (q = *p; isspace(CAST(unsigned char, *q)); q++)
				continue;
			if (*q == '-')
				ull = -CAST(int64_t, ull);
			switch (ts) {
			case 1:
				x = CAST(uint64_t, ull & ~0xffULL);
				break;
			case 2:
				x = CAST(uint64_t, ull & ~0xffffULL);
				break;
			case 4:
				x = CAST(uint64_t, ull & ~0xffffffffULL);
				break;
			case 8:
				x = 0;
				break;
			default:
				abort();
			}
			if (x) {
				file_magwarn(ms, "Overflow for numeric"
				    " type `%s' value %#" PRIx64,
				    type_tbl[m->type].name, ull);
			}
		}
		if (errno == 0) {
			*p = ep;
			eatsize(p);
		}
		return 0;
	}
}

/*
 * Convert a string containing C character escapes.  Stop at an unescaped
 * space or tab.
 * Copy the converted version to "m->value.s", and the length in m->vallen.
 * Return updated scan pointer as function result. Warn if set.
 */
private const char *
getstr(struct magic_set *ms, struct magic *m, const char *s, int warn)
{
	const char *origs = s;
	char	*p = m->value.s;
	size_t  plen = sizeof(m->value.s);
	char 	*origp = p;
	char	*pmax = p + plen - 1;
	int	c;
	int	val;

	while ((c = *s++) != '\0') {
		if (isspace(CAST(unsigned char, c)))
			break;
		if (p >= pmax) {
			file_error(ms, 0, "string too long: `%s'", origs);
			return NULL;
		}
		if (c == '\\') {
			switch(c = *s++) {

			case '\0':
				if (warn)
					file_magwarn(ms, "incomplete escape");
				s--;
				goto out;

			case '\t':
				if (warn) {
					file_magwarn(ms,
					    "escaped tab found, use \\t instead");
					warn = 0;	/* already did */
				}
				/*FALLTHROUGH*/
			default:
				if (warn) {
					if (isprint(CAST(unsigned char, c))) {
						/* Allow escaping of
						 * ``relations'' */
						if (strchr("<>&^=!", c) == NULL
						    && (m->type != FILE_REGEX ||
						    strchr("[]().*?^$|{}", c)
						    == NULL)) {
							file_magwarn(ms, "no "
							    "need to escape "
							    "`%c'", c);
						}
					} else {
						file_magwarn(ms,
						    "unknown escape sequence: "
						    "\\%03o", c);
					}
				}
				/*FALLTHROUGH*/
			/* space, perhaps force people to use \040? */
			case ' ':
#if 0
			/*
			 * Other things people escape, but shouldn't need to,
			 * so we disallow them
			 */
			case '\'':
			case '"':
			case '?':
#endif
			/* Relations */
			case '>':
			case '<':
			case '&':
			case '^':
			case '=':
			case '!':
			/* and baskslash itself */
			case '\\':
				*p++ = CAST(char, c);
				break;

			case 'a':
				*p++ = '\a';
				break;

			case 'b':
				*p++ = '\b';
				break;

			case 'f':
				*p++ = '\f';
				break;

			case 'n':
				*p++ = '\n';
				break;

			case 'r':
				*p++ = '\r';
				break;

			case 't':
				*p++ = '\t';
				break;

			case 'v':
				*p++ = '\v';
				break;

			/* \ and up to 3 octal digits */
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = c - '0';
				c = *s++;  /* try for 2 */
				if (c >= '0' && c <= '7') {
					val = (val << 3) | (c - '0');
					c = *s++;  /* try for 3 */
					if (c >= '0' && c <= '7')
						val = (val << 3) | (c-'0');
					else
						--s;
				}
				else
					--s;
				*p++ = CAST(char, val);
				break;

			/* \x and up to 2 hex digits */
			case 'x':
				val = 'x';	/* Default if no digits */
				c = hextoint(*s++);	/* Get next char */
				if (c >= 0) {
					val = c;
					c = hextoint(*s++);
					if (c >= 0)
						val = (val << 4) + c;
					else
						--s;
				} else
					--s;
				*p++ = CAST(char, val);
				break;
			}
		} else
			*p++ = CAST(char, c);
	}
	--s;
out:
	*p = '\0';
	m->vallen = CAST(unsigned char, (p - origp));
	if (m->type == FILE_PSTRING) {
		size_t l =  file_pstring_length_size(ms, m);
		if (l == FILE_BADSIZE)
			return NULL;
		m->vallen += CAST(unsigned char, l);
	}
	return s;
}


/* Single hex char to int; -1 if not a hex char. */
private int
hextoint(int c)
{
	if (!isascii(CAST(unsigned char, c)))
		return -1;
	if (isdigit(CAST(unsigned char, c)))
		return c - '0';
	if ((c >= 'a') && (c <= 'f'))
		return c + 10 - 'a';
	if (( c>= 'A') && (c <= 'F'))
		return c + 10 - 'A';
	return -1;
}


/*
 * Print a string containing C character escapes.
 */
protected void
file_showstr(FILE *fp, const char *s, size_t len)
{
	char	c;

	for (;;) {
		if (len == FILE_BADSIZE) {
			c = *s++;
			if (c == '\0')
				break;
		}
		else  {
			if (len-- == 0)
				break;
			c = *s++;
		}
		if (c >= 040 && c <= 0176)	/* TODO isprint && !iscntrl */
			(void) fputc(c, fp);
		else {
			(void) fputc('\\', fp);
			switch (c) {
			case '\a':
				(void) fputc('a', fp);
				break;

			case '\b':
				(void) fputc('b', fp);
				break;

			case '\f':
				(void) fputc('f', fp);
				break;

			case '\n':
				(void) fputc('n', fp);
				break;

			case '\r':
				(void) fputc('r', fp);
				break;

			case '\t':
				(void) fputc('t', fp);
				break;

			case '\v':
				(void) fputc('v', fp);
				break;

			default:
				(void) fprintf(fp, "%.3o", c & 0377);
				break;
			}
		}
	}
}

/*
 * eatsize(): Eat the size spec from a number [eg. 10UL]
 */
private void
eatsize(const char **p)
{
	const char *l = *p;

	if (LOWCASE(*l) == 'u')
		l++;

	switch (LOWCASE(*l)) {
	case 'l':    /* long */
	case 's':    /* short */
	case 'h':    /* short */
	case 'b':    /* char/byte */
	case 'c':    /* char/byte */
		l++;
		/*FALLTHROUGH*/
	default:
		break;
	}

	*p = l;
}

/*
 * handle a compiled file.
 */

private struct magic_map *
apprentice_map(struct magic_set *ms, const char *fn)
{
	uint32_t *ptr;
	uint32_t version, entries = 0, nentries;
	int needsbyteswap;
	char *dbname = NULL;
	struct magic_map *map;
	size_t i;
	php_stream *stream = NULL;
	php_stream_statbuf st;



	if ((map = CAST(struct magic_map *, ecalloc(1, sizeof(*map)))) == NULL) {
		file_oomem(ms, sizeof(*map));
		return NULL;
	}

	if (fn == NULL) {
		map->p = (void *)&php_magic_database;
		goto internal_loaded;
	}

#ifdef PHP_WIN32
	/* Don't bother on windows with php_stream_open_wrapper,
	return to give apprentice_load() a chance. */
	if (php_stream_stat_path_ex((char *)fn, 0, &st, NULL) == SUCCESS) {
               if (st.sb.st_mode & S_IFDIR) {
                       goto error;
               }
       }
#endif

	dbname = mkdbname(ms, fn, 0);
	if (dbname == NULL)
		goto error;

	stream = php_stream_open_wrapper((char *)fn, "rb", REPORT_ERRORS, NULL);

	if (!stream) {
		goto error;
	}

#ifndef PHP_WIN32
	if (php_stream_stat(stream, &st) < 0) {
		file_error(ms, errno, "cannot stat `%s'", dbname);
		goto error;
	}
#endif
	if (st.sb.st_size < 8 || st.sb.st_size > maxoff_t()) {
		file_error(ms, 0, "file `%s' is too %s", dbname,
		    st.sb.st_size < 8 ? "small" : "large");
		goto error;
	}

	map->type = MAP_TYPE_MALLOC;
	map->len = CAST(size_t, st.sb.st_size);
	map->p = CAST(void *, emalloc(map->len));

	if (php_stream_read(stream, map->p, (size_t)st.sb.st_size) != (size_t)st.sb.st_size) {
		file_badread(ms);
		goto error;
	}

	php_stream_close(stream);
	stream = NULL;

internal_loaded:
	ptr = (uint32_t *)(void *)map->p;
	if (*ptr != MAGICNO) {
		if (swap4(*ptr) != MAGICNO) {
			file_error(ms, 0, "bad magic in `%s'", dbname);
			goto error;
		}
		needsbyteswap = 1;
	} else
		needsbyteswap = 0;
	if (needsbyteswap)
		version = swap4(ptr[1]);
	else
		version = ptr[1];
	if (version != VERSIONNO) {
		file_error(ms, 0, "File %d supports only version %d magic "
		    "files. `%s' is version %d", MAGIC_VERSION,
		    VERSIONNO, dbname, version);
		goto error;
	}

	/* php_magic_database is a const, performing writes will segfault. This is for big-endian
	machines only, PPC and Sparc specifically. Consider static variable or MINIT in
	future. */
	if (needsbyteswap && fn == NULL) {
		map->p = emalloc(sizeof(php_magic_database));
		map->p = memcpy(map->p, php_magic_database, sizeof(php_magic_database));
	}

	if (NULL != fn) {
		nentries = (uint32_t)(st.sb.st_size / sizeof(struct magic));
		entries = (uint32_t)(st.sb.st_size / sizeof(struct magic));
		if ((zend_off_t)(entries * sizeof(struct magic)) != st.sb.st_size) {
			file_error(ms, 0, "Size of `%s' %llu is not a multiple of %zu",
				dbname, (unsigned long long)st.sb.st_size,
				sizeof(struct magic));
			goto error;
		}
	}
	map->magic[0] = CAST(struct magic *, map->p) + 1;
	nentries = 0;
	for (i = 0; i < MAGIC_SETS; i++) {
		if (needsbyteswap)
			map->nmagic[i] = swap4(ptr[i + 2]);
		else
			map->nmagic[i] = ptr[i + 2];
		if (i != MAGIC_SETS - 1)
			map->magic[i + 1] = map->magic[i] + map->nmagic[i];
		nentries += map->nmagic[i];
	}
	if (NULL != fn && entries != nentries + 1) {
		file_error(ms, 0, "Inconsistent entries in `%s' %u != %u",
		    dbname, entries, nentries + 1);
		goto error;
	}
	if (needsbyteswap)
		for (i = 0; i < MAGIC_SETS; i++)
			byteswap(map->magic[i], map->nmagic[i]);

	if (dbname) {
		efree(dbname);
	}
	return map;

error:
	if (stream) {
		php_stream_close(stream);
	}
	apprentice_unmap(map);
	if (dbname) {
		efree(dbname);
	}
	return NULL;
}

/*
 * handle an mmaped file.
 */
private int
apprentice_compile(struct magic_set *ms, struct magic_map *map, const char *fn)
{
	static const size_t nm = sizeof(*map->nmagic) * MAGIC_SETS;
	static const size_t m = sizeof(**map->magic);
	size_t len;
	char *dbname;
	int rv = -1;
	uint32_t i;
	union {
		struct magic m;
		uint32_t h[2 + MAGIC_SETS];
	} hdr;
	php_stream *stream;

	dbname = mkdbname(ms, fn, 1);

	if (dbname == NULL)
		goto out;

	/* wb+ == O_WRONLY|O_CREAT|O_TRUNC|O_BINARY */
	stream = php_stream_open_wrapper((char *)fn, "wb+", REPORT_ERRORS, NULL);

	if (!stream) {
		file_error(ms, errno, "cannot open `%s'", dbname);
		goto out;
	}
	memset(&hdr, 0, sizeof(hdr));
	hdr.h[0] = MAGICNO;
	hdr.h[1] = VERSIONNO;
	memcpy(hdr.h + 2, map->nmagic, nm);

	if (php_stream_write(stream,(const char *)&hdr, sizeof(hdr)) != (ssize_t)sizeof(hdr)) {
		file_error(ms, errno, "error writing `%s'", dbname);
		goto out;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		len = m * map->nmagic[i];
		if (php_stream_write(stream, (const char *)map->magic[i], len) != (ssize_t)len) {
			file_error(ms, errno, "error writing `%s'", dbname);
			goto out;
		}
	}

	if (stream) {
		php_stream_close(stream);
	}
	rv = 0;
out:
	efree(dbname);
	return rv;
}

private const char ext[] = ".mgc";
/*
 * make a dbname
 */
private char *
mkdbname(struct magic_set *ms, const char *fn, int strip)
{
	const char *p, *q;
	char *buf;

	if (strip) {
		if ((p = strrchr(fn, '/')) != NULL)
			fn = ++p;
	}

	for (q = fn; *q; q++)
		continue;
	/* Look for .mgc */
	for (p = ext + sizeof(ext) - 1; p >= ext && q >= fn; p--, q--)
		if (*p != *q)
			break;

	/* Did not find .mgc, restore q */
	if (p >= ext)
		while (*q)
			q++;

	q++;
	/* Compatibility with old code that looked in .mime */
	if (ms->flags & MAGIC_MIME) {
		spprintf(&buf, MAXPATHLEN, "%.*s.mime%s", CAST(int, q - fn), fn, ext);
#ifdef PHP_WIN32
		if (VCWD_ACCESS(buf, R_OK) == 0) {
#else
		if (VCWD_ACCESS(buf, R_OK) != -1) {
#endif
			ms->flags &= MAGIC_MIME_TYPE;
			return buf;
		}
		efree(buf);
	}
	spprintf(&buf, MAXPATHLEN, "%.*s%s", CAST(int, q - fn), fn, ext);

	/* Compatibility with old code that looked in .mime */
	if (strstr(fn, ".mime") != NULL)
		ms->flags &= MAGIC_MIME_TYPE;
	return buf;
}

/*
 * Byteswap an mmap'ed file if needed
 */
private void
byteswap(struct magic *magic, uint32_t nmagic)
{
	uint32_t i;
	for (i = 0; i < nmagic; i++)
		bs1(&magic[i]);
}

/*
 * swap a short
 */
private uint16_t
swap2(uint16_t sv)
{
	uint16_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
	d[0] = s[1];
	d[1] = s[0];
	return rv;
}

/*
 * swap an int
 */
private uint32_t
swap4(uint32_t sv)
{
	uint32_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
	d[0] = s[3];
	d[1] = s[2];
	d[2] = s[1];
	d[3] = s[0];
	return rv;
}

/*
 * swap a quad
 */
private uint64_t
swap8(uint64_t sv)
{
	uint64_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
#if 0
	d[0] = s[3];
	d[1] = s[2];
	d[2] = s[1];
	d[3] = s[0];
	d[4] = s[7];
	d[5] = s[6];
	d[6] = s[5];
	d[7] = s[4];
#else
	d[0] = s[7];
	d[1] = s[6];
	d[2] = s[5];
	d[3] = s[4];
	d[4] = s[3];
	d[5] = s[2];
	d[6] = s[1];
	d[7] = s[0];
#endif
	return rv;
}

/*
 * byteswap a single magic entry
 */
private void
bs1(struct magic *m)
{
	m->cont_level = swap2(m->cont_level);
	m->offset = swap4(CAST(uint32_t, m->offset));
	m->in_offset = swap4(CAST(uint32_t, m->in_offset));
	m->lineno = swap4(CAST(uint32_t, m->lineno));
	if (IS_LIBMAGIC_STRING(m->type)) {
		m->str_range = swap4(m->str_range);
		m->str_flags = swap4(m->str_flags);
	}
	else {
		m->value.q = swap8(m->value.q);
		m->num_mask = swap8(m->num_mask);
	}
}

protected size_t
file_pstring_length_size(struct magic_set *ms, const struct magic *m)
{
	switch (m->str_flags & PSTRING_LEN) {
	case PSTRING_1_LE:
		return 1;
	case PSTRING_2_LE:
	case PSTRING_2_BE:
		return 2;
	case PSTRING_4_LE:
	case PSTRING_4_BE:
		return 4;
	default:
		file_error(ms, 0, "corrupt magic file "
		    "(bad pascal string length %d)",
		    m->str_flags & PSTRING_LEN);
		return FILE_BADSIZE;
	}
}
protected size_t
file_pstring_get_length(struct magic_set *ms, const struct magic *m,
    const char *ss)
{
	size_t len = 0;
	const unsigned char *s = RCAST(const unsigned char *, ss);
	unsigned int s3, s2, s1, s0;

	switch (m->str_flags & PSTRING_LEN) {
	case PSTRING_1_LE:
		len = *s;
		break;
	case PSTRING_2_LE:
		s0 = s[0];
		s1 = s[1];
		len = (s1 << 8) | s0;
		break;
	case PSTRING_2_BE:
		s0 = s[0];
		s1 = s[1];
		len = (s0 << 8) | s1;
		break;
	case PSTRING_4_LE:
		s0 = s[0];
		s1 = s[1];
		s2 = s[2];
		s3 = s[3];
		len = (s3 << 24) | (s2 << 16) | (s1 << 8) | s0;
		break;
	case PSTRING_4_BE:
		s0 = s[0];
		s1 = s[1];
		s2 = s[2];
		s3 = s[3];
		len = (s0 << 24) | (s1 << 16) | (s2 << 8) | s3;
		break;
	default:
		file_error(ms, 0, "corrupt magic file "
		    "(bad pascal string length %d)",
		    m->str_flags & PSTRING_LEN);
		return FILE_BADSIZE;
	}

	if (m->str_flags & PSTRING_LENGTH_INCLUDES_ITSELF) {
		size_t l = file_pstring_length_size(ms, m);
		if (l == FILE_BADSIZE)
			return l;
		len -= l;
	}

	return len;
}

protected int
file_magicfind(struct magic_set *ms, const char *name, struct mlist *v)
{
	uint32_t i, j;
	struct mlist *mlist, *ml;

	mlist = ms->mlist[1];

	for (ml = mlist->next; ml != mlist; ml = ml->next) {
		struct magic *ma = ml->magic;
		uint32_t nma = ml->nmagic;
		for (i = 0; i < nma; i++) {
			if (ma[i].type != FILE_NAME)
				continue;
			if (strcmp(ma[i].value.s, name) == 0) {
				v->magic = &ma[i];
				for (j = i + 1; j < nma; j++)
				    if (ma[j].cont_level == 0)
					    break;
				v->nmagic = j - i;
				return 0;
			}
		}
	}
	return -1;
}
