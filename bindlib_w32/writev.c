#ifndef LINT
static char rcsid[] = "$Id$";
#endif

/* writev() emulations contained in this source file for the following systems:
 *
 *	Cray UNICOS
 *	SCO
 *  WindowsNT
 */

#if defined(_CRAY)
#define OWN_WRITEV
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/socket.h>

int
__writev(int fd, struct iovec *iov, int iovlen)
{
	struct stat statbuf;

	if (fstat(fd, &statbuf) < 0)
		return (-1);

	/*
	 * Allow for atomic writes to network.
	 */
	if (statbuf.st_mode & S_IFSOCK) {
		struct msghdr   mesg;		

		mesg.msg_name = 0;
		mesg.msg_namelen = 0;
		mesg.msg_iov = iov;
		mesg.msg_iovlen = iovlen;
		mesg.msg_accrights = 0;
		mesg.msg_accrightslen = 0;
		return (sendmsg(fd, &mesg, 0));
	} else {
		register struct iovec *tv;
		register int i, rcode = 0, count = 0;

		for (i = 0, tv = iov; i <= iovlen; tv++) {
			rcode = write(fd, tv->iov_base, tv->iov_len);

			if (rcode < 0)
				break;

			count += rcode;
		}

		if (count == 0)
			return (rcode);
		else
			return (count);
	}
}
#endif

#if defined (M_UNIX) && !defined(_SCO_DS) || defined (NEED_WRITEV)
#define OWN_WRITEV
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

int
__writev(fd, vp, vpcount)
	int fd;
	const struct iovec *vp;
	register int vpcount;
{
	register int count = 0;

	while (vpcount-- > 0) {
		register int written = write(fd, vp->iov_base, vp->iov_len);

		if (written <= 0)
			return (-1);
		count += written;
		vp++;
	}
	return (count);
}
#endif

#ifdef WINNT
#define OWN_WRITEV
#define TIMEOUT_SEC 120
#include <stdarg.h>
#include "conf/portability.h"


/*
 * writev --
 * simplistic writev implementation for WindowsNT using the WriteFile WIN32API.
 */	
/* lgk win95 does not support overlapped/async file operations so change it to
   synchronous */
/* Zeev:  We don't compile the whole bindlib, so stay with Windows 95's implementation
 * (we dont have the hReadWriteEvent handle available)
 */

int
writev(fd, iov, iovcnt)
	int fd;
	struct iovec *iov;
	int iovcnt;
{
	int i;
	char *base;
	DWORD BytesWritten, TotalBytesWritten = 0, len;
	BOOL ret; 

	for (i=0; i<iovcnt; i++) {
		base = iov[i].iov_base;
		len = (DWORD)iov[i].iov_len;
		ret = WriteFile((HANDLE)fd, (char *)base, len,
		              (LPDWORD)&BytesWritten, NULL);
		if (ret == FALSE)
		   return (-1);
		TotalBytesWritten += BytesWritten;
	}
	return((int)TotalBytesWritten);
}

#endif // winnt

#ifndef OWN_WRITEV
int __bindcompat_writev;
#endif
