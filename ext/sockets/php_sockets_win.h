/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

#ifdef PHP_WIN32

#define F_SETFL		0
#define F_GETFL		1
#define F_SETFD		2
#define F_GETFD		3

#define O_NONBLOCK FIONBIO

#define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define ECONNRESET		WSAECONNRESET

#ifdef errno
#undef errno
#endif

#define errno WSAGetLastError()
#define h_errno WSAGetLastError()
#define set_errno(a) WSASetLastError(a)
#define set_h_errno(a) WSASetLastError(a)
#define close(a) closesocket(a)
#define CMSG_DATA(cmsg) ((cmsg)->cmsg_data)

typedef int ssize_t;

struct	sockaddr_un {
	short	sun_family;
	char	sun_path[108];
};

struct iovec {
	char *  iov_base;
	int 	iov_len;
};

struct msghdr {
	void*			msg_name;
	socklen_t		msg_namelen;
	struct iovec*	msg_iov;
	int				msg_iovlen;
	void*			msg_control;
	socklen_t		msg_controllen;
	int				msg_flags;
};

struct cmsghdr {
	socklen_t	cmsg_len;
	int			cmsg_level;
	int			cmsg_type;
	unsigned char      cmsg_data[];
};

ssize_t readv(SOCKET sock, const struct iovec *iov, int iovcnt);
ssize_t writev(SOCKET sock, const struct iovec *iov, int iovcnt);
ssize_t recvmsg(SOCKET sock, struct msghdr *msg, int flags);
ssize_t sendmsg(SOCKET sock, struct msghdr *msg, int flags);
int socketpair(int domain, int type, int protocol, SOCKET sock[2]);
int inet_aton(const char *cp, struct in_addr *inp);
int	fcntl(int fd, int cmd, ...);

#endif
