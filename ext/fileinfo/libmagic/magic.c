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
#include "magic.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef PHP_WIN32
#include "win32/unistd.h"
#else
#include <unistd.h>
#endif
#include <string.h>
#include <sys/types.h>
#ifdef PHP_WIN32
# include "config.w32.h"
#else
# include "php_config.h"
#endif

#include <sys/stat.h>
#include <limits.h>	/* for PIPE_BUF */

#if defined(HAVE_UTIMES)
# include <sys/time.h>
#elif defined(HAVE_UTIME)
# if defined(HAVE_SYS_UTIME_H)
#  include <sys/utime.h>
# elif defined(HAVE_UTIME_H)
#  include <utime.h>
# endif
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>	/* for read() */
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifndef PHP_WIN32
# include <netinet/in.h>		/* for byte swapping */
#endif
#include "patchlevel.h"

#ifndef	lint
FILE_RCSID("@(#)$File: magic.c,v 1.50 2008/02/19 00:58:59 rrt Exp $")
#endif	/* lint */

#ifndef PIPE_BUF 
/* Get the PIPE_BUF from pathconf */
#ifdef _PC_PIPE_BUF
#define PIPE_BUF pathconf(".", _PC_PIPE_BUF)
#else
#define PIPE_BUF 512
#endif
#endif

#ifdef PHP_WIN32
# undef S_IFLNK
# undef S_IFIFO
#endif

#ifdef __EMX__
private char *apptypeName = NULL;
protected int file_os2_apptype(struct magic_set *ms, const char *fn,
    const void *buf, size_t nb);
#endif /* __EMX__ */

private void free_mlist(struct mlist *);
private void close_and_restore(const struct magic_set *, const char *, int,
    const struct stat *);
private int unreadable_info(struct magic_set *, mode_t, const char *);
private const char *file_or_stream(struct magic_set *, const char *, php_stream *);

#ifndef	STDIN_FILENO
#define	STDIN_FILENO	0
#endif

public struct magic_set *
magic_open(int flags)
{
	struct magic_set *ms;

	ms = ecalloc((size_t)1, sizeof(struct magic_set));

	if (magic_setflags(ms, flags) == -1) {
		errno = EINVAL;
		goto free;
	}

	ms->o.buf = ms->o.pbuf = NULL;

	ms->c.li = emalloc((ms->c.len = 10) * sizeof(*ms->c.li));
	
	ms->haderr = 0;
	ms->error = -1;
	ms->mlist = NULL;
	ms->file = "unknown";
	ms->line = 0;
	return ms;
free:
	efree(ms);
	return NULL;
}

private void
free_mlist(struct mlist *mlist)
{
	struct mlist *ml;

	if (mlist == NULL)
		return;

	for (ml = mlist->next; ml != mlist;) {
		struct mlist *next = ml->next;
		struct magic *mg = ml->magic;
		file_delmagic(mg, ml->mapped, ml->nmagic);
		efree(ml);
		ml = next;
	}
	efree(ml);
}

private int
unreadable_info(struct magic_set *ms, mode_t md, const char *file)
{
	/* We cannot open it, but we were able to stat it. */
	if (access(file, W_OK) == 0)
		if (file_printf(ms, "writable, ") == -1)
			return -1;
	if (access(file, X_OK) == 0)
		if (file_printf(ms, "executable, ") == -1)
			return -1;
	if (S_ISREG(md))
		if (file_printf(ms, "regular file, ") == -1)
			return -1;
	if (file_printf(ms, "no read permission") == -1)
		return -1;
	return 0;
}

public void
magic_close(struct magic_set *ms)
{
	if (ms->mlist) {
		free_mlist(ms->mlist);
	}
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

/*
 * load a magic file
 */
public int
magic_load(struct magic_set *ms, const char *magicfile)
{
	struct mlist *ml = file_apprentice(ms, magicfile, FILE_LOAD);
	if (ml) {
		free_mlist(ms->mlist);
		ms->mlist = ml;
		return 0;
	}
	return -1;
}

public int
magic_compile(struct magic_set *ms, const char *magicfile)
{
	struct mlist *ml = file_apprentice(ms, magicfile, FILE_COMPILE);
	free_mlist(ml);
	return ml ? 0 : -1;
}

private void
close_and_restore(const struct magic_set *ms, const char *name, int fd,
    const struct stat *sb)
{
	if ((ms->flags & MAGIC_PRESERVE_ATIME) != 0) {
		/*
		 * Try to restore access, modification times if read it.
		 * This is really *bad* because it will modify the status
		 * time of the file... And of course this will affect
		 * backup programs
		 */
#ifdef HAVE_UTIMES
		struct timeval  utsbuf[2];
		(void)memset(utsbuf, 0, sizeof(utsbuf));
		utsbuf[0].tv_sec = sb->st_atime;
		utsbuf[1].tv_sec = sb->st_mtime;

		(void) utimes(name, utsbuf); /* don't care if loses */
#elif defined(HAVE_UTIME_H) || defined(HAVE_SYS_UTIME_H)
		struct utimbuf  utbuf;

		(void)memset(utbuf, 0, sizeof(utbuf));
		utbuf.actime = sb->st_atime;
		utbuf.modtime = sb->st_mtime;
		(void) utime(name, &utbuf); /* don't care if loses */
#endif
	}
}


/*
 * find type of descriptor
 */
public const char *
magic_descriptor(struct magic_set *ms, int fd)
{
	return file_or_stream(ms, NULL, NULL);
}

/*
 * find type of named file
 */
public const char *
magic_file(struct magic_set *ms, const char *inname)
{
	return file_or_stream(ms, inname, NULL);
}

public const char *
magic_stream(struct magic_set *ms, php_stream *stream)
{
	return file_or_stream(ms, NULL, stream);
}

private const char *
file_or_stream(struct magic_set *ms, const char *inname, php_stream *stream)
{
	int	rv = -1;
	unsigned char *buf;
	struct stat	sb;
	ssize_t nbytes = 0;	/* number of bytes read from a datafile */
	int no_in_stream = 0;
	TSRMLS_FETCH();

	if (!inname && !stream) {
		return NULL;
	}

	/*
	 * one extra for terminating '\0', and
	 * some overlapping space for matches near EOF
	 */
#define SLOP (1 + sizeof(union VALUETYPE))
	buf = emalloc(HOWMANY + SLOP);

	if (file_reset(ms) == -1) {
		goto done;
	}

	switch (file_fsmagic(ms, inname, &sb, stream)) {
		case -1:		/* error */
			goto done;
		case 0:			/* nothing found */
			break;
		default:		/* matched it and printed type */
			rv = 0;
			goto done;
	}

	errno = 0;

	if (!stream && inname) {
		no_in_stream = 1;
#if (PHP_MAJOR_VERSION < 6)
		stream = php_stream_open_wrapper(inname, "rb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);
#else
		stream = php_stream_open_wrapper(inname, "rb", REPORT_ERRORS, NULL);
#endif
	}

	if (!stream) {
				if (unreadable_info(ms, sb.st_mode,
#ifdef __CYGWIN
						    tmp
#else
						    inname
#endif
						    ) == -1)
			goto done;
		rv = 0;
		goto done;
	}

#ifdef O_NONBLOCK
		/* we should be already be in non blocking mode for network socket */
#endif

	/*
	 * try looking at the first HOWMANY bytes
	 */
	if ((nbytes = php_stream_read(stream, (char *)buf, HOWMANY)) < 0) {
		file_error(ms, errno, "cannot read `%s'", inname);
		goto done;
	}

	(void)memset(buf + nbytes, 0, SLOP); /* NUL terminate */
	if (file_buffer(ms, stream, inname, buf, (size_t)nbytes) == -1)
		goto done;
	rv = 0;
done:
	efree(buf);

	if (no_in_stream && stream) {
		php_stream_close(stream);
	}

	close_and_restore(ms, inname, 0, &sb);
	return rv == 0 ? file_getbuffer(ms) : NULL;
}

public const char *
magic_buffer(struct magic_set *ms, const void *buf, size_t nb)
{
	if (file_reset(ms) == -1)
		return NULL;
	/*
	 * The main work is done here!
	 * We have the file name and/or the data buffer to be identified. 
	 */
	if (file_buffer(ms, NULL, NULL, buf, nb) == -1) {
		return NULL;
	}
	return file_getbuffer(ms);
}

public const char *
magic_error(struct magic_set *ms)
{
	return ms->haderr ? ms->o.buf : NULL;
}

public int
magic_errno(struct magic_set *ms)
{
	return ms->haderr ? ms->error : 0;
}

public int
magic_setflags(struct magic_set *ms, int flags)
{
#if !defined(HAVE_UTIME) && !defined(HAVE_UTIMES)
	if (flags & MAGIC_PRESERVE_ATIME)
		return -1;
#endif
	ms->flags = flags;
	return 0;
}
