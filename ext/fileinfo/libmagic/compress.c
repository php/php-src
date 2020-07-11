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
#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: compress.c,v 1.121 2019/05/07 02:27:11 christos Exp $")
#endif

#include "magic.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>
#include <signal.h>
#ifndef HAVE_SIG_T
typedef void (*sig_t)(int);
#endif /* HAVE_SIG_T */
#ifndef PHP_WIN32
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_ZLIB_H) && defined(PHP_FILEINFO_UNCOMPRESS)
#define BUILTIN_DECOMPRESS
#include <zlib.h>
#endif

#undef FIONREAD

#if defined(PHP_FILEINFO_UNCOMPRESS)
#define BUILTIN_BZLIB
#include <bzlib.h>
#endif

#ifdef DEBUG
int tty = -1;
#define DPRINTF(...)	do { \
	if (tty == -1) \
		tty = open("/dev/tty", O_RDWR); \
	if (tty == -1) \
		abort(); \
	dprintf(tty, __VA_ARGS__); \
} while (/*CONSTCOND*/0)
#else
#define DPRINTF(...)
#endif

#ifdef ZLIBSUPPORT
/*
 * The following python code is not really used because ZLIBSUPPORT is only
 * defined if we have a built-in zlib, and the built-in zlib handles that.
 * That is not true for android where we have zlib.h and not -lz.
 */
static const char zlibcode[] =
    "import sys, zlib; sys.stdout.write(zlib.decompress(sys.stdin.read()))";

static const char *zlib_args[] = { "python", "-c", zlibcode, NULL };

static int
zlibcmp(const unsigned char *buf)
{
	unsigned short x = 1;
	unsigned char *s = CAST(unsigned char *, CAST(void *, &x));

	if ((buf[0] & 0xf) != 8 || (buf[0] & 0x80) != 0)
		return 0;
	if (s[0] != 1)	/* endianness test */
		x = buf[0] | (buf[1] << 8);
	else
		x = buf[1] | (buf[0] << 8);
	if (x % 31)
		return 0;
	return 1;
}
#endif

#define gzip_flags "-cd"
#define lrzip_flags "-do"
#define lzip_flags gzip_flags
#ifdef PHP_FILEINFO_UNCOMPRESS

static const char *gzip_args[] = {
	"gzip", gzip_flags, NULL
};
static const char *uncompress_args[] = {
	"uncompress", "-c", NULL
};
static const char *bzip2_args[] = {
	"bzip2", "-cd", NULL
};
static const char *lzip_args[] = {
	"lzip", lzip_flags, NULL
};
static const char *xz_args[] = {
	"xz", "-cd", NULL
};
static const char *lrzip_args[] = {
	"lrzip", lrzip_flags, NULL
};
static const char *lz4_args[] = {
	"lz4", "-cd", NULL
};
static const char *zstd_args[] = {
	"zstd", "-cd", NULL
};

#define	do_zlib		NULL
#define	do_bzlib	NULL

private const struct {
	const void *magic;
	size_t maglen;
	const char **argv;
	void *unused;
} compr[] = {
	{ "\037\235",	2, gzip_args, NULL },		/* compressed */
	/* Uncompress can get stuck; so use gzip first if we have it
	 * Idea from Damien Clark, thanks! */
	{ "\037\235",	2, uncompress_args, NULL },	/* compressed */
	{ "\037\213",	2, gzip_args, do_zlib },	/* gzipped */
	{ "\037\236",	2, gzip_args, NULL },		/* frozen */
	{ "\037\240",	2, gzip_args, NULL },		/* SCO LZH */
	/* the standard pack utilities do not accept standard input */
	{ "\037\036",	2, gzip_args, NULL },		/* packed */
	{ "PK\3\4",	4, gzip_args, NULL },		/* pkzipped, */
	/* ...only first file examined */
	{ "BZh",	3, bzip2_args, do_bzlib },	/* bzip2-ed */
	{ "LZIP",	4, lzip_args, NULL },		/* lzip-ed */
 	{ "\3757zXZ\0",	6, xz_args, NULL },		/* XZ Utils */
 	{ "LRZI",	4, lrzip_args, NULL },	/* LRZIP */
 	{ "\004\"M\030",4, lz4_args, NULL },		/* LZ4 */
 	{ "\x28\xB5\x2F\xFD", 4, zstd_args, NULL },	/* zstd */
#ifdef ZLIBSUPPORT
	{ RCAST(const void *, zlibcmp),	0, zlib_args, NULL },	/* zlib */
#endif
};

#define OKDATA 	0
#define NODATA	1
#define ERRDATA	2

private ssize_t swrite(int, const void *, size_t);
#if HAVE_FORK
private size_t ncompr = __arraycount(compr);
private int uncompressbuf(int, size_t, size_t, const unsigned char *,
    unsigned char **, size_t *);
#ifdef BUILTIN_DECOMPRESS
private int uncompresszlib(const unsigned char *, unsigned char **, size_t,
    size_t *, int);
private int uncompressgzipped(const unsigned char *, unsigned char **, size_t,
    size_t *);
#endif
#ifdef BUILTIN_BZLIB
private int uncompressbzlib(const unsigned char *, unsigned char **, size_t,
    size_t *, int);
#endif

static int makeerror(unsigned char **, size_t *, const char *, ...);
private const char *methodname(size_t);

private int
format_decompression_error(struct magic_set *ms, size_t i, unsigned char *buf)
{
	unsigned char *p;
	int mime = ms->flags & MAGIC_MIME;

	if (!mime)
		return file_printf(ms, "ERROR:[%s: %s]", methodname(i), buf);

	for (p = buf; *p; p++)
		if (!isalnum(*p))
			*p = '-';

	return file_printf(ms, "application/x-decompression-error-%s-%s",
	    methodname(i), buf);
}

protected int
file_zmagic(struct magic_set *ms, const struct buffer *b, const char *name)
{
	unsigned char *newbuf = NULL;
	size_t i, nsz;
	char *rbuf;
	file_pushbuf_t *pb;
	int urv, prv, rv = 0;
	int mime = ms->flags & MAGIC_MIME;
	int fd = b->fd;
	const unsigned char *buf = CAST(const unsigned char *, b->fbuf);
	size_t nbytes = b->flen;
	int sa_saved = 0;
	struct sigaction sig_act;

	if ((ms->flags & MAGIC_COMPRESS) == 0)
		return 0;

	for (i = 0; i < ncompr; i++) {
		int zm;
		if (nbytes < compr[i].maglen)
			continue;
#ifdef ZLIBSUPPORT
		if (compr[i].maglen == 0)
			zm = (RCAST(int (*)(const unsigned char *),
			    CCAST(void *, compr[i].magic)))(buf);
		else
#endif
			zm = memcmp(buf, compr[i].magic, compr[i].maglen) == 0;

		if (!zm)
			continue;

		/* Prevent SIGPIPE death if child dies unexpectedly */
		if (!sa_saved) {
			//We can use sig_act for both new and old, but
			struct sigaction new_act;
			memset(&new_act, 0, sizeof(new_act));
			new_act.sa_handler = SIG_IGN;
			sa_saved = sigaction(SIGPIPE, &new_act, &sig_act) != -1;
		}

		nsz = nbytes;
		urv = uncompressbuf(fd, ms->bytes_max, i, buf, &newbuf, &nsz);
		DPRINTF("uncompressbuf = %d, %s, %" SIZE_T_FORMAT "u\n", urv,
		    (char *)newbuf, nsz);
		switch (urv) {
		case OKDATA:
		case ERRDATA:
			ms->flags &= ~MAGIC_COMPRESS;
			if (urv == ERRDATA)
				prv = format_decompression_error(ms, i, newbuf);
			else
				prv = file_buffer(ms, NULL, NULL, name, newbuf, nsz);
			if (prv == -1)
				goto error;
			rv = 1;
			if ((ms->flags & MAGIC_COMPRESS_TRANSP) != 0)
				goto out;
			if (mime != MAGIC_MIME && mime != 0)
				goto out;
			if ((file_printf(ms,
			    mime ? " compressed-encoding=" : " (")) == -1)
				goto error;
			if ((pb = file_push_buffer(ms)) == NULL)
				goto error;
			/*
			 * XXX: If file_buffer fails here, we overwrite
			 * the compressed text. FIXME.
			 */
			if (file_buffer(ms, NULL, NULL, NULL, buf, nbytes) == -1) {
				if (file_pop_buffer(ms, pb) != NULL)
					abort();
				goto error;
			}
			if ((rbuf = file_pop_buffer(ms, pb)) != NULL) {
				if (file_printf(ms, "%s", rbuf) == -1) {
					efree(rbuf);
					goto error;
				}
				efree(rbuf);
			}
			if (!mime && file_printf(ms, ")") == -1)
				goto error;
			/*FALLTHROUGH*/
		case NODATA:
			break;
		default:
			abort();
			/*NOTREACHED*/
		error:
			rv = -1;
			break;
		}
	}
out:
	DPRINTF("rv = %d\n", rv);

	if (sa_saved && sig_act.sa_handler != SIG_IGN)
		(void)sigaction(SIGPIPE, &sig_act, NULL);

	if (newbuf)
		efree(newbuf);
	ms->flags |= MAGIC_COMPRESS;
	DPRINTF("Zmagic returns %d\n", rv);
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

	if (t > 0 && CAST(size_t, t) < n) {
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
			buf = CAST(char *, CCAST(void *, buf)) + rv;
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
		(void)umask(ou);
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

	if (swrite(tfd, startbuf, nbytes) != CAST(ssize_t, nbytes))
		r = 1;
	else {
		while ((r = sread(fd, buf, sizeof(buf), 1)) > 0)
			if (swrite(tfd, buf, CAST(size_t, r)) != r)
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


private int
uncompressgzipped(const unsigned char *old, unsigned char **newch,
    size_t bytes_max, size_t *n)
{
	unsigned char flg = old[3];
	size_t data_start = 10;

	if (flg & FEXTRA) {
		if (data_start + 1 >= *n)
			goto err;
		data_start += 2 + old[data_start] + old[data_start + 1] * 256;
	}
	if (flg & FNAME) {
		while(data_start < *n && old[data_start])
			data_start++;
		data_start++;
	}
	if (flg & FCOMMENT) {
		while(data_start < *n && old[data_start])
			data_start++;
		data_start++;
	}
	if (flg & FHCRC)
		data_start += 2;

	if (data_start >= *n)
		goto err;

	*n -= data_start;
	old += data_start;
	return uncompresszlib(old, newch, bytes_max, n, 0);
err:
	return makeerror(newch, n, "File too short");
}

private int
uncompresszlib(const unsigned char *old, unsigned char **newch,
    size_t bytes_max, size_t *n, int zlib)
{
	int rc;
	z_stream z;

	if ((*newch = CAST(unsigned char *, emalloc(bytes_max + 1))) == NULL)
		return makeerror(newch, n, "No buffer, %s", strerror(errno));

	z.next_in = CCAST(Bytef *, old);
	z.avail_in = CAST(uint32_t, *n);
	z.next_out = *newch;
	z.avail_out = CAST(unsigned int, bytes_max);
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	/* LINTED bug in header macro */
	rc = zlib ? inflateInit(&z) : inflateInit2(&z, -15);
	if (rc != Z_OK)
		goto err;

	rc = inflate(&z, Z_SYNC_FLUSH);
	if (rc != Z_OK && rc != Z_STREAM_END)
		goto err;

	*n = CAST(size_t, z.total_out);
	rc = inflateEnd(&z);
	if (rc != Z_OK)
		goto err;

	/* let's keep the nul-terminate tradition */
	(*newch)[*n] = '\0';

	return OKDATA;
err:
	strlcpy(RCAST(char *, *newch), z.msg ? z.msg : zError(rc), bytes_max);
	*n = strlen(RCAST(char *, *newch));
	return ERRDATA;
}
#endif

static int
makeerror(unsigned char **buf, size_t *len, const char *fmt, ...)
{
	char *msg;
	va_list ap;
	int rv;

	va_start(ap, fmt);
	rv = vasprintf(&msg, fmt, ap);
	va_end(ap);
	if (rv < 0) {
		*buf = NULL;
		*len = 0;
		return NODATA;
	}
	*buf = RCAST(unsigned char *, msg);
	*len = strlen(msg);
	return ERRDATA;
}

static void
closefd(int *fd, size_t i)
{
	if (fd[i] == -1)
		return;
	(void) close(fd[i]);
	fd[i] = -1;
}

static void
closep(int *fd)
{
	size_t i;
	for (i = 0; i < 2; i++)
		closefd(fd, i);
}

static int
copydesc(int i, int fd)
{
	if (fd == i)
		return 0; /* "no dup was necessary" */
	if (dup2(fd, i) == -1) {
		DPRINTF("dup(%d, %d) failed (%s)\n", fd, i, strerror(errno));
		exit(1);
	}
	return 1;
}

static pid_t
writechild(int fd, const void *old, size_t n)
{
	pid_t pid;

	/*
	 * fork again, to avoid blocking because both
	 * pipes filled
	 */
	pid = fork();
	if (pid == -1) {
		DPRINTF("Fork failed (%s)\n", strerror(errno));
		exit(1);
	}
	if (pid == 0) {
		/* child */
		if (swrite(fd, old, n) != CAST(ssize_t, n)) {
			DPRINTF("Write failed (%s)\n", strerror(errno));
			exit(1);
		}
		exit(0);
	}
	/* parent */
	return pid;
}

static ssize_t
filter_error(unsigned char *ubuf, ssize_t n)
{
	char *p;
	char *buf;

	ubuf[n] = '\0';
	buf = RCAST(char *, ubuf);
	while (isspace(CAST(unsigned char, *buf)))
		buf++;
	DPRINTF("Filter error[[[%s]]]\n", buf);
	if ((p = strchr(CAST(char *, buf), '\n')) != NULL)
		*p = '\0';
	if ((p = strchr(CAST(char *, buf), ';')) != NULL)
		*p = '\0';
	if ((p = strrchr(CAST(char *, buf), ':')) != NULL) {
		++p;
		while (isspace(CAST(unsigned char, *p)))
			p++;
		n = strlen(p);
		memmove(ubuf, p, CAST(size_t, n + 1));
	}
	DPRINTF("Filter error after[[[%s]]]\n", (char *)ubuf);
	if (islower(*ubuf))
		*ubuf = toupper(*ubuf);
	return n;
}

private const char *
methodname(size_t method)
{
#ifdef BUILTIN_DECOMPRESS
        /* FIXME: This doesn't cope with bzip2 */
	if (method == 2 || compr[method].maglen == 0)
	    return "zlib";
#endif
	return compr[method].argv[0];
}

private int
uncompressbuf(int fd, size_t bytes_max, size_t method, const unsigned char *old,
    unsigned char **newch, size_t* n)
{
	int fdp[3][2];
	int status, rv, w;
	pid_t pid;
	pid_t writepid = -1;
	size_t i;
	ssize_t r;

#ifdef BUILTIN_DECOMPRESS
        /* FIXME: This doesn't cope with bzip2 */
	if (method == 2)
		return uncompressgzipped(old, newch, bytes_max, n);
	if (compr[method].maglen == 0)
		return uncompresszlib(old, newch, bytes_max, n, 1);
#endif
	(void)fflush(stdout);
	(void)fflush(stderr);

	for (i = 0; i < __arraycount(fdp); i++)
		fdp[i][0] = fdp[i][1] = -1;

	if ((fd == -1 && pipe(fdp[STDIN_FILENO]) == -1) ||
	    pipe(fdp[STDOUT_FILENO]) == -1 || pipe(fdp[STDERR_FILENO]) == -1) {
		closep(fdp[STDIN_FILENO]);
		closep(fdp[STDOUT_FILENO]);
		return makeerror(newch, n, "Cannot create pipe, %s",
		    strerror(errno));
	}

	/* For processes with large mapped virtual sizes, vfork
	 * may be _much_ faster (10-100 times) than fork.
	 */
	pid = vfork();
	if (pid == -1) {
		return makeerror(newch, n, "Cannot vfork, %s",
		    strerror(errno));
	}
	if (pid == 0) {
		/* child */
		/* Note: we are after vfork, do not modify memory
		 * in a way which confuses parent. In particular,
		 * do not modify fdp[i][j].
		 */
		if (fd != -1) {
			(void) lseek(fd, CAST(off_t, 0), SEEK_SET);
			if (copydesc(STDIN_FILENO, fd))
				(void) close(fd);
		} else {
			if (copydesc(STDIN_FILENO, fdp[STDIN_FILENO][0]))
				(void) close(fdp[STDIN_FILENO][0]);
			if (fdp[STDIN_FILENO][1] > 2)
				(void) close(fdp[STDIN_FILENO][1]);
		}
///FIXME: if one of the fdp[i][j] is 0 or 1, this can bomb spectacularly
		if (copydesc(STDOUT_FILENO, fdp[STDOUT_FILENO][1]))
			(void) close(fdp[STDOUT_FILENO][1]);
		if (fdp[STDOUT_FILENO][0] > 2)
			(void) close(fdp[STDOUT_FILENO][0]);

		if (copydesc(STDERR_FILENO, fdp[STDERR_FILENO][1]))
			(void) close(fdp[STDERR_FILENO][1]);
		if (fdp[STDERR_FILENO][0] > 2)
			(void) close(fdp[STDERR_FILENO][0]);

		(void)execvp(compr[method].argv[0],
		    RCAST(char *const *, RCAST(intptr_t, compr[method].argv)));
		dprintf(STDERR_FILENO, "exec `%s' failed, %s",
		    compr[method].argv[0], strerror(errno));
		_exit(1); /* _exit(), not exit(), because of vfork */
	}
	/* parent */
	/* Close write sides of child stdout/err pipes */
	for (i = 1; i < __arraycount(fdp); i++)
		closefd(fdp[i], 1);
	/* Write the buffer data to child stdin, if we don't have fd */
	if (fd == -1) {
		closefd(fdp[STDIN_FILENO], 0);
		writepid = writechild(fdp[STDIN_FILENO][1], old, *n);
		closefd(fdp[STDIN_FILENO], 1);
	}

	*newch = CAST(unsigned char *, malloc(bytes_max + 1));
	if (*newch == NULL) {
		rv = makeerror(newch, n, "No buffer, %s",
		    strerror(errno));
		goto err;
	}
	rv = OKDATA;
	r = sread(fdp[STDOUT_FILENO][0], *newch, bytes_max, 0);
	if (r <= 0) {
		DPRINTF("Read stdout failed %d (%s)\n", fdp[STDOUT_FILENO][0],
		    r != -1 ? strerror(errno) : "no data");

		rv = ERRDATA;
		if (r == 0 &&
		    (r = sread(fdp[STDERR_FILENO][0], *newch, bytes_max, 0)) > 0)
		{
			r = filter_error(*newch, r);
			goto ok;
		}
		free(*newch);
		if  (r == 0)
			rv = makeerror(newch, n, "Read failed, %s",
			    strerror(errno));
		else
			rv = makeerror(newch, n, "No data");
		goto err;
	}
ok:
	*n = r;
	/* NUL terminate, as every buffer is handled here. */
	(*newch)[*n] = '\0';
err:
	closefd(fdp[STDIN_FILENO], 1);
	closefd(fdp[STDOUT_FILENO], 0);
	closefd(fdp[STDERR_FILENO], 0);

	w = waitpid(pid, &status, 0);
wait_err:
	if (w == -1) {
		free(*newch);
		rv = makeerror(newch, n, "Wait failed, %s", strerror(errno));
		DPRINTF("Child wait return %#x\n", status);
	} else if (!WIFEXITED(status)) {
		DPRINTF("Child not exited (%#x)\n", status);
	} else if (WEXITSTATUS(status) != 0) {
		DPRINTF("Child exited (%#x)\n", WEXITSTATUS(status));
	}
	if (writepid > 0) {
		/* _After_ we know decompressor has exited, our input writer
		 * definitely will exit now (at worst, writing fails in it,
		 * since output fd is closed now on the reading size).
		 */
		w = waitpid(writepid, &status, 0);
		writepid = -1;
		goto wait_err;
	}

	closefd(fdp[STDIN_FILENO], 0); //why? it is already closed here!
	DPRINTF("Returning %p n=%" SIZE_T_FORMAT "u rv=%d\n", *newch, *n, rv);

	return rv;
}
#endif
#endif
