/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Defines and types for Windows 95/NT */
#define HAVE_DECLARED_TIMEZONE
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <malloc.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <process.h>

typedef int uid_t;
typedef int gid_t;
typedef char * caddr_t;
#define lstat(x, y) php_sys_lstat(x, y)
#define		_IFIFO	0010000	/* fifo */
#define		_IFBLK	0060000	/* block special */
#define		_IFLNK	0120000	/* symbolic link */
#define S_IFIFO		_IFIFO
#define S_IFBLK		_IFBLK
#define	S_IFLNK		_IFLNK
#ifndef S_ISREG 
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#define chdir(path) _chdir(path)
#define mkdir(a, b)	_mkdir(a)
#define rmdir(a)	_rmdir(a)
#define getpid		_getpid
#define php_sleep(t)	SleepEx(t*1000, TRUE)
#ifndef getcwd
# define getcwd(a, b)		_getcwd(a, b)
#endif
#define off_t		_off_t
typedef unsigned int uint;
typedef unsigned long ulong;
#if !NSAPI
typedef long pid_t;
#endif

/* missing in vc5 math.h */
#define M_PI             3.14159265358979323846
#define M_TWOPI         (M_PI * 2.0)
#define M_PI_2           1.57079632679489661923
#ifndef M_PI_4
#define M_PI_4           0.78539816339744830962
#endif

#if !defined(PHP_DEBUG)
#ifdef inline
#undef inline
#endif
#define inline  __inline
#endif

/* General Windows stuff */
#ifndef WINDOWS
# define WINDOWS 1
#endif


/* Prevent use of VC5 OpenFile function */
#define NOOPENFILE

/* sendmail is built-in */
#ifdef PHP_PROG_SENDMAIL
#undef PHP_PROG_SENDMAIL
#define PHP_PROG_SENDMAIL "Built in mailer"
#endif
