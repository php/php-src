#ifndef _FLOCK_COMPAT_H
#define _FLOCK_COMPAT_H

#if !HAVE_FLOCK
#	define LOCK_SH 1
#	define LOCK_EX 2
#	define LOCK_NB 4
#	define LOCK_UN 8
int flock(int fd, int operation);
#endif

#if WIN32|WINNT
#	define fsync _commit
#	define ftruncate chsize
#endif /* WIN32|WINNT */

#if !HAVE_INET_ATON
extern int inet_aton(const char *, struct in_addr *);
#endif

#endif	/* _FLOCK_COMPAT_H */
