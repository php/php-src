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

#ifndef	lint
FILE_RCSID("@(#)$File: magic.c,v 1.102 2017/08/28 13:39:18 christos Exp $")
#endif	/* lint */

#include "magic.h"

#include <stdlib.h>
#ifdef PHP_WIN32
#include "win32/unistd.h"
#else
#include <unistd.h>
#endif
#include <string.h>
#include "config.h"

#ifdef PHP_WIN32
#include <shlwapi.h>
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

private void close_and_restore(const struct magic_set *, const char *, int,
    const zend_stat_t *);
private int unreadable_info(struct magic_set *, mode_t, const char *);
#if 0
private const char* get_default_magic(void);
#endif
private const char *file_or_stream(struct magic_set *, const char *, php_stream *);

#ifndef	STDIN_FILENO
#define	STDIN_FILENO	0
#endif

public struct magic_set *
magic_open(int flags)
{
	return file_ms_alloc(flags);
}

private int
unreadable_info(struct magic_set *ms, mode_t md, const char *file)
{
	if (file) {
		/* We cannot open it, but we were able to stat it. */
		if (access(file, W_OK) == 0)
			if (file_printf(ms, "writable, ") == -1)
				return -1;
		if (access(file, X_OK) == 0)
			if (file_printf(ms, "executable, ") == -1)
				return -1;
	}
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
	if (ms == NULL)
		return;
	file_ms_free(ms);
}

/*
 * load a magic file
 */
public int
magic_load(struct magic_set *ms, const char *magicfile)
{
	if (ms == NULL)
		return -1;
	return file_apprentice(ms, magicfile, FILE_LOAD);
}

public int
magic_compile(struct magic_set *ms, const char *magicfile)
{
	if (ms == NULL)
		return -1;
	return file_apprentice(ms, magicfile, FILE_COMPILE);
}

public int
magic_check(struct magic_set *ms, const char *magicfile)
{
	if (ms == NULL)
		return -1;
	return file_apprentice(ms, magicfile, FILE_CHECK);
}

public int
magic_list(struct magic_set *ms, const char *magicfile)
{
	if (ms == NULL)
		return -1;
	return file_apprentice(ms, magicfile, FILE_LIST);
}

private void
close_and_restore(const struct magic_set *ms, const char *name, int fd,
    const zend_stat_t *sb)
{
	if (fd == STDIN_FILENO || name == NULL)
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

		(void)memset(&utbuf, 0, sizeof(utbuf));
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
	if (ms == NULL)
		return NULL;
	return file_or_stream(ms, NULL, NULL);
}

/*
 * find type of named file
 */
public const char *
magic_file(struct magic_set *ms, const char *inname)
{
	if (ms == NULL)
		return NULL;
	return file_or_stream(ms, inname, NULL);
}

public const char *
magic_stream(struct magic_set *ms, php_stream *stream)
{
	if (ms == NULL)
		return NULL;
	return file_or_stream(ms, NULL, stream);
}

private const char *
file_or_stream(struct magic_set *ms, const char *inname, php_stream *stream)
{
	int	rv = -1;
	unsigned char *buf;
	zend_stat_t   sb;
	ssize_t nbytes = 0;	/* number of bytes read from a datafile */
	int no_in_stream = 0;

	if (file_reset(ms, 1) == -1)
		goto out;

	if (!inname && !stream) {
		return NULL;
	}

	/*
	 * one extra for terminating '\0', and
	 * some overlapping space for matches near EOF
	 */
#define SLOP (1 + sizeof(union VALUETYPE))
	if ((buf = CAST(unsigned char *, emalloc(ms->bytes_max + SLOP))) == NULL)
		return NULL;

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
		stream = php_stream_open_wrapper((char *)inname, "rb", REPORT_ERRORS, NULL);
	}

	if (!stream) {
		if (unreadable_info(ms, sb.st_mode, inname) == -1)
			goto done;
		rv = 0;
		goto done;
	}

#ifdef O_NONBLOCK
/* we should be already be in non blocking mode for network socket */
#endif

	/*
	 * try looking at the first ms->bytes_max bytes
	 */
	if ((nbytes = php_stream_read(stream, (char *)buf, ms->bytes_max - nbytes)) < 0) {
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
out:
	return rv == 0 ? file_getbuffer(ms) : NULL;
}


public const char *
magic_buffer(struct magic_set *ms, const void *buf, size_t nb)
{
	if (ms == NULL)
		return NULL;
	if (file_reset(ms, 1) == -1)
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
	if (ms == NULL)
		return "Magic database is not open";
	return (ms->event_flags & EVENT_HAD_ERR) ? ms->o.buf : NULL;
}

public int
magic_errno(struct magic_set *ms)
{
	if (ms == NULL)
		return EINVAL;
	return (ms->event_flags & EVENT_HAD_ERR) ? ms->error : 0;
}

public int
magic_getflags(struct magic_set *ms)
{
	if (ms == NULL)
		return -1;

	return ms->flags;
}

public int
magic_setflags(struct magic_set *ms, int flags)
{
	if (ms == NULL)
		return -1;
#if !defined(HAVE_UTIME) && !defined(HAVE_UTIMES)
	if (flags & MAGIC_PRESERVE_ATIME)
		return -1;
#endif
	ms->flags = flags;
	return 0;
}

public int
magic_version(void)
{
	return MAGIC_VERSION;
}

public int
magic_setparam(struct magic_set *ms, int param, const void *val)
{
	switch (param) {
	case MAGIC_PARAM_INDIR_MAX:
		ms->indir_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_NAME_MAX:
		ms->name_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_ELF_PHNUM_MAX:
		ms->elf_phnum_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_ELF_SHNUM_MAX:
		ms->elf_shnum_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_ELF_NOTES_MAX:
		ms->elf_notes_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_REGEX_MAX:
		ms->elf_notes_max = (uint16_t)*(const size_t *)val;
		return 0;
	case MAGIC_PARAM_BYTES_MAX:
		ms->bytes_max = *(const size_t *)val;
		return 0;
	default:
		errno = EINVAL;
		return -1;
	}
}

public int
magic_getparam(struct magic_set *ms, int param, void *val)
{
	switch (param) {
	case MAGIC_PARAM_INDIR_MAX:
		*(size_t *)val = ms->indir_max;
		return 0;
	case MAGIC_PARAM_NAME_MAX:
		*(size_t *)val = ms->name_max;
		return 0;
	case MAGIC_PARAM_ELF_PHNUM_MAX:
		*(size_t *)val = ms->elf_phnum_max;
		return 0;
	case MAGIC_PARAM_ELF_SHNUM_MAX:
		*(size_t *)val = ms->elf_shnum_max;
		return 0;
	case MAGIC_PARAM_ELF_NOTES_MAX:
		*(size_t *)val = ms->elf_notes_max;
		return 0;
	case MAGIC_PARAM_REGEX_MAX:
		*(size_t *)val = ms->regex_max;
		return 0;
	case MAGIC_PARAM_BYTES_MAX:
		*(size_t *)val = ms->bytes_max;
		return 0;
	default:
		errno = EINVAL;
		return -1;
	}
}
