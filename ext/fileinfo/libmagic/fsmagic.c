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
 * fsmagic - magic based on filesystem info - directory, special files, etc.
 */

#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: fsmagic.c,v 1.59 2009/02/03 20:27:51 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
/* Since major is a function on SVR4, we cannot use `ifndef major'.  */
#ifdef MAJOR_IN_MKDEV
# include <sys/mkdev.h>
# define HAVE_MAJOR
#endif
#ifdef MAJOR_IN_SYSMACROS
# include <sys/sysmacros.h>
# define HAVE_MAJOR
#endif
#ifdef major			/* Might be defined in sys/types.h.  */
# define HAVE_MAJOR
#endif
  
#ifndef HAVE_MAJOR
# define major(dev)  (((dev) >> 8) & 0xff)
# define minor(dev)  ((dev) & 0xff)
#endif
#undef HAVE_MAJOR

#ifdef PHP_WIN32

# undef S_IFIFO
#endif


#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode) ((mode) & _S_IFREG)
#endif

private int
handle_mime(struct magic_set *ms, int mime, const char *str)
{
	if ((mime & MAGIC_MIME_TYPE)) {
		if (file_printf(ms, "application/%s", str) == -1)
			return -1;
		if ((mime & MAGIC_MIME_ENCODING) && file_printf(ms,
		    "; charset=") == -1)
			return -1;
	}
	if ((mime & MAGIC_MIME_ENCODING) && file_printf(ms, "binary") == -1)
		return -1;
	return 0;
}

protected int
file_fsmagic(struct magic_set *ms, const char *fn, struct stat *sb, php_stream *stream)
{
	int mime = ms->flags & MAGIC_MIME;
	TSRMLS_FETCH();

	if (ms->flags & MAGIC_APPLE)
		return 0;

	if (!fn && !stream) {
		return 0;
	}

	if (stream) {
		php_stream_statbuf ssb;
		if (php_stream_stat(stream, &ssb) < 0) {
			if (ms->flags & MAGIC_ERROR) {
				file_error(ms, errno, "cannot stat `%s'", fn);
				return -1;
			}
			return 1;
		}
		memcpy(sb, &ssb.sb, sizeof(struct stat));
	} else {
		if (php_sys_stat(fn, sb) != 0) {
			if (ms->flags & MAGIC_ERROR) {
				file_error(ms, errno, "cannot stat `%s'", fn);
				return -1;
			}
			return 1;
		}
	}

	if (!mime) {
#ifdef S_ISUID
		if (sb->st_mode & S_ISUID) 
			if (file_printf(ms, "setuid ") == -1)
				return -1;
#endif
#ifdef S_ISGID
		if (sb->st_mode & S_ISGID) 
			if (file_printf(ms, "setgid ") == -1)
				return -1;
#endif
#ifdef S_ISVTX
		if (sb->st_mode & S_ISVTX) 
			if (file_printf(ms, "sticky ") == -1)
				return -1;
#endif
	}

	switch (sb->st_mode & S_IFMT) {
#ifndef PHP_WIN32
# ifdef S_IFCHR
		case S_IFCHR:
			/* 
			 * If -s has been specified, treat character special files
			 * like ordinary files.  Otherwise, just report that they
			 * are block special files and go on to the next file.
			 */
			if ((ms->flags & MAGIC_DEVICES) != 0) {
				break;
			}
			if (mime) {
				if (handle_mime(ms, mime, "x-character-device") == -1)
					return -1;
			} else {
#  ifdef HAVE_STAT_ST_RDEV
#   ifdef dv_unit
				if (file_printf(ms, "character special (%d/%d/%d)",
				    major(sb->st_rdev), dv_unit(sb->st_rdev),
						dv_subunit(sb->st_rdev)) == -1)
					return -1;
#   else
				if (file_printf(ms, "character special (%ld/%ld)",
				    (long)major(sb->st_rdev), (long)minor(sb->st_rdev))
				    == -1)
					return -1;
#   endif
#  else
				if (file_printf(ms, "character special") == -1)
					return -1;
#  endif
			}
			return 1;
# endif
#endif

#ifdef	S_IFIFO
			case S_IFIFO:
				if((ms->flags & MAGIC_DEVICES) != 0)
					break;
				if (mime) {
					if (handle_mime(ms, mime, "x-fifo") == -1)
						return -1;
				} else if (file_printf(ms, "fifo (named pipe)") == -1)
					return -1;
				return 1;
#endif
#ifdef	S_IFDOOR
				case S_IFDOOR:
					if (mime) {
						if (handle_mime(ms, mime, "x-door") == -1)
							return -1;
					} else if (file_printf(ms, "door") == -1)
						return -1;
					return 1;
#endif

#ifdef	S_IFLNK
	case S_IFLNK:
		/* stat is used, if it made here then the link is broken */
			if (ms->flags & MAGIC_ERROR) {
			    file_error(ms, errno, "unreadable symlink `%s'", fn);
			    return -1;
			}
	return 1;
#endif

#ifdef	S_IFSOCK
#ifndef __COHERENT__
	case S_IFSOCK:
		if (mime) {
			if (handle_mime(ms, mime, "x-socket") == -1)
				return -1;
		} else if (file_printf(ms, "socket") == -1)
			return -1;
		return 1;
#endif
#endif

		case S_IFREG:
			break;

		default:
			file_error(ms, 0, "invalid mode 0%o", sb->st_mode);
			return -1;
			/*NOTREACHED*/
	}

	/*
	 * regular file, check next possibility
	 *
	 * If stat() tells us the file has zero length, report here that
	 * the file is empty, so we can skip all the work of opening and 
	 * reading the file.
	 * But if the -s option has been given, we skip this optimization,
	 * since on some systems, stat() reports zero size for raw disk
	 * partitions.  (If the block special device really has zero length,
	 * the fact that it is empty will be detected and reported correctly
	 * when we read the file.)
	 */
	if ((ms->flags & MAGIC_DEVICES) == 0 && sb->st_size == 0) {
		if (mime) {
			if (handle_mime(ms, mime, "x-empty") == -1)
				return -1;
		} else if (file_printf(ms, "empty") == -1)
			return -1;
		return 1;
	}
	return 0;
}
