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
#include "magic.h"
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <sys/stat.h>
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

#ifndef	lint
FILE_RCSID("@(#)$File: fsmagic.c,v 1.50 2008/02/12 17:22:54 rrt Exp $")
#endif	/* lint */

private int
bad_link(struct magic_set *ms, int err, char *buf)
{
	char *errfmt;
	if (err == ELOOP)
		errfmt = "symbolic link in a loop";
	else
		errfmt = "broken symbolic link to `%s'";
	if (ms->flags & MAGIC_ERROR) {
		file_error(ms, err, errfmt, buf);
		return -1;
	} 
	if (file_printf(ms, errfmt, buf) == -1)
		return -1;
	return 1;
}

protected int
file_fsmagic(struct magic_set *ms, const char *fn, struct stat *sb)
{
	int ret = 0;
	int mime = ms->flags & MAGIC_MIME;
#ifdef	S_IFLNK
	char buf[BUFSIZ+4];
	int nch;
	struct stat tstatbuf;
#endif

	if (fn == NULL)
		return 0;

	/*
	 * Fstat is cheaper but fails for files you don't have read perms on.
	 * On 4.2BSD and similar systems, use lstat() to identify symlinks.
	 */
#ifdef	S_IFLNK
	if ((ms->flags & MAGIC_SYMLINK) == 0)
		ret = lstat(fn, sb);
	else
#endif
	ret = stat(fn, sb);	/* don't merge into if; see "ret =" above */

	if (ret) {
		if (ms->flags & MAGIC_ERROR) {
			file_error(ms, errno, "cannot stat `%s'", fn);
			return -1;
		}
		if (file_printf(ms, "cannot open `%s' (%s)",
		    fn, strerror(errno)) == -1)
			return -1;
		return 1;
	}

	if (mime) {
		if ((sb->st_mode & S_IFMT) != S_IFREG) {
			if ((mime & MAGIC_MIME_TYPE) &&
			    file_printf(ms, "application/x-not-regular-file")
			    == -1)
				    return -1;
			return 1;
		}
	}
	else {
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
	case S_IFDIR:
		if (file_printf(ms, "directory") == -1)
			return -1;
		return 1;
#ifdef S_IFCHR
	case S_IFCHR:
		/* 
		 * If -s has been specified, treat character special files
		 * like ordinary files.  Otherwise, just report that they
		 * are block special files and go on to the next file.
		 */
		if ((ms->flags & MAGIC_DEVICES) != 0)
			break;
#ifdef HAVE_STAT_ST_RDEV
# ifdef dv_unit
		if (file_printf(ms, "character special (%d/%d/%d)",
		    major(sb->st_rdev), dv_unit(sb->st_rdev),
		    dv_subunit(sb->st_rdev)) == -1)
			return -1;
# else
		if (file_printf(ms, "character special (%ld/%ld)",
		    (long) major(sb->st_rdev), (long) minor(sb->st_rdev)) == -1)
			return -1;
# endif
#else
		if (file_printf(ms, "character special") == -1)
			return -1;
#endif
		return 1;
#endif
#ifdef S_IFBLK
	case S_IFBLK:
		/* 
		 * If -s has been specified, treat block special files
		 * like ordinary files.  Otherwise, just report that they
		 * are block special files and go on to the next file.
		 */
		if ((ms->flags & MAGIC_DEVICES) != 0)
			break;
#ifdef HAVE_STAT_ST_RDEV
# ifdef dv_unit
		if (file_printf(ms, "block special (%d/%d/%d)",
		    major(sb->st_rdev), dv_unit(sb->st_rdev),
		    dv_subunit(sb->st_rdev)) == -1)
			return -1;
# else
		if (file_printf(ms, "block special (%ld/%ld)",
		    (long)major(sb->st_rdev), (long)minor(sb->st_rdev)) == -1)
			return -1;
# endif
#else
		if (file_printf(ms, "block special") == -1)
			return -1;
#endif
		return 1;
#endif
	/* TODO add code to handle V7 MUX and Blit MUX files */
#ifdef	S_IFIFO
	case S_IFIFO:
		if((ms->flags & MAGIC_DEVICES) != 0)
			break;
		if (file_printf(ms, "fifo (named pipe)") == -1)
			return -1;
		return 1;
#endif
#ifdef	S_IFDOOR
	case S_IFDOOR:
		if (file_printf(ms, "door") == -1)
			return -1;
		return 1;
#endif
#ifdef	S_IFLNK
	case S_IFLNK:
		if ((nch = readlink(fn, buf, BUFSIZ-1)) <= 0) {
			if (ms->flags & MAGIC_ERROR) {
			    file_error(ms, errno, "unreadable symlink `%s'",
				fn);
			    return -1;
			}
			if (file_printf(ms,
			    "unreadable symlink `%s' (%s)", fn,
			    strerror(errno)) == -1)
				return -1;
			return 1;
		}
		buf[nch] = '\0';	/* readlink(2) does not do this */

		/* If broken symlink, say so and quit early. */
		if (*buf == '/') {
			if (stat(buf, &tstatbuf) < 0)
				return bad_link(ms, errno, buf);
		} else {
			char *tmp;
			char buf2[BUFSIZ+BUFSIZ+4];

			if ((tmp = strrchr(fn,  '/')) == NULL) {
				tmp = buf; /* in current directory anyway */
			} else {
				if (tmp - fn + 1 > BUFSIZ) {
					if (ms->flags & MAGIC_ERROR) {
						file_error(ms, 0, 
						    "path too long: `%s'", buf);
						return -1;
					}
					if (file_printf(ms,
					    "path too long: `%s'", fn) == -1)
						return -1;
					return 1;
				}
				(void)strcpy(buf2, fn);  /* take dir part */
				buf2[tmp - fn + 1] = '\0';
				(void)strcat(buf2, buf); /* plus (rel) link */
				tmp = buf2;
			}
			if (stat(tmp, &tstatbuf) < 0)
				return bad_link(ms, errno, buf);
		}

		/* Otherwise, handle it. */
		if ((ms->flags & MAGIC_SYMLINK) != 0) {
			const char *p;
			ms->flags &= MAGIC_SYMLINK;
			p = magic_file(ms, buf);
			ms->flags |= MAGIC_SYMLINK;
			return p != NULL ? 1 : -1;
		} else { /* just print what it points to */
			if (file_printf(ms, "symbolic link to `%s'",
			    buf) == -1)
				return -1;
		}
	return 1;
#endif
#ifdef	S_IFSOCK
#ifndef __COHERENT__
	case S_IFSOCK:
		if (file_printf(ms, "socket") == -1)
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
		if ((!mime || (mime & MAGIC_MIME_TYPE)) &&
		    file_printf(ms, mime ? "application/x-empty" :
		    "empty") == -1)
			return -1;
		return 1;
	}
	return 0;
}
