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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>	/* for MAXPATHLEN */
#include <sys/stat.h>
#ifdef QUICK
#include <sys/mman.h>
#endif
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

#include <netinet/in.h>		/* for byte swapping */

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

#ifdef __EMX__
private char *apptypeName = NULL;
protected int file_os2_apptype(struct magic_set *ms, const char *fn,
    const void *buf, size_t nb);
#endif /* __EMX__ */

private void free_mlist(struct mlist *);
private void close_and_restore(const struct magic_set *, const char *, int,
    const struct stat *);
private int info_from_stat(struct magic_set *, mode_t);
#ifndef COMPILE_ONLY
private const char *file_or_fd(struct magic_set *, const char *, int);
#endif

#ifndef	STDIN_FILENO
#define	STDIN_FILENO	0
#endif

public struct magic_set *
magic_open(int flags)
{
	struct magic_set *ms;

	if ((ms = calloc((size_t)1, sizeof(struct magic_set))) == NULL)
		return NULL;

	if (magic_setflags(ms, flags) == -1) {
		errno = EINVAL;
		goto free;
	}

	ms->o.buf = ms->o.pbuf = NULL;

	ms->c.li = malloc((ms->c.len = 10) * sizeof(*ms->c.li));
	if (ms->c.li == NULL)
		goto free;
	
	ms->haderr = 0;
	ms->error = -1;
	ms->mlist = NULL;
	ms->file = "unknown";
	ms->line = 0;
	return ms;
free:
	free(ms);
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
		free(ml);
		ml = next;
	}
	free(ml);
}

private int
info_from_stat(struct magic_set *ms, mode_t md)
{
	/* We cannot open it, but we were able to stat it. */
	if (md & 0222)
		if (file_printf(ms, "writable, ") == -1)
			return -1;
	if (md & 0111)
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
	free_mlist(ms->mlist);
	free(ms->o.pbuf);
	free(ms->o.buf);
	free(ms->c.li);
	free(ms);
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

public int
magic_check(struct magic_set *ms, const char *magicfile)
{
	struct mlist *ml = file_apprentice(ms, magicfile, FILE_CHECK);
	free_mlist(ml);
	return ml ? 0 : -1;
}

private void
close_and_restore(const struct magic_set *ms, const char *name, int fd,
    const struct stat *sb)
{
	if (fd == STDIN_FILENO)
		return;
	(void) close(fd);

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

#ifndef COMPILE_ONLY

/*
 * find type of descriptor
 */
public const char *
magic_descriptor(struct magic_set *ms, int fd)
{
	return file_or_fd(ms, NULL, fd);
}

/*
 * find type of named file
 */
public const char *
magic_file(struct magic_set *ms, const char *inname)
{
	return file_or_fd(ms, inname, STDIN_FILENO);
}

private const char *
file_or_fd(struct magic_set *ms, const char *inname, int fd)
{
	int	rv = -1;
	unsigned char *buf;
	struct stat	sb;
	ssize_t nbytes = 0;	/* number of bytes read from a datafile */
	int	ispipe = 0;

	/*
	 * one extra for terminating '\0', and
	 * some overlapping space for matches near EOF
	 */
#define SLOP (1 + sizeof(union VALUETYPE))
	if ((buf = malloc(HOWMANY + SLOP)) == NULL)
		return NULL;

	if (file_reset(ms) == -1)
		goto done;

	switch (file_fsmagic(ms, inname, &sb)) {
	case -1:		/* error */
		goto done;
	case 0:			/* nothing found */
		break;
	default:		/* matched it and printed type */
		rv = 0;
		goto done;
	}

	if (inname == NULL) {
		if (fstat(fd, &sb) == 0 && S_ISFIFO(sb.st_mode))
			ispipe = 1;
	} else {
		int flags = O_RDONLY|O_BINARY;

		if (stat(inname, &sb) == 0 && S_ISFIFO(sb.st_mode)) {
			flags |= O_NONBLOCK;
			ispipe = 1;
		}

		errno = 0;
		if ((fd = open(inname, flags)) < 0) {
#ifdef __CYGWIN__
			/* FIXME: Do this with EXEEXT from autotools */
			char *tmp = alloca(strlen(inname) + 5);
			(void)strcat(strcpy(tmp, inname), ".exe");
			if ((fd = open(tmp, flags)) < 0) {
#endif
				fprintf(stderr, "couldn't open file\n");
				if (info_from_stat(ms, sb.st_mode) == -1)
					goto done;
				rv = 0;
				goto done;
#ifdef __CYGWIN__
			}
#endif
		}
#ifdef O_NONBLOCK
		if ((flags = fcntl(fd, F_GETFL)) != -1) {
			flags &= ~O_NONBLOCK;
			(void)fcntl(fd, F_SETFL, flags);
		}
#endif
	}

	/*
	 * try looking at the first HOWMANY bytes
	 */
	if (ispipe) {
		ssize_t r = 0;

		while ((r = sread(fd, (void *)&buf[nbytes],
		    (size_t)(HOWMANY - nbytes), 1)) > 0) {
			nbytes += r;
			if (r < PIPE_BUF) break;
		}

		if (nbytes == 0) {
			/* We can not read it, but we were able to stat it. */
			if (info_from_stat(ms, sb.st_mode) == -1)
				goto done;
			rv = 0;
			goto done;
		}

	} else {
		if ((nbytes = read(fd, (char *)buf, HOWMANY)) == -1) {
			file_error(ms, errno, "cannot read `%s'", inname);
			goto done;
		}
	}

	(void)memset(buf + nbytes, 0, SLOP); /* NUL terminate */
	if (file_buffer(ms, fd, inname, buf, (size_t)nbytes) == -1)
		goto done;
	rv = 0;
done:
	free(buf);
	close_and_restore(ms, inname, fd, &sb);
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
	if (file_buffer(ms, -1, NULL, buf, nb) == -1) {
		return NULL;
	}
	return file_getbuffer(ms);
}
#endif

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
