#ifndef FLOCK_COMPAT_H
#define FLOCK_COMPAT_H

#ifndef HAVE_FLOCK
#	define LOCK_SH 1
#	define LOCK_EX 2
#	define LOCK_NB 4
#	define LOCK_UN 8
int flock(int fd, int operation);
#endif

#ifdef PHP_WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#	define fsync _commit
#	define ftruncate(a,b) chsize(a,b)
#endif /* defined(PHP_WIN32) */

#if !HAVE_INET_ATON
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

extern int inet_aton(const char *, struct in_addr *);
#endif

#endif	/* FLOCK_COMPAT_H */
