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
 * file - find type of a file or files - main program.
 */

#include "file.h"
#include "magic.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>	/* for MAXPATHLEN */
#include <sys/stat.h>
#ifdef RESTORE_TIME
# if (__COHERENT__ >= 0x420)
#  include <sys/utime.h>
# else
#  ifdef USE_UTIMES
#   include <sys/time.h>
#  else
#   include <utime.h>
#  endif
# endif
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>	/* for read() */
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif

#include <getopt.h>
#ifndef HAVE_GETOPT_LONG
int getopt_long(int argc, char * const *argv, const char *optstring, const struct option *longopts, int *longindex);
#endif

#include <netinet/in.h>		/* for byte swapping */

#include "patchlevel.h"

#ifndef	lint
FILE_RCSID("@(#)$File: file.c,v 1.119 2008/02/07 00:58:52 christos Exp $")
#endif	/* lint */


#ifdef S_IFLNK
#define SYMLINKFLAG "Lh"
#else
#define SYMLINKFLAG ""
#endif

# define USAGE  "Usage: %s [-bcik" SYMLINKFLAG "nNrsvz0] [-e test] [-f namefile] [-F separator] [-m magicfiles] file...\n       %s -C -m magicfiles\n"

#ifndef MAXPATHLEN
#define	MAXPATHLEN	512
#endif

private int 		/* Global command-line options 		*/
	bflag = 0,	/* brief output format	 		*/
	nopad = 0,	/* Don't pad output			*/
	nobuffer = 0,   /* Do not buffer stdout 		*/
	nulsep = 0;	/* Append '\0' to the separator		*/

private const char *magicfile = 0;	/* where the magic is	*/
private const char *default_magicfile = MAGIC;
private const char *separator = ":";	/* Default field separator	*/

private char *progname;		/* used throughout 		*/

private struct magic_set *magic;

private void unwrap(char *);
private void usage(void);
private void help(void);

int main(int, char *[]);
private void process(const char *, int);
private void load(const char *, int);


/*
 * main - parse arguments and handle options
 */
int
main(int argc, char *argv[])
{
	int c;
	size_t i;
	int action = 0, didsomefiles = 0, errflg = 0;
	int flags = 0;
	char *home, *usermagic;
	struct stat sb;
	static const char hmagic[] = "/.magic";
#define OPTSTRING	"bcCde:f:F:hikLm:nNprsvz0"
	int longindex;
	static const struct option long_options[] =
	{
#define OPT(shortname, longname, opt, doc)      \
    {longname, opt, NULL, shortname},
#define OPT_LONGONLY(longname, opt, doc)        \
    {longname, opt, NULL, 0},
#include "file_opts.h"
#undef OPT
#undef OPT_LONGONLY
    {0, 0, NULL, 0}
};

	static const struct {
		const char *name;
		int value;
	} nv[] = {
		{ "apptype",	MAGIC_NO_CHECK_APPTYPE },
		{ "ascii",	MAGIC_NO_CHECK_ASCII },
		{ "compress",	MAGIC_NO_CHECK_COMPRESS },
		{ "elf",	MAGIC_NO_CHECK_ELF },
		{ "soft",	MAGIC_NO_CHECK_SOFT },
		{ "tar",	MAGIC_NO_CHECK_TAR },
		{ "tokens",	MAGIC_NO_CHECK_TOKENS },
	};

	/* makes islower etc work for other langs */
	(void)setlocale(LC_CTYPE, "");

#ifdef __EMX__
	/* sh-like wildcard expansion! Shouldn't hurt at least ... */
	_wildcard(&argc, &argv);
#endif

	if ((progname = strrchr(argv[0], '/')) != NULL)
		progname++;
	else
		progname = argv[0];

	magicfile = default_magicfile;
	if ((usermagic = getenv("MAGIC")) != NULL)
		magicfile = usermagic;
	else
		if ((home = getenv("HOME")) != NULL) {
			if ((usermagic = malloc(strlen(home)
			    + sizeof(hmagic))) != NULL) {
				(void)strcpy(usermagic, home);
				(void)strcat(usermagic, hmagic);
				if (stat(usermagic, &sb)<0) 
					free(usermagic);
				else
					magicfile = usermagic;
			}
		}

#ifdef S_IFLNK
	flags |= getenv("POSIXLY_CORRECT") ? MAGIC_SYMLINK : 0;
#endif
	while ((c = getopt_long(argc, argv, OPTSTRING, long_options,
	    &longindex)) != -1)
		switch (c) {
		case 0 :
			switch (longindex) {
			case 0:
				help();
				break;
			case 10:
				flags |= MAGIC_MIME_TYPE;
				break;
			case 11:
				flags |= MAGIC_MIME_ENCODING;
				break;
			}
			break;
		case '0':
			nulsep = 1;
			break;
		case 'b':
			bflag++;
			break;
		case 'c':
			action = FILE_CHECK;
			break;
		case 'C':
			action = FILE_COMPILE;
			break;
		case 'd':
			flags |= MAGIC_DEBUG|MAGIC_CHECK;
			break;
		case 'e':
			for (i = 0; i < sizeof(nv) / sizeof(nv[0]); i++)
				if (strcmp(nv[i].name, optarg) == 0)
					break;

			if (i == sizeof(nv) / sizeof(nv[0]))
				errflg++;
			else
				flags |= nv[i].value;
			break;
			
		case 'f':
			if(action)
				usage();
			load(magicfile, flags);
			unwrap(optarg);
			++didsomefiles;
			break;
		case 'F':
			separator = optarg;
			break;
		case 'i':
			flags |= MAGIC_MIME;
			break;
		case 'k':
			flags |= MAGIC_CONTINUE;
			break;
		case 'm':
			magicfile = optarg;
			break;
		case 'n':
			++nobuffer;
			break;
		case 'N':
			++nopad;
			break;
#if defined(HAVE_UTIME) || defined(HAVE_UTIMES)
		case 'p':
			flags |= MAGIC_PRESERVE_ATIME;
			break;
#endif
		case 'r':
			flags |= MAGIC_RAW;
			break;
		case 's':
			flags |= MAGIC_DEVICES;
			break;
		case 'v':
			(void)fprintf(stderr, "%s-%d.%.2d\n", progname,
				       FILE_VERSION_MAJOR, patchlevel);
			(void)fprintf(stderr, "magic file from %s\n",
				       magicfile);
			return 1;
		case 'z':
			flags |= MAGIC_COMPRESS;
			break;
#ifdef S_IFLNK
		case 'L':
			flags |= MAGIC_SYMLINK;
			break;
		case 'h':
			flags &= ~MAGIC_SYMLINK;
			break;
#endif
		case '?':
		default:
			errflg++;
			break;
		}

	if (errflg) {
		usage();
	}

	switch(action) {
	case FILE_CHECK:
	case FILE_COMPILE:
		magic = magic_open(flags|MAGIC_CHECK);
		if (magic == NULL) {
			(void)fprintf(stderr, "%s: %s\n", progname,
			    strerror(errno));
			return 1;
		}
		c = action == FILE_CHECK ? magic_check(magic, magicfile) :
		    magic_compile(magic, magicfile);
		if (c == -1) {
			(void)fprintf(stderr, "%s: %s\n", progname,
			    magic_error(magic));
			return -1;
		}
		return 0;
	default:
		load(magicfile, flags);
		break;
	}

	if (optind == argc) {
		if (!didsomefiles) {
			usage();
		}
	}
	else {
		size_t j, wid, nw;
		for (wid = 0, j = (size_t)optind; j < (size_t)argc; j++) {
			nw = file_mbswidth(argv[j]);
			if (nw > wid)
				wid = nw;
		}
		/*
		 * If bflag is only set twice, set it depending on
		 * number of files [this is undocumented, and subject to change]
		 */
		if (bflag == 2) {
			bflag = optind >= argc - 1;
		}
		for (; optind < argc; optind++)
			process(argv[optind], wid);
	}

	c = magic->haderr ? 1 : 0;
	magic_close(magic);
	return c;
}


private void
/*ARGSUSED*/
load(const char *m, int flags)
{
	if (magic || m == NULL)
		return;
	magic = magic_open(flags);
	if (magic == NULL) {
		(void)fprintf(stderr, "%s: %s\n", progname, strerror(errno));
		exit(1);
	}
	if (magic_load(magic, magicfile) == -1) {
		(void)fprintf(stderr, "%s: %s\n",
		    progname, magic_error(magic));
		exit(1);
	}
}

/*
 * unwrap -- read a file of filenames, do each one.
 */
private void
unwrap(char *fn)
{
	char buf[MAXPATHLEN];
	FILE *f;
	int wid = 0, cwid;

	if (strcmp("-", fn) == 0) {
		f = stdin;
		wid = 1;
	} else {
		if ((f = fopen(fn, "r")) == NULL) {
			(void)fprintf(stderr, "%s: Cannot open `%s' (%s).\n",
			    progname, fn, strerror(errno));
			exit(1);
		}

		while (fgets(buf, MAXPATHLEN, f) != NULL) {
			buf[strcspn(buf, "\n")] = '\0';
			cwid = file_mbswidth(buf);
			if (cwid > wid)
				wid = cwid;
		}

		rewind(f);
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		buf[strcspn(buf, "\n")] = '\0';
		process(buf, wid);
		if(nobuffer)
			(void)fflush(stdout);
	}

	(void)fclose(f);
}

/*
 * Called for each input file on the command line (or in a list of files)
 */
private void
process(const char *inname, int wid)
{
	const char *type;
	int std_in = strcmp(inname, "-") == 0;

	if (wid > 0 && !bflag) {
		(void)printf("%s", std_in ? "/dev/stdin" : inname);
		if (nulsep)
			(void)putc('\0', stdout);
		else
			(void)printf("%s", separator);
		(void)printf("%*s ",
		    (int) (nopad ? 0 : (wid - file_mbswidth(inname))), "");
	}

	type = magic_file(magic, std_in ? NULL : inname);
	if (type == NULL)
		(void)printf("ERROR: %s\n", magic_error(magic));
	else
		(void)printf("%s\n", type);
}

size_t
file_mbswidth(const char *s)
{
#if defined(HAVE_WCHAR_H) && defined(HAVE_MBRTOWC) && defined(HAVE_WCWIDTH)
	size_t bytesconsumed, old_n, n, width = 0;
	mbstate_t state;
	wchar_t nextchar;
	(void)memset(&state, 0, sizeof(mbstate_t));
	old_n = n = strlen(s);

	while (n > 0) {
		bytesconsumed = mbrtowc(&nextchar, s, n, &state);
		if (bytesconsumed == (size_t)(-1) ||
		    bytesconsumed == (size_t)(-2)) {
			/* Something went wrong, return something reasonable */
			return old_n;
		}
		if (s[0] == '\n') {
			/*
			 * do what strlen() would do, so that caller
			 * is always right
			 */
			width++;
		} else
			width += wcwidth(nextchar);

		s += bytesconsumed, n -= bytesconsumed;
	}
	return width;
#else
	return strlen(s);
#endif
}

private void
usage(void)
{
	(void)fprintf(stderr, USAGE, progname, progname);
	(void)fputs("Try `file --help' for more information.\n", stderr);
	exit(1);
}

private void
help(void)
{
	(void)fputs(
"Usage: file [OPTION...] [FILE...]\n"
"Determine type of FILEs.\n"
"\n", stderr);
#define OPT(shortname, longname, opt, doc)      \
        fprintf(stderr, "  -%c, --" longname doc, shortname);
#define OPT_LONGONLY(longname, opt, doc)        \
        fprintf(stderr, "      --" longname doc);
#include "file_opts.h"
#undef OPT
#undef OPT_LONGONLY
	exit(0);
}
