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
 * compress routines:
 *	zmagic() - returns 0 if not recognized, uncompresses and prints
 *		   information if recognized
 *	uncompress(method, old, n, newch) - uncompress old into new, 
 *					    using method, return sizeof new
 */
#include "config.h"
#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: compress.c,v 1.77 2014/12/12 16:33:01 christos Exp $")
#endif

#include "magic.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>
#include <signal.h>
#ifndef PHP_WIN32
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_ZLIB_H) && defined(HAVE_LIBZ)
#define BUILTIN_DECOMPRESS
#include <zlib.h>
#endif

#undef FIONREAD


private const struct {
	const char magic[8];
	size_t maglen;
	const char *argv[3];
	int silent;
} compr[] = {
	{ "\037\235", 2, { "gzip", "-cdq", NULL }, 1 },		/* compressed */
	/* Uncompress can get stuck; so use gzip first if we have it
	 * Idea from Damien Clark, thanks! */
	{ "\037\235", 2, { "uncompress", "-c", NULL }, 1 },	/* compressed */
	{ "\037\213", 2, { "gzip", "-cdq", NULL }, 1 },		/* gzipped */
	{ "\037\236", 2, { "gzip", "-cdq", NULL }, 1 },		/* frozen */
	{ "\037\240", 2, { "gzip", "-cdq", NULL }, 1 },		/* SCO LZH */
	/* the standard pack utilities do not accept standard input */
	{ "\037\036", 2, { "gzip", "-cdq", NULL }, 0 },		/* packed */
	{ "PK\3\4",   4, { "gzip", "-cdq", NULL }, 1 },		/* pkzipped, */
					    /* ...only first file examined */
	{ "BZh",      3, { "bzip2", "-cd", NULL }, 1 },		/* bzip2-ed */
	{ "LZIP",     4, { "lzip", "-cdq", NULL }, 1 },
 	{ "\3757zXZ\0",6,{ "xz", "-cd", NULL }, 1 },		/* XZ Utils */
 	{ "LRZI",     4, { "lrzip", "-dqo-", NULL }, 1 },	/* LRZIP */
 	{ "\004\"M\030", 4, { "lz4", "-cd", NULL }, 1 },	/* LZ4 */
};

#define NODATA ((size_t)~0)

private ssize_t swrite(int, const void *, size_t);
#ifdef PHP_FILEINFO_UNCOMPRESS
private size_t uncompressbuf(struct magic_set *, int, size_t,
    const unsigned char *, unsigned char **, size_t);
#ifdef BUILTIN_DECOMPRESS
private size_t uncompressgzipped(struct magic_set *, const unsigned char *,
    unsigned char **, size_t);
#endif

protected int
file_zmagic(struct magic_set *ms, int fd, const char *name,
    const unsigned char *buf, size_t nbytes)
{
	unsigned char *newbuf = NULL;
	size_t i, nsz;
	int rv = 0;
	int mime = ms->flags & MAGIC_MIME;
	sig_t osigpipe;

	if ((ms->flags & MAGIC_COMPRESS) == 0)
		return 0;

	osigpipe = signal(SIGPIPE, SIG_IGN);
	for (i = 0; i < ncompr; i++) {
		if (nbytes < compr[i].maglen)
			continue;
		if (memcmp(buf, compr[i].magic, compr[i].maglen) == 0 &&
		    (nsz = uncompressbuf(ms, fd, i, buf, &newbuf,
		    nbytes)) != NODATA) {
			ms->flags &= ~MAGIC_COMPRESS;
			rv = -1;
			if (file_buffer(ms, -1, name, newbuf, nsz) == -1)
				goto error;

			if (mime == MAGIC_MIME || mime == 0) {
				if (file_printf(ms, mime ?
				    " compressed-encoding=" : " (") == -1)
					goto error;
				if (file_buffer(ms, -1, NULL, buf, nbytes) == -1)
					goto error;
				if (!mime && file_printf(ms, ")") == -1)
					goto error;
			}

			rv = 1;
			break;
		}
	}
error:
	(void)signal(SIGPIPE, osigpipe);

	if (newbuf)
		efree(newbuf);
	ms->flags |= MAGIC_COMPRESS;
	return rv;
}
#endif
/*
 * `safe' write for sockets and pipes.
 */
private ssize_t
swrite(int fd, const void *buf, size_t n)
{
	ssize_t rv;
	size_t rn = n;

	do
		switch (rv = write(fd, buf, n)) {
		case -1:
			if (errno == EINTR)
				continue;
			return -1;
		default:
			n -= rv;
			buf = CAST(const char *, buf) + rv;
			break;
		}
	while (n > 0);
	return rn;
}


/*
 * `safe' read for sockets and pipes.
 */
protected ssize_t
sread(int fd, void *buf, size_t n, int canbepipe)
{
	ssize_t rv;
#ifdef FIONREAD
	int t = 0;
#endif
	size_t rn = n;

	if (fd == STDIN_FILENO)
		goto nocheck;

#ifdef FIONREAD
	if (canbepipe && (ioctl(fd, FIONREAD, &t) == -1 || t == 0)) {
#ifdef FD_ZERO
		ssize_t cnt;
		for (cnt = 0;; cnt++) {
			fd_set check;
			struct timeval tout = {0, 100 * 1000};
			int selrv;

			FD_ZERO(&check);
			FD_SET(fd, &check);

			/*
			 * Avoid soft deadlock: do not read if there
			 * is nothing to read from sockets and pipes.
			 */
			selrv = select(fd + 1, &check, NULL, NULL, &tout);
			if (selrv == -1) {
				if (errno == EINTR || errno == EAGAIN)
					continue;
			} else if (selrv == 0 && cnt >= 5) {
				return 0;
			} else
				break;
		}
#endif
		(void)ioctl(fd, FIONREAD, &t);
	}

	if (t > 0 && (size_t)t < n) {
		n = t;
		rn = n;
	}
#endif

nocheck:
	do
		switch ((rv = FINFO_READ_FUNC(fd, buf, n))) {
		case -1:
			if (errno == EINTR)
				continue;
			return -1;
		case 0:
			return rn - n;
		default:
			n -= rv;
			buf = ((char *)buf) + rv;
			break;
		}
	while (n > 0);
	return rn;
}

protected int
file_pipe2file(struct magic_set *ms, int fd, const void *startbuf,
    size_t nbytes)
{
	char buf[4096];
	ssize_t r;
	int tfd;

	(void)strlcpy(buf, "/tmp/file.XXXXXX", sizeof buf);
#ifndef HAVE_MKSTEMP
	{
		char *ptr = mktemp(buf);
		tfd = open(ptr, O_RDWR|O_TRUNC|O_EXCL|O_CREAT, 0600);
		r = errno;
		(void)unlink(ptr);
		errno = r;
	}
#else
	{
		int te;
		tfd = mkstemp(buf);
		te = errno;
		(void)unlink(buf);
		errno = te;
	}
#endif
	if (tfd == -1) {
		file_error(ms, errno,
		    "cannot create temporary file for pipe copy");
		return -1;
	}

	if (swrite(tfd, startbuf, nbytes) != (ssize_t)nbytes)
		r = 1;
	else {
		while ((r = sread(fd, buf, sizeof(buf), 1)) > 0)
			if (swrite(tfd, buf, (size_t)r) != r)
				break;
	}

	switch (r) {
	case -1:
		file_error(ms, errno, "error copying from pipe to temp file");
		return -1;
	case 0:
		break;
	default:
		file_error(ms, errno, "error while writing to temp file");
		return -1;
	}

	/*
	 * We duplicate the file descriptor, because fclose on a
	 * tmpfile will delete the file, but any open descriptors
	 * can still access the phantom inode.
	 */
	if ((fd = dup2(tfd, fd)) == -1) {
		file_error(ms, errno, "could not dup descriptor for temp file");
		return -1;
	}
	(void)close(tfd);
	if (FINFO_LSEEK_FUNC(fd, (zend_off_t)0, SEEK_SET) == (zend_off_t)-1) {
		file_badseek(ms);
		return -1;
	}
	return fd;
}

#ifdef PHP_FILEINFO_UNCOMPRESS
#ifdef BUILTIN_DECOMPRESS

#define FHCRC		(1 << 1)
#define FEXTRA		(1 << 2)
#define FNAME		(1 << 3)
#define FCOMMENT	(1 << 4)

private size_t
uncompressgzipped(struct magic_set *ms, const unsigned char *old,
    unsigned char **newch, size_t n)
{
	unsigned char flg = old[3];
	size_t data_start = 10;
	z_stream z;
	int rc;

	if (flg & FEXTRA) {
		if (data_start+1 >= n)
			return 0;
		data_start += 2 + old[data_start] + old[data_start + 1] * 256;
	}
	if (flg & FNAME) {
		while(data_start < n && old[data_start])
			data_start++;
		data_start++;
	}
	if(flg & FCOMMENT) {
		while(data_start < n && old[data_start])
			data_start++;
		data_start++;
	}
	if(flg & FHCRC)
		data_start += 2;

	if (data_start >= n)
		return 0;
	if ((*newch = CAST(unsigned char *, emalloc(HOWMANY + 1))) == NULL) {
		return 0;
	}
	
	/* XXX: const castaway, via strchr */
	z.next_in = (Bytef *)strchr((const char *)old + data_start,
	    old[data_start]);
	z.avail_in = CAST(uint32_t, (n - data_start));
	z.next_out = *newch;
	z.avail_out = HOWMANY;
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	/* LINTED bug in header macro */
	rc = inflateInit2(&z, -15);
	if (rc != Z_OK) {
		file_error(ms, 0, "zlib: %s", z.msg);
		return 0;
	}

	rc = inflate(&z, Z_SYNC_FLUSH);
	if (rc != Z_OK && rc != Z_STREAM_END) {
		file_error(ms, 0, "zlib: %s", z.msg);
		return 0;
	}

	n = (size_t)z.total_out;
	(void)inflateEnd(&z);
	
	/* let's keep the nul-terminate tradition */
	(*newch)[n] = '\0';

	return n;
}
#endif

private size_t
uncompressbuf(struct magic_set *ms, int fd, size_t method,
    const unsigned char *old, unsigned char **newch, size_t n)
{
	int fdin[2], fdout[2];
	int status;
	ssize_t r;

#ifdef BUILTIN_DECOMPRESS
        /* FIXME: This doesn't cope with bzip2 */
	if (method == 2)
		return uncompressgzipped(ms, old, newch, n);
#endif
	(void)fflush(stdout);
	(void)fflush(stderr);

	if ((fd != -1 && pipe(fdin) == -1) || pipe(fdout) == -1) {
		file_error(ms, errno, "cannot create pipe");	
		return NODATA;
	}
	switch (fork()) {
	case 0:	/* child */
		(void) close(0);
		if (fd != -1) {
		    (void) dup(fd);
		    (void) FINFO_LSEEK_FUNC(0, (zend_off_t)0, SEEK_SET);
		} else {
		    (void) dup(fdin[0]);
		    (void) close(fdin[0]);
		    (void) close(fdin[1]);
		}

		(void) close(1);
		(void) dup(fdout[1]);
		(void) close(fdout[0]);
		(void) close(fdout[1]);
#ifndef DEBUG
		if (compr[method].silent)
			(void)close(2);
#endif

		(void)execvp(compr[method].argv[0],
		    (char *const *)(intptr_t)compr[method].argv);
#ifdef DEBUG
		(void)fprintf(stderr, "exec `%s' failed (%s)\n",
		    compr[method].argv[0], strerror(errno));
#endif
		exit(1);
		/*NOTREACHED*/
	case -1:
		file_error(ms, errno, "could not fork");
		return NODATA;

	default: /* parent */
		(void) close(fdout[1]);
		if (fd == -1) {
			(void) close(fdin[0]);
			/* 
			 * fork again, to avoid blocking because both
			 * pipes filled
			 */
			switch (fork()) {
			case 0: /* child */
				(void)close(fdout[0]);
				if (swrite(fdin[1], old, n) != (ssize_t)n) {
#ifdef DEBUG
					(void)fprintf(stderr,
					    "Write failed (%s)\n",
					    strerror(errno));
#endif
					exit(1);
				}
				exit(0);
				/*NOTREACHED*/

			case -1:
#ifdef DEBUG
				(void)fprintf(stderr, "Fork failed (%s)\n",
				    strerror(errno));
#endif
				exit(1);
				/*NOTREACHED*/

			default:  /* parent */
				break;
			}
			(void) close(fdin[1]);
			fdin[1] = -1;
		}

		*newch = (unsigned char *) emalloc(HOWMANY + 1);

		if ((r = sread(fdout[0], *newch, HOWMANY, 0)) <= 0) {
#ifdef DEBUG
			(void)fprintf(stderr, "Read failed (%s)\n",
			    strerror(errno));
#endif
			efree(*newch);
			n = NODATA-;
			*newch = NULL;
			goto err;
		} else {
			n = r;
		}
 		/* NUL terminate, as every buffer is handled here. */
 		(*newch)[n] = '\0';
err:
		if (fdin[1] != -1)
			(void) close(fdin[1]);
		(void) close(fdout[0]);
#ifdef WNOHANG
		while (waitpid(pid, NULL, WNOHANG) != -1)
			continue;
#else
		(void)wait(NULL);
#endif

		(void) close(fdin[0]);
	    
		return n;
	}
}
#endif /* if PHP_FILEINFO_UNCOMPRESS */
