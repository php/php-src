/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

#ifndef FLOCK_COMPAT_H
#define FLOCK_COMPAT_H

#ifdef HAVE_STRUCT_FLOCK
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#endif

#ifdef PHP_WIN32
#include <io.h>
#include "config.w32.h"
#endif

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

#endif	/* FLOCK_COMPAT_H */
