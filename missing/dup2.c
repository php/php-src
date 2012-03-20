/*
 * Public domain dup2() lookalike
 * by Curtis Jackson @ AT&T Technologies, Burlington, NC
 * electronic address:  burl!rcj
 *
 * dup2 performs the following functions:
 *
 * Check to make sure that fd1 is a valid open file descriptor.
 * Check to see if fd2 is already open; if so, close it.
 * Duplicate fd1 onto fd2; checking to make sure fd2 is a valid fd.
 * Return fd2 if all went well; return BADEXIT otherwise.
 */

#include "ruby/config.h"

#if defined(HAVE_FCNTL)
# include <fcntl.h>
#endif

#if !defined(HAVE_FCNTL) || !defined(F_DUPFD)
# include <errno.h>
#endif

#define BADEXIT -1

int
dup2(int fd1, int fd2)
{
#if defined(HAVE_FCNTL) && defined(F_DUPFD)
	if (fd1 != fd2) {
#ifdef F_GETFL
		if (fcntl(fd1, F_GETFL) < 0)
			return BADEXIT;
		if (fcntl(fd2, F_GETFL) >= 0)
			close(fd2);
#else
		close(fd2);
#endif
		if (fcntl(fd1, F_DUPFD, fd2) < 0)
			return BADEXIT;
	}
	return fd2;
#else
	extern int errno;
	int i, fd, fds[256];

	if (fd1 == fd2) return 0;
	close(fd2);
	for (i=0; i<256; i++) {
		fd = fds[i] = dup(fd1);
		if (fd == fd2) break;
	}
	while (i) {
	    	close(fds[i--]);
	}
	if (fd == fd2) return 0;
	errno = EMFILE;
	return BADEXIT;
#endif
}
