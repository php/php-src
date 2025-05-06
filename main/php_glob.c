/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * glob(3) -- a superset of the one defined in POSIX 1003.2.
 *
 * The [!...] convention to negate a range is supported (SysV, Posix, ksh).
 *
 * Optional extra services, controlled by flags not defined by POSIX:
 *
 * PHP_GLOB_QUOTE:
 *	Escaping convention: \ inhibits any special meaning the following
 *	character might have (except \ at end of string is retained).
 * PHP_GLOB_MAGCHAR:
 *	Set in gl_flags if pattern contained a globbing character.
 * PHP_GLOB_NOMAGIC:
 *	Same as PHP_GLOB_NOCHECK, but it will only append pattern if it did
 *	not contain any magic characters.  [Used in csh style globbing]
 * PHP_GLOB_ALTDIRFUNC:
 *	Use alternately specified directory access functions.
 * PHP_GLOB_TILDE:
 *	expand ~user/foo to the /home/dir/of/user/foo
 * PHP_GLOB_BRACE:
 *	expand {1,2}{a,b} to 1a 1b 2a 2b
 * gl_matchc:
 *	Number of matches in the current invocation of glob.
 */

#include "php_glob.h"

#if !(defined(HAVE_GLOB) && defined(PHP_SYSTEM_GLOB))

#ifdef PHP_WIN32
#if _MSC_VER < 1800
# define _POSIX_
# include <limits.h>
# undef _POSIX_
#else
/* Visual Studio 2013 removed all the _POSIX_ defines, but we depend on some */
# ifndef ARG_MAX
#  define ARG_MAX 14500
# endif
#endif
# ifndef PATH_MAX
#  define PATH_MAX MAXPATHLEN
# endif
/* Windows defines SIZE_MAX but not SSIZE_MAX */
# ifndef SSIZE_MAX
#  ifdef _WIN64
#   define SSIZE_MAX _I64_MAX
#  else
#   define SSIZE_MAX INT_MAX
#  endif
# endif
#endif

#ifndef _PW_BUF_LEN
/* XXX: Should be sysconf(_SC_GETPW_R_SIZE_MAX), but then VLA */
#define _PW_BUF_LEN 4096
#endif

#include "php.h"
#include <sys/stat.h>

#include <ctype.h>
#ifndef PHP_WIN32
#include <sys/param.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charclass.h"

#define	DOLLAR		'$'
#define	DOT		'.'
#define	EOS		'\0'
#define	LBRACKET	'['
#define	NOT		'!'
#define	QUESTION	'?'
#define	QUOTE		'\\'
#define	RANGE		'-'
#define	RBRACKET	']'
#define	SEP		DEFAULT_SLASH
#define	STAR		'*'
#define	TILDE		'~'
#define	UNDERSCORE	'_'
#define	LBRACE		'{'
#define	RBRACE		'}'
#define	SLASH		'/'
#define	COMMA		','

#ifndef DEBUG

#define	M_QUOTE		0x8000
#define	M_PROTECT	0x4000
#define	M_MASK		0xffff
#define	M_ASCII		0x00ff

typedef u_short Char;

#else

#define	M_QUOTE		0x80
#define	M_PROTECT	0x40
#define	M_MASK		0xff
#define	M_ASCII		0x7f

typedef char Char;

#endif


#define	CHAR(c)		((Char)((c)&M_ASCII))
#define	META(c)		((Char)((c)|M_QUOTE))
#define	M_ALL		META('*')
#define	M_END		META(']')
#define	M_NOT		META('!')
#define	M_ONE		META('?')
#define	M_RNG		META('-')
#define	M_SET		META('[')
#define	M_CLASS		META(':')
#define	ismeta(c)	(((c)&M_QUOTE) != 0)

#define	PHP_GLOB_LIMIT_MALLOC	65536
#define	PHP_GLOB_LIMIT_STAT		2048
#define	PHP_GLOB_LIMIT_READDIR	16384

struct glob_lim {
	size_t	glim_malloc;
	size_t	glim_stat;
	size_t	glim_readdir;
};

struct glob_path_stat {
	char		*gps_path;
	zend_stat_t	*gps_stat;
};

#ifndef HAVE_REALLOCARRAY
/*
 * XXX: This is temporary to avoid having reallocarray be imported and part of
 * PHP's public API. Since it's only needed here and on Windows, we can just
 * put it here for now. Convert this file to ZendMM and remove this function
 * when that's complete.
 */

/*	$OpenBSD: reallocarray.c,v 1.3 2015/09/13 08:31:47 guenther Exp $	*/
/*
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))

static void *
reallocarray(void *optr, size_t nmemb, size_t size)
{
	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return NULL;
	}
	return realloc(optr, size * nmemb);
}
#endif

static int	 compare(const void *, const void *);
static int	 compare_gps(const void *, const void *);
static int	 g_Ctoc(const Char *, char *, size_t);
static int	 g_lstat(Char *, zend_stat_t *, php_glob_t *);
static DIR	*g_opendir(Char *, php_glob_t *);
static Char	*g_strchr(const Char *, int);
static int	 g_strncmp(const Char *, const char *, size_t);
static int	 g_stat(Char *, zend_stat_t *, php_glob_t *);
static int	 glob0(const Char *, php_glob_t *, struct glob_lim *);
static int	 glob1(Char *, Char *, php_glob_t *, struct glob_lim *);
static int	 glob2(Char *, Char *, Char *, Char *, Char *, Char *,
				php_glob_t *, struct glob_lim *);
static int	 glob3(Char *, Char *, Char *, Char *, Char *,
				Char *, Char *, php_glob_t *, struct glob_lim *);
static int	 globextend(const Char *, php_glob_t *, struct glob_lim *,
				zend_stat_t *);
static const Char *globtilde(const Char *, Char *, size_t, php_glob_t *);
static int	 globexp1(const Char *, php_glob_t *, struct glob_lim *);
static int	 globexp2(const Char *, const Char *, php_glob_t *,
				struct glob_lim *);
static int	 match(Char *, Char *, Char *);
#ifdef DEBUG
static void	 qprintf(const char *, Char *);
#endif

PHPAPI int php_glob(const char *pattern, int flags, int (*errfunc)(const char *, int), php_glob_t *pglob)
{
	const uint8_t *patnext;
	int c;
	Char *bufnext, *bufend, patbuf[PATH_MAX];
	struct glob_lim limit = { 0, 0, 0 };

#ifdef PHP_WIN32
	/* Force skipping escape sequences on windows
	 * due to the ambiguity with path backslashes
	 */
	flags |= PHP_GLOB_NOESCAPE;
#endif

	patnext = (uint8_t *) pattern;
	if (!(flags & PHP_GLOB_APPEND)) {
		pglob->gl_pathc = 0;
		pglob->gl_pathv = NULL;
		pglob->gl_statv = NULL;
		if (!(flags & PHP_GLOB_DOOFFS))
			pglob->gl_offs = 0;
	}
	pglob->gl_flags = flags & ~PHP_GLOB_MAGCHAR;
	pglob->gl_errfunc = errfunc;
	pglob->gl_matchc = 0;

	if (strnlen(pattern, PATH_MAX) == PATH_MAX)
		return(PHP_GLOB_NOMATCH);

	if (pglob->gl_offs >= SSIZE_MAX || pglob->gl_pathc >= SSIZE_MAX ||
		pglob->gl_pathc >= SSIZE_MAX - pglob->gl_offs - 1)
		return PHP_GLOB_NOSPACE;

	bufnext = patbuf;
	bufend = bufnext + PATH_MAX - 1;
	if (flags & PHP_GLOB_NOESCAPE)
		while (bufnext < bufend && (c = *patnext++) != EOS)
			*bufnext++ = c;
	else {
		/* Protect the quoted characters. */
		while (bufnext < bufend && (c = *patnext++) != EOS)
			if (c == QUOTE) {
				if ((c = *patnext++) == EOS) {
					c = QUOTE;
					--patnext;
				}
				*bufnext++ = c | M_PROTECT;
			} else
				*bufnext++ = c;
	}
	*bufnext = EOS;

	if (flags & PHP_GLOB_BRACE)
		return globexp1(patbuf, pglob, &limit);
	else
		return glob0(patbuf, pglob, &limit);
}

/*
 * Expand recursively a glob {} pattern. When there is no more expansion
 * invoke the standard globbing routine to glob the rest of the magic
 * characters
 */
static int globexp1(const Char *pattern, php_glob_t *pglob, struct glob_lim *limitp)
{
	const Char* ptr = pattern;

	/* Protect a single {}, for find(1), like csh */
	if (pattern[0] == LBRACE && pattern[1] == RBRACE && pattern[2] == EOS)
		return glob0(pattern, pglob, limitp);

	if ((ptr = (const Char *) g_strchr(ptr, LBRACE)) != NULL)
		return globexp2(ptr, pattern, pglob, limitp);

	return glob0(pattern, pglob, limitp);
}


/*
 * Recursive brace globbing helper. Tries to expand a single brace.
 * If it succeeds then it invokes globexp1 with the new pattern.
 * If it fails then it tries to glob the rest of the pattern and returns.
 */
static int globexp2(const Char *ptr, const Char *pattern, php_glob_t *pglob, struct glob_lim *limitp)
{
	int     i, rv;
	Char   *lm, *ls;
	const Char *pe, *pm, *pl;
	Char    patbuf[PATH_MAX];

	/* copy part up to the brace */
	for (lm = patbuf, pm = pattern; pm != ptr; *lm++ = *pm++)
		;
	*lm = EOS;
	ls = lm;

	/* Find the balanced brace */
	for (i = 0, pe = ++ptr; *pe; pe++)
		if (*pe == LBRACKET) {
			/* Ignore everything between [] */
			for (pm = pe++; *pe != RBRACKET && *pe != EOS; pe++)
				;
			if (*pe == EOS) {
				/*
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
				pe = pm;
			}
		} else if (*pe == LBRACE)
			i++;
		else if (*pe == RBRACE) {
			if (i == 0)
				break;
			i--;
		}

	/* Non matching braces; just glob the pattern */
	if (i != 0 || *pe == EOS)
		return glob0(patbuf, pglob, limitp);

	for (i = 0, pl = pm = ptr; pm <= pe; pm++) {
		const Char *pb;

		switch (*pm) {
		case LBRACKET:
			/* Ignore everything between [] */
			for (pb = pm++; *pm != RBRACKET && *pm != EOS; pm++)
				;
			if (*pm == EOS) {
				/*
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
				pm = pb;
			}
			break;

		case LBRACE:
			i++;
			break;

		case RBRACE:
			if (i) {
				i--;
				break;
			}
			/* FALLTHROUGH */
		case COMMA:
			if (i && *pm == COMMA)
				break;
			else {
				/* Append the current string */
				for (lm = ls; (pl < pm); *lm++ = *pl++)
					;

				/*
				 * Append the rest of the pattern after the
				 * closing brace
				 */
				for (pl = pe + 1; (*lm++ = *pl++) != EOS; )
					;

				/* Expand the current pattern */
#ifdef DEBUG
				qprintf("globexp2:", patbuf);
#endif
				rv = globexp1(patbuf, pglob, limitp);
				if (rv && rv != PHP_GLOB_NOMATCH)
					return rv;

				/* move after the comma, to the next string */
				pl = pm + 1;
			}
			break;

		default:
			break;
		}
	}
	return 0;
}



/*
 * expand tilde from the passwd file.
 */
static const Char *globtilde(const Char *pattern, Char *patbuf, size_t patbuf_len, php_glob_t *pglob)
{
#ifndef PHP_WIN32
	struct passwd pwstore, *pwd = NULL;
	char pwbuf[_PW_BUF_LEN];
#endif
	char *h;
	const Char *p;
	Char *b, *eb;

	if (*pattern != TILDE || !(pglob->gl_flags & PHP_GLOB_TILDE))
		return pattern;

	/* Copy up to the end of the string or / */
	eb = &patbuf[patbuf_len - 1];
	for (p = pattern + 1, h = (char *) patbuf;
		h < (char *)eb && *p && *p != SLASH; *h++ = *p++)
		;

	*h = EOS;

#if 0
	if (h == (char *)eb)
		return what;
#endif

	if (((char *) patbuf)[0] == EOS) {
#ifndef PHP_WIN32
		/*
		 * handle a plain ~ or ~/ by expanding $HOME
		 * first and then trying the password file
		 */
#ifdef HAVE_ISSETUGID
		if (issetugid() != 0 || (h = getenv("HOME")) == NULL) {
#else
		if ((h = getenv("HOME")) == NULL) {
#endif
			getpwuid_r(getuid(), &pwstore, pwbuf, sizeof(pwbuf),
				&pwd);
			if (pwd == NULL)
				return pattern;
			else
				h = pwd->pw_dir;
		}
#else
		return pattern;
#endif
	} else {
		/*
		 * Expand a ~user
		 */
#ifndef PHP_WIN32
		getpwnam_r((char *)patbuf, &pwstore, pwbuf, sizeof(pwbuf),
			&pwd);
		if (pwd == NULL)
			return pattern;
		else
			h = pwd->pw_dir;
#else
		return pattern;
#endif
	}

	/* Copy the home directory */
	for (b = patbuf; b < eb && *h; *b++ = *h++)
		;

	/* Append the rest of the pattern */
	while (b < eb && (*b++ = *p++) != EOS)
		;
	*b = EOS;

	return patbuf;
}

static int g_strncmp(const Char *s1, const char *s2, size_t n)
{
	int rv = 0;

	while (n--) {
		rv = *(Char *)s1 - *(const unsigned char *)s2++;
		if (rv)
			break;
		if (*s1++ == '\0')
			break;
	}
	return rv;
}

static int g_charclass(const Char **patternp, Char **bufnextp)
{
	const Char *pattern = *patternp + 1;
	Char *bufnext = *bufnextp;
	const Char *colon;
	const struct cclass *cc;
	size_t len;

	if ((colon = g_strchr(pattern, ':')) == NULL || colon[1] != ']')
		return 1;	/* not a character class */

	len = (size_t)(colon - pattern);
	for (cc = cclasses; cc->name != NULL; cc++) {
		if (!g_strncmp(pattern, cc->name, len) && cc->name[len] == '\0')
			break;
	}
	if (cc->name == NULL)
		return -1;	/* invalid character class */
	*bufnext++ = M_CLASS;
	*bufnext++ = (Char)(cc - &cclasses[0]);
	*bufnextp = bufnext;
	*patternp += len + 3;

	return 0;
}

/*
 * The main glob() routine: compiles the pattern (optionally processing
 * quotes), calls glob1() to do the real pattern matching, and finally
 * sorts the list (unless unsorted operation is requested).  Returns 0
 * if things went well, nonzero if errors occurred.  It is not an error
 * to find no matches.
 */
static int glob0(const Char *pattern, php_glob_t *pglob, struct glob_lim *limitp)
{
	const Char *qpatnext;
	int c, err;
	size_t oldpathc;
	Char *bufnext, patbuf[PATH_MAX];

	qpatnext = globtilde(pattern, patbuf, PATH_MAX, pglob);
	oldpathc = pglob->gl_pathc;
	bufnext = patbuf;

	/* We don't need to check for buffer overflow any more. */
	while ((c = *qpatnext++) != EOS) {
		switch (c) {
		case LBRACKET:
			c = *qpatnext;
			if (c == NOT)
				++qpatnext;
			if (*qpatnext == EOS ||
				g_strchr(qpatnext+1, RBRACKET) == NULL) {
				*bufnext++ = LBRACKET;
				if (c == NOT)
					--qpatnext;
				break;
			}
			*bufnext++ = M_SET;
			if (c == NOT)
				*bufnext++ = M_NOT;
			c = *qpatnext++;
			do {
				if (c == LBRACKET && *qpatnext == ':') {
					do {
						err = g_charclass(&qpatnext,
							&bufnext);
						if (err)
							break;
						c = *qpatnext++;
					} while (c == LBRACKET && *qpatnext == ':');
					if (err == -1 &&
						!(pglob->gl_flags & PHP_GLOB_NOCHECK))
						return PHP_GLOB_NOMATCH;
					if (c == RBRACKET)
						break;
				}
				*bufnext++ = CHAR(c);
				if (*qpatnext == RANGE &&
					(c = qpatnext[1]) != RBRACKET) {
					*bufnext++ = M_RNG;
					*bufnext++ = CHAR(c);
					qpatnext += 2;
				}
			} while ((c = *qpatnext++) != RBRACKET);
			pglob->gl_flags |= PHP_GLOB_MAGCHAR;
			*bufnext++ = M_END;
			break;
		case QUESTION:
			pglob->gl_flags |= PHP_GLOB_MAGCHAR;
			*bufnext++ = M_ONE;
			break;
		case STAR:
			pglob->gl_flags |= PHP_GLOB_MAGCHAR;
			/* collapse adjacent stars to one,
			 * to avoid exponential behavior
			 */
			if (bufnext == patbuf || bufnext[-1] != M_ALL)
				*bufnext++ = M_ALL;
			break;
		default:
			*bufnext++ = CHAR(c);
			break;
		}
	}
	*bufnext = EOS;
#ifdef DEBUG
	qprintf("glob0:", patbuf);
#endif

	if ((err = glob1(patbuf, patbuf+PATH_MAX-1, pglob, limitp)) != 0)
		return(err);

	/*
	 * If there was no match we are going to append the pattern
	 * if PHP_GLOB_NOCHECK was specified or if PHP_GLOB_NOMAGIC was specified
	 * and the pattern did not contain any magic characters
	 * PHP_GLOB_NOMAGIC is there just for compatibility with csh.
	 */
	if (pglob->gl_pathc == oldpathc) {
		if ((pglob->gl_flags & PHP_GLOB_NOCHECK) ||
			((pglob->gl_flags & PHP_GLOB_NOMAGIC) &&
			!(pglob->gl_flags & PHP_GLOB_MAGCHAR)))
			return(globextend(pattern, pglob, limitp, NULL));
		else
			return(PHP_GLOB_NOMATCH);
	}
	if (!(pglob->gl_flags & PHP_GLOB_NOSORT)) {
		if ((pglob->gl_flags & PHP_GLOB_KEEPSTAT)) {
			/* Keep the paths and stat info synced during sort */
			struct glob_path_stat *path_stat;
			size_t i;
			size_t n = pglob->gl_pathc - oldpathc;
			size_t o = pglob->gl_offs + oldpathc;

			if ((path_stat = calloc(n, sizeof(*path_stat))) == NULL)
				return PHP_GLOB_NOSPACE;
			for (i = 0; i < n; i++) {
				path_stat[i].gps_path = pglob->gl_pathv[o + i];
				path_stat[i].gps_stat = pglob->gl_statv[o + i];
			}
			qsort(path_stat, n, sizeof(*path_stat), compare_gps);
			for (i = 0; i < n; i++) {
				pglob->gl_pathv[o + i] = path_stat[i].gps_path;
				pglob->gl_statv[o + i] = path_stat[i].gps_stat;
			}
			free(path_stat);
		} else {
			qsort(pglob->gl_pathv + pglob->gl_offs + oldpathc,
				pglob->gl_pathc - oldpathc, sizeof(char *),
				compare);
		}
	}
	return(0);
}

static int compare(const void *p, const void *q)
{
	return(strcmp(*(char **)p, *(char **)q));
}

static int compare_gps(const void *_p, const void *_q)
{
	const struct glob_path_stat *p = (const struct glob_path_stat *)_p;
	const struct glob_path_stat *q = (const struct glob_path_stat *)_q;

	return(strcmp(p->gps_path, q->gps_path));
}

static int glob1(Char *pattern, Char *pattern_last, php_glob_t *pglob, struct glob_lim *limitp)
{
	Char pathbuf[PATH_MAX];

	/* A null pathname is invalid -- POSIX 1003.1 sect. 2.4. */
	if (*pattern == EOS)
		return(0);
	return(glob2(pathbuf, pathbuf+PATH_MAX-1,
		pathbuf, pathbuf+PATH_MAX-1,
		pattern, pattern_last, pglob, limitp));
}

/*
 * The functions glob2 and glob3 are mutually recursive; there is one level
 * of recursion for each segment in the pattern that contains one or more
 * meta characters.
 */
static int glob2(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last, Char *pattern, Char *pattern_last, php_glob_t *pglob, struct glob_lim *limitp)
{
	zend_stat_t sb;
	Char *p, *q;
	int anymeta;

	/*
	 * Loop over pattern segments until end of pattern or until
	 * segment with meta character found.
	 */
	for (anymeta = 0;;) {
		if (*pattern == EOS) {		/* End of pattern? */
			*pathend = EOS;

			if ((pglob->gl_flags & PHP_GLOB_LIMIT) &&
				limitp->glim_stat++ >= PHP_GLOB_LIMIT_STAT) {
				errno = 0;
				*pathend++ = SEP;
				*pathend = EOS;
				return(PHP_GLOB_NOSPACE);
			}
			if (g_lstat(pathbuf, &sb, pglob))
				return(0);

			if (((pglob->gl_flags & PHP_GLOB_MARK) &&
				pathend[-1] != SEP) && (S_ISDIR(sb.st_mode) ||
				(S_ISLNK(sb.st_mode) &&
				(g_stat(pathbuf, &sb, pglob) == 0) &&
				S_ISDIR(sb.st_mode)))) {
				if (pathend+1 > pathend_last)
					return (1);
				*pathend++ = SEP;
				*pathend = EOS;
			}
			++pglob->gl_matchc;
			return(globextend(pathbuf, pglob, limitp, &sb));
		}

		/* Find end of next segment, copy tentatively to pathend. */
		q = pathend;
		p = pattern;
		while (*p != EOS && !IS_SLASH(*p)) {
			if (ismeta(*p))
				anymeta = 1;
			if (q+1 > pathend_last)
				return (1);
			*q++ = *p++;
		}

		if (!anymeta) {		/* No expansion, do next segment. */
			pathend = q;
			pattern = p;
			while (IS_SLASH(*pattern)) {
				if (pathend+1 > pathend_last)
					return (1);
				*pathend++ = *pattern++;
			}
		} else
			/* Need expansion, recurse. */
			return(glob3(pathbuf, pathbuf_last, pathend,
				pathend_last, pattern, p, pattern_last,
				pglob, limitp));
	}
	/* NOTREACHED */
}

static int glob3(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last, Char *pattern, Char *restpattern, Char *restpattern_last, php_glob_t *pglob, struct glob_lim *limitp)
{
	struct dirent *dp;
	DIR *dirp;
	int err;
	char buf[PATH_MAX];

	/*
	 * The readdirfunc declaration can't be prototyped, because it is
	 * assigned, below, to two functions which are prototyped in glob.h
	 * and dirent.h as taking pointers to differently typed opaque
	 * structures.
	 */
	struct dirent *(*readdirfunc)(void *);

	if (pathend > pathend_last)
		return (1);
	*pathend = EOS;
	errno = 0;

	if ((dirp = g_opendir(pathbuf, pglob)) == NULL) {
		/* TODO: don't call for ENOENT or ENOTDIR? */
		if (pglob->gl_errfunc) {
			if (g_Ctoc(pathbuf, buf, sizeof(buf)))
				return(PHP_GLOB_ABORTED);
			if (pglob->gl_errfunc(buf, errno) ||
				pglob->gl_flags & PHP_GLOB_ERR)
				return(PHP_GLOB_ABORTED);
		}
		return(0);
	}

	err = 0;

	/* Search directory for matching names. */
	if (pglob->gl_flags & PHP_GLOB_ALTDIRFUNC)
		readdirfunc = pglob->gl_readdir;
	else
		readdirfunc = (struct dirent *(*)(void *))readdir;
	while ((dp = (*readdirfunc)(dirp))) {
		uint8_t *sc;
		Char *dc;

		if ((pglob->gl_flags & PHP_GLOB_LIMIT) &&
			limitp->glim_readdir++ >= PHP_GLOB_LIMIT_READDIR) {
			errno = 0;
			*pathend++ = SEP;
			*pathend = EOS;
			err = PHP_GLOB_NOSPACE;
			break;
		}

		/* Initial DOT must be matched literally. */
		if (dp->d_name[0] == DOT && *pattern != DOT)
			continue;
		dc = pathend;
		sc = (uint8_t *) dp->d_name;
		while (dc < pathend_last && (*dc++ = *sc++) != EOS)
			;
		if (dc >= pathend_last) {
			*dc = EOS;
			err = 1;
			break;
		}

		if (!match(pathend, pattern, restpattern)) {
			*pathend = EOS;
			continue;
		}
		err = glob2(pathbuf, pathbuf_last, --dc, pathend_last,
			restpattern, restpattern_last, pglob, limitp);
		if (err)
			break;
	}

	if (pglob->gl_flags & PHP_GLOB_ALTDIRFUNC)
		(*pglob->gl_closedir)(dirp);
	else
		closedir(dirp);
	return(err);
}


/*
 * Extend the gl_pathv member of a php_glob_t structure to accommodate a new item,
 * add the new item, and update gl_pathc.
 *
 * This assumes the BSD realloc, which only copies the block when its size
 * crosses a power-of-two boundary; for v7 realloc, this would cause quadratic
 * behavior.
 *
 * Return 0 if new item added, error code if memory couldn't be allocated.
 *
 * Invariant of the php_glob_t structure:
 *	Either gl_pathc is zero and gl_pathv is NULL; or gl_pathc > 0 and
 *	gl_pathv points to (gl_offs + gl_pathc + 1) items.
 */
static int globextend(const Char *path, php_glob_t *pglob, struct glob_lim *limitp, zend_stat_t *sb)
{
	char **pathv;
	size_t i, newn, len;
	char *copy = NULL;
	const Char *p;
	zend_stat_t **statv;

	newn = 2 + pglob->gl_pathc + pglob->gl_offs;
	if (pglob->gl_offs >= SSIZE_MAX ||
		pglob->gl_pathc >= SSIZE_MAX ||
		newn >= SSIZE_MAX ||
		SIZE_MAX / sizeof(*pathv) <= newn ||
		SIZE_MAX / sizeof(*statv) <= newn) {
 nospace:
		for (i = pglob->gl_offs; i < newn - 2; i++) {
			if (pglob->gl_pathv && pglob->gl_pathv[i])
				free(pglob->gl_pathv[i]);
			if ((pglob->gl_flags & PHP_GLOB_KEEPSTAT) != 0 &&
				pglob->gl_pathv && pglob->gl_pathv[i])
				free(pglob->gl_statv[i]);
		}
		free(pglob->gl_pathv);
		pglob->gl_pathv = NULL;
		free(pglob->gl_statv);
		pglob->gl_statv = NULL;
		return(PHP_GLOB_NOSPACE);
	}

	pathv = reallocarray(pglob->gl_pathv, newn, sizeof(*pathv));
	if (pathv == NULL)
		goto nospace;
	if (pglob->gl_pathv == NULL && pglob->gl_offs > 0) {
		/* first time around -- clear initial gl_offs items */
		pathv += pglob->gl_offs;
		for (i = pglob->gl_offs; i > 0; i--)
			*--pathv = NULL;
	}
	pglob->gl_pathv = pathv;

	if ((pglob->gl_flags & PHP_GLOB_KEEPSTAT) != 0) {
		statv = reallocarray(pglob->gl_statv, newn, sizeof(*statv));
		if (statv == NULL)
			goto nospace;
		if (pglob->gl_statv == NULL && pglob->gl_offs > 0) {
			/* first time around -- clear initial gl_offs items */
			statv += pglob->gl_offs;
			for (i = pglob->gl_offs; i > 0; i--)
				*--statv = NULL;
		}
		pglob->gl_statv = statv;
		if (sb == NULL)
			statv[pglob->gl_offs + pglob->gl_pathc] = NULL;
		else {
			limitp->glim_malloc += sizeof(**statv);
			if ((pglob->gl_flags & PHP_GLOB_LIMIT) &&
				limitp->glim_malloc >= PHP_GLOB_LIMIT_MALLOC) {
				errno = 0;
				return(PHP_GLOB_NOSPACE);
			}
			if ((statv[pglob->gl_offs + pglob->gl_pathc] =
				malloc(sizeof(**statv))) == NULL)
				goto copy_error;
			memcpy(statv[pglob->gl_offs + pglob->gl_pathc], sb,
				sizeof(*sb));
		}
		statv[pglob->gl_offs + pglob->gl_pathc + 1] = NULL;
	}

	for (p = path; *p++;)
		;
	len = (size_t)(p - path);
	limitp->glim_malloc += len;
	if ((copy = malloc(len)) != NULL) {
		if (g_Ctoc(path, copy, len)) {
			free(copy);
			return(PHP_GLOB_NOSPACE);
		}
		pathv[pglob->gl_offs + pglob->gl_pathc++] = copy;
	}
	pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

	if ((pglob->gl_flags & PHP_GLOB_LIMIT) &&
		(newn * sizeof(*pathv)) + limitp->glim_malloc >
		PHP_GLOB_LIMIT_MALLOC) {
		errno = 0;
		return(PHP_GLOB_NOSPACE);
	}
 copy_error:
	return(copy == NULL ? PHP_GLOB_NOSPACE : 0);
}


/*
 * pattern matching function for filenames.  Each occurrence of the *
 * pattern causes an iteration.
 *
 * Note, this function differs from the original as per the discussion
 * here: https://research.swtch.com/glob
 *
 * Basically we removed the recursion and made it use the algorithm
 * from Russ Cox to not go quadratic on cases like a file called
 * ("a" x 100) . "x" matched against a pattern like "a*a*a*a*a*a*a*y".
 */
static int match(Char *name, Char *pat, Char *patend)
{
	int ok, negate_range;
	Char c, k;
	Char *nextp = NULL;
	Char *nextn = NULL;

loop:
	while (pat < patend) {
		c = *pat++;
		switch (c & M_MASK) {
		case M_ALL:
			while (pat < patend && (*pat & M_MASK) == M_ALL)
				pat++;	/* eat consecutive '*' */
			if (pat == patend)
				return(1);
			if (*name == EOS)
				return(0);
			nextn = name + 1;
			nextp = pat - 1;
			break;
		case M_ONE:
			if (*name++ == EOS)
				goto fail;
			break;
		case M_SET:
			ok = 0;
			if ((k = *name++) == EOS)
				goto fail;
			if ((negate_range = ((*pat & M_MASK) == M_NOT)) != EOS)
				++pat;
			while (((c = *pat++) & M_MASK) != M_END) {
				if ((c & M_MASK) == M_CLASS) {
					Char idx = *pat & M_MASK;
					if (idx < NCCLASSES &&
						cclasses[idx].isctype(k))
						ok = 1;
					++pat;
				}
				if ((*pat & M_MASK) == M_RNG) {
					if (c <= k && k <= pat[1])
						ok = 1;
					pat += 2;
				} else if (c == k)
					ok = 1;
			}
			if (ok == negate_range)
				goto fail;
			break;
		default:
			if (*name++ != c)
				goto fail;
			break;
		}
	}
	if (*name == EOS)
		return(1);

fail:
	if (nextn) {
		pat = nextp;
		name = nextn;
		goto loop;
	}
	return(0);
}

/* Free allocated data belonging to a php_glob_t structure. */
PHPAPI void php_globfree(php_glob_t *pglob)
{
	size_t i;
	char **pp;

	if (pglob->gl_pathv != NULL) {
		pp = pglob->gl_pathv + pglob->gl_offs;
		for (i = pglob->gl_pathc; i--; ++pp)
			free(*pp);
		free(pglob->gl_pathv);
		pglob->gl_pathv = NULL;
	}
	if (pglob->gl_statv != NULL) {
		for (i = 0; i < pglob->gl_pathc; i++) {
			free(pglob->gl_statv[i]);
		}
		free(pglob->gl_statv);
		pglob->gl_statv = NULL;
	}
}

static DIR *g_opendir(Char *str, php_glob_t *pglob)
{
	char buf[PATH_MAX];

	if (!*str)
		strlcpy(buf, ".", sizeof buf);
	else {
		if (g_Ctoc(str, buf, sizeof(buf)))
			return(NULL);
	}

	if (pglob->gl_flags & PHP_GLOB_ALTDIRFUNC)
		return((*pglob->gl_opendir)(buf));

	return(opendir(buf));
}

static int g_lstat(Char *fn, zend_stat_t *sb, php_glob_t *pglob)
{
	char buf[PATH_MAX];

	if (g_Ctoc(fn, buf, sizeof(buf)))
		return(-1);
	if (pglob->gl_flags & PHP_GLOB_ALTDIRFUNC)
		return((*pglob->gl_lstat)(buf, sb));
	return(php_sys_lstat(buf, sb));
}

static int g_stat(Char *fn, zend_stat_t *sb, php_glob_t *pglob)
{
	char buf[PATH_MAX];

	if (g_Ctoc(fn, buf, sizeof(buf)))
		return(-1);
	if (pglob->gl_flags & PHP_GLOB_ALTDIRFUNC)
		return((*pglob->gl_stat)(buf, sb));
	return(php_sys_stat(buf, sb));
}

static Char *g_strchr(const Char *str, int ch)
{
	do {
		if (*str == ch)
			return ((Char *)str);
	} while (*str++);
	return (NULL);
}

static int g_Ctoc(const Char *str, char *buf, size_t len)
{

	while (len--) {
		if ((*buf++ = *str++) == EOS)
			return (0);
	}
	return (1);
}

#ifdef DEBUG
static void qprintf(const char *str, Char *s)
{
	Char *p;

	(void)printf("%s:\n", str);
	for (p = s; *p; p++)
		(void)printf("%c", CHAR(*p));
	(void)printf("\n");
	for (p = s; *p; p++)
		(void)printf("%c", *p & M_PROTECT ? '"' : ' ');
	(void)printf("\n");
	for (p = s; *p; p++)
		(void)printf("%c", ismeta(*p) ? '_' : ' ');
	(void)printf("\n");
}
#endif

#endif /* defined(HAVE_GLOB) */
