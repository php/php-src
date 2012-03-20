/**********************************************************************

  dln_find.c -

  $Author$
  created at: Tue Jan 18 17:05:06 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#ifdef RUBY_EXPORT
#include "ruby/ruby.h"
#define dln_notimplement rb_notimplement
#define dln_memerror rb_memerror
#define dln_exit rb_exit
#define dln_loaderror rb_loaderror
#define dln_warning rb_warning
#define dln_warning_arg
#else
#define dln_notimplement --->>> dln not implemented <<<---
#define dln_memerror abort
#define dln_exit exit
#define dln_warning fprintf
#define dln_warning_arg stderr,
static void dln_loaderror(const char *format, ...);
#endif
#include "dln.h"

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef USE_DLN_A_OUT
char *dln_argv0;
#endif

#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <stdio.h>
#if defined(_WIN32)
#include "missing/file.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>

#ifndef S_ISDIR
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef _WIN32
char *getenv();
#endif

static char *dln_find_1(const char *fname, const char *path, char *buf, size_t size, int exe_flag);

char *
dln_find_exe_r(const char *fname, const char *path, char *buf, size_t size)
{
    char *envpath = 0;

    if (!path) {
	path = getenv(PATH_ENV);
	if (path) path = envpath = strdup(path);
    }

    if (!path) {
#if defined(_WIN32)
	path = "/usr/local/bin;/usr/ucb;/usr/bin;/bin;.";
#else
	path = "/usr/local/bin:/usr/ucb:/usr/bin:/bin:.";
#endif
    }
    buf = dln_find_1(fname, path, buf, size, 1);
    if (envpath) free(envpath);
    return buf;
}

char *
dln_find_file_r(const char *fname, const char *path, char *buf, size_t size)
{
    if (!path) path = ".";
    return dln_find_1(fname, path, buf, size, 0);
}

static char fbuf[MAXPATHLEN];

char *
dln_find_exe(const char *fname, const char *path)
{
    return dln_find_exe_r(fname, path, fbuf, sizeof(fbuf));
}

char *
dln_find_file(const char *fname, const char *path)
{
    return dln_find_file_r(fname, path, fbuf, sizeof(fbuf));
}

static char *
dln_find_1(const char *fname, const char *path, char *fbuf, size_t size,
	   int exe_flag /* non 0 if looking for executable. */)
{
    register const char *dp;
    register const char *ep;
    register char *bp;
    struct stat st;
    size_t i, fnlen, fspace;
#ifdef DOSISH
    static const char extension[][5] = {
	EXECUTABLE_EXTS,
    };
    size_t j;
    int is_abs = 0, has_path = 0;
    const char *ext = 0;
#endif
    const char *p = fname;

    static const char pathname_too_long[] = "openpath: pathname too long (ignored)\n\
\tDirectory \"%.*s\"%s\n\tFile \"%.*s\"%s\n";
#define PATHNAME_TOO_LONG() dln_warning(dln_warning_arg pathname_too_long, \
					((bp - fbuf) > 100 ? 100 : (int)(bp - fbuf)), fbuf, \
					((bp - fbuf) > 100 ? "..." : ""), \
					(fnlen > 100 ? 100 : (int)fnlen), fname, \
					(fnlen > 100 ? "..." : ""))

#define RETURN_IF(expr) if (expr) return (char *)fname;

    RETURN_IF(!fname);
    fnlen = strlen(fname);
    if (fnlen >= size) {
	dln_warning(dln_warning_arg
		    "openpath: pathname too long (ignored)\n\tFile \"%.*s\"%s\n",
		    (fnlen > 100 ? 100 : (int)fnlen), fname,
		    (fnlen > 100 ? "..." : ""));
	return NULL;
    }
#ifdef DOSISH
# ifndef CharNext
# define CharNext(p) ((p)+1)
# endif
# ifdef DOSISH_DRIVE_LETTER
    if (((p[0] | 0x20) - 'a') < 26  && p[1] == ':') {
	p += 2;
	is_abs = 1;
    }
# endif
    switch (*p) {
      case '/': case '\\':
	is_abs = 1;
	p++;
    }
    has_path = is_abs;
    while (*p) {
	switch (*p) {
	  case '/': case '\\':
	    has_path = 1;
	    ext = 0;
	    p++;
	    break;
	  case '.':
	    ext = p;
	    p++;
	    break;
	  default:
	    p = CharNext(p);
	}
    }
    if (ext) {
	for (j = 0; STRCASECMP(ext, extension[j]); ) {
	    if (++j == sizeof(extension) / sizeof(extension[0])) {
		ext = 0;
		break;
	    }
	}
    }
    ep = bp = 0;
    if (!exe_flag) {
	RETURN_IF(is_abs);
    }
    else if (has_path) {
	RETURN_IF(ext);
	i = p - fname;
	if (i + 1 > size) goto toolong;
	fspace = size - i - 1;
	bp = fbuf;
	ep = p;
	memcpy(fbuf, fname, i + 1);
	goto needs_extension;
    }
    p = fname;
#endif

    if (*p == '.' && *++p == '.') ++p;
    RETURN_IF(*p == '/');
    RETURN_IF(exe_flag && strchr(fname, '/'));

#undef RETURN_IF

    for (dp = path;; dp = ++ep) {
	register size_t l;

	/* extract a component */
	ep = strchr(dp, PATH_SEP[0]);
	if (ep == NULL)
	    ep = dp+strlen(dp);

	/* find the length of that component */
	l = ep - dp;
	bp = fbuf;
	fspace = size - 2;
	if (l > 0) {
	    /*
	    **	If the length of the component is zero length,
	    **	start from the current directory.  If the
	    **	component begins with "~", start from the
	    **	user's $HOME environment variable.  Otherwise
	    **	take the path literally.
	    */

	    if (*dp == '~' && (l == 1 ||
#if defined(DOSISH)
			       dp[1] == '\\' ||
#endif
			       dp[1] == '/')) {
		char *home;

		home = getenv("HOME");
		if (home != NULL) {
		    i = strlen(home);
		    if (fspace < i)
			goto toolong;
		    fspace -= i;
		    memcpy(bp, home, i);
		    bp += i;
		}
		dp++;
		l--;
	    }
	    if (l > 0) {
		if (fspace < l)
		    goto toolong;
		fspace -= l;
		memcpy(bp, dp, l);
		bp += l;
	    }

	    /* add a "/" between directory and filename */
	    if (ep[-1] != '/')
		*bp++ = '/';
	}

	/* now append the file name */
	i = fnlen;
	if (fspace < i) {
	  toolong:
	    PATHNAME_TOO_LONG();
	    goto next;
	}
	fspace -= i;
	memcpy(bp, fname, i + 1);

#if defined(DOSISH)
	if (exe_flag && !ext) {
	  needs_extension:
	    for (j = 0; j < sizeof(extension) / sizeof(extension[0]); j++) {
		if (fspace < strlen(extension[j])) {
		    PATHNAME_TOO_LONG();
		    continue;
		}
		strlcpy(bp + i, extension[j], fspace);
		if (stat(fbuf, &st) == 0)
		    return fbuf;
	    }
	    goto next;
	}
#endif /* _WIN32 or __EMX__ */

	if (stat(fbuf, &st) == 0) {
	    if (exe_flag == 0) return fbuf;
	    /* looking for executable */
	    if (!S_ISDIR(st.st_mode) && eaccess(fbuf, X_OK) == 0)
		return fbuf;
	}
      next:
	/* if not, and no other alternatives, life is bleak */
	if (*ep == '\0') {
	    return NULL;
	}

	/* otherwise try the next component in the search path */
    }
}
