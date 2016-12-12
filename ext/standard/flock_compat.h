/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef FLOCK_COMPAT_H
#define FLOCK_COMPAT_H

/* php_flock internally uses fcntl whether or not flock is available
 * This way our php_flock even works on NFS files.
 * More info: /usr/src/linux/Documentation
 */
PHPAPI int php_flock(int fd, int operation);

#ifndef HAVE_FLOCK
#	define LOCK_SH 1
#	define LOCK_EX 2
#	define LOCK_NB 4
#	define LOCK_UN 8
PHPAPI int flock(int fd, int operation);
#endif

/* Userland LOCK_* constants */
#define PHP_LOCK_SH 1
#define PHP_LOCK_EX 2
#define PHP_LOCK_UN 3
#define PHP_LOCK_NB 4

#ifdef PHP_WIN32
# ifdef EWOULDBLOCK
#  undef EWOULDBLOCK
# endif
# define EWOULDBLOCK WSAEWOULDBLOCK
# define fsync _commit
# define ftruncate(a, b) chsize(a, b)
#endif /* defined(PHP_WIN32) */

#if !HAVE_INET_ATON
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef PHP_WIN32
extern int inet_aton(const char *, struct in_addr *);
#endif
#endif

#endif	/* FLOCK_COMPAT_H */
