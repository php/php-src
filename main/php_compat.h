#ifndef PHP_COMPAT_H
#define PHP_COMPAT_H

#if (WIN32|WINNT) & HAVE_BINDLIB
#ifndef WINNT
#define WINNT 1
#endif
#include "arpa/inet.h"
#include "netdb.h"
#include "arpa/nameser.h"
#include "resolv.h"
#endif

#if !HAVE_FLOCK

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

extern PHPAPI int flock(int fd, int operation);

#endif

#if WIN32|WINNT

/* These were in win32/flock.h, dunno if they are really needed
   (or maybe break something) */

#define fsync _commit
#define ftruncate chsize

#endif /* WIN32|WINNT */

#if !HAVE_INET_ATON
extern PHPAPI int inet_aton(const char *, struct in_addr *);
#endif

#endif
