/*	$OpenBSD: glob.h,v 1.14 2019/02/04 16:45:40 millert Exp $	*/
/*	$NetBSD: glob.h,v 1.5 1994/10/26 00:55:56 cgd Exp $	*/

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
 *
 *	@(#)glob.h	8.1 (Berkeley) 6/2/93
 */

#ifdef PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif

#ifndef _PHP_GLOB_H_
#define	_PHP_GLOB_H_

#if defined(HAVE_GLOB) && defined(PHP_SYSTEM_GLOB)
#include <glob.h>

#ifdef GLOB_APPEND
#define PHP_GLOB_APPEND GLOB_APPEND
#endif
#ifdef GLOB_DOOFFS
#define PHP_GLOB_DOOFFS GLOB_DOOFFS
#endif
#ifdef GLOB_ERR
#define PHP_GLOB_ERR GLOB_ERR
#endif
#ifdef GLOB_MARK
#define PHP_GLOB_MARK GLOB_MARK
#endif
#ifdef GLOB_NOCHECK
#define PHP_GLOB_NOCHECK GLOB_NOCHECK
#endif
#ifdef GLOB_NOSORT
#define PHP_GLOB_NOSORT GLOB_NOSORT
#endif
#ifdef GLOB_NOESCAPE
#define PHP_GLOB_NOESCAPE GLOB_NOESCAPE
#endif
#ifdef GLOB_NOSPACE
#define PHP_GLOB_NOSPACE GLOB_NOSPACE
#endif
#ifdef GLOB_ABORTED
#define PHP_GLOB_ABORTED GLOB_ABORTED
#endif
#ifdef GLOB_NOMATCH
#define PHP_GLOB_NOMATCH GLOB_NOMATCH
#endif
#ifdef GLOB_NOSYS
#define PHP_GLOB_NOSYS GLOB_NOSYS
#endif
#ifdef GLOB_ALTDIRFUNC
#define PHP_GLOB_ALTDIRFUNC GLOB_ALTDIRFUNC
#endif
#ifdef GLOB_BRACE
#define PHP_GLOB_BRACE GLOB_BRACE
#endif
#ifdef GLOB_MAGCHAR
#define PHP_GLOB_MAGCHAR GLOB_MAGCHAR
#endif
#ifdef GLOB_NOMAGIC
#define PHP_GLOB_NOMAGIC GLOB_NOMAGIC
#endif
#ifdef GLOB_QUOTE
#define PHP_GLOB_QUOTE GLOB_QUOTE
#endif
#ifdef GLOB_TILDE
#define PHP_GLOB_TILDE GLOB_TILDE
#endif
#ifdef GLOB_LIMIT
#define PHP_GLOB_LIMIT GLOB_LIMIT
#endif
#ifdef GLOB_KEEPSTAT
#define PHP_GLOB_KEEPSTAT GLOB_KEEPSTAT
#endif

#define php_glob_t glob_t
#define php_glob glob
#define php_globfree globfree
#else

#include "php.h"
#include "Zend/zend_stream.h"

typedef struct {
	size_t gl_pathc;	/* Count of total paths so far. */
	size_t gl_matchc;	/* Count of paths matching pattern. */
	size_t gl_offs;		/* Reserved at beginning of gl_pathv. */
	int gl_flags;		/* Copy of flags parameter to glob. */
	char **gl_pathv;	/* List of paths matching pattern. */
	zend_stat_t **gl_statv;	/* Stat entries corresponding to gl_pathv */
				/* Copy of errfunc parameter to glob. */
	int (*gl_errfunc)(const char *, int);

	/*
	 * Alternate filesystem access methods for glob; replacement
	 * versions of closedir(3), readdir(3), opendir(3), stat(2)
	 * and lstat(2).
	 */
	void (*gl_closedir)(void *);
	struct dirent *(*gl_readdir)(void *);
	void *(*gl_opendir)(const char *);
	int (*gl_lstat)(const char *, zend_stat_t *);
	int (*gl_stat)(const char *, zend_stat_t *);
} php_glob_t;

#define	PHP_GLOB_APPEND	0x0001	/* Append to output from previous call. */
#define	PHP_GLOB_DOOFFS	0x0002	/* Use gl_offs. */
#define	PHP_GLOB_ERR	0x0004	/* Return on error. */
#define	PHP_GLOB_MARK	0x0008	/* Append / to matching directories. */
#define	PHP_GLOB_NOCHECK	0x0010	/* Return pattern itself if nothing matches. */
#define	PHP_GLOB_NOSORT	0x0020	/* Don't sort. */
#define	PHP_GLOB_NOESCAPE	0x1000	/* Disable backslash escaping. */

#define	PHP_GLOB_NOSPACE	(-1)	/* Malloc call failed. */
#define	PHP_GLOB_ABORTED	(-2)	/* Unignored error. */
#define	PHP_GLOB_NOMATCH	(-3)	/* No match and PHP_GLOB_NOCHECK not set. */
#define	PHP_GLOB_NOSYS	(-4)	/* Function not supported. */

#define	PHP_GLOB_ALTDIRFUNC	0x0040	/* Use alternately specified directory funcs. */
#define	PHP_GLOB_BRACE	0x0080	/* Expand braces ala csh. */
#define	PHP_GLOB_MAGCHAR	0x0100	/* Pattern had globbing characters. */
#define	PHP_GLOB_NOMAGIC	0x0200	/* PHP_GLOB_NOCHECK without magic chars (csh). */
#define	PHP_GLOB_QUOTE	0x0400	/* Quote special chars with \. */
#define	PHP_GLOB_TILDE	0x0800	/* Expand tilde names from the passwd file. */
#define PHP_GLOB_LIMIT	0x2000	/* Limit pattern match output to ARG_MAX */
#define	PHP_GLOB_KEEPSTAT	0x4000	/* Retain stat data for paths in gl_statv. */

BEGIN_EXTERN_C()
PHPAPI int	php_glob(const char *__restrict, int, int (*)(const char *, int),
	    php_glob_t *__restrict);
PHPAPI void	php_globfree(php_glob_t *);
END_EXTERN_C()

#endif /* defined(HAVE_GLOB) */

/* These were copied from dir and zip */

#ifndef PHP_GLOB_ONLYDIR
#define PHP_GLOB_ONLYDIR (1<<30)
#define PHP_GLOB_FLAGMASK (~PHP_GLOB_ONLYDIR)
#else
#define PHP_GLOB_FLAGMASK (~0)
#endif

#define PHP_GLOB_AVAILABLE_FLAGS (0 | PHP_GLOB_BRACE | PHP_GLOB_MARK | PHP_GLOB_NOSORT | PHP_GLOB_NOCHECK | PHP_GLOB_NOESCAPE | PHP_GLOB_ERR | PHP_GLOB_ONLYDIR)

#endif /* !_GLOB_H_ */
