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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#include "win32/winutil.h"
#elif defined(NETWARE)
#ifdef USE_WINSOCK
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
#include <sys/socket.h>
#endif
#ifdef NEW_LIBC
#include <sys/param.h>
#else
#include "netware/param.h"
#endif
#include "netware/mktemp.h"
#else
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if !defined(P_tmpdir)
#define P_tmpdir ""
#endif

/* {{{ php_do_open_temporary_file */

/* Loosely based on a tempnam() implementation by UCLA */

/*
 * Copyright (c) 1988, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

static FILE *php_do_open_temporary_file(const char *path, const char *pfx, char **opened_path_p TSRMLS_DC)
{
	char *trailing_slash;
	FILE *fp;
	char *opened_path;
#ifndef PHP_WIN32
	int fd;
#endif
#ifdef NETWARE
    char *file_path = NULL;
#endif

	if (!path) {
		return NULL;
	}

	if (!(opened_path = emalloc(MAXPATHLEN))) {
		return NULL;
	}

	if (IS_SLASH(path[strlen(path)-1])) {
		trailing_slash = "";
	} else {
		trailing_slash = "/";
	}

	(void)snprintf(opened_path, MAXPATHLEN, "%s%s%sXXXXXX", path, trailing_slash, pfx);

#ifdef PHP_WIN32
	if (GetTempFileName(path, pfx, 0, opened_path)) {
		fp = VCWD_FOPEN(opened_path, "r+b");
	} else {
		fp = NULL;
	}
#elif defined(NETWARE)
	/* Using standard mktemp() implementation for NetWare */
	file_path = mktemp(opened_path);
	if (file_path) {
		fp = VCWD_FOPEN(file_path, "r+b");
	} else {
		fp = NULL;
	}
#elif defined(HAVE_MKSTEMP)
	fd = mkstemp(opened_path);
	if (fd==-1) {
		fp = NULL;
	} else {
		fp = fdopen(fd, "r+b");
	}
#else
	if (mktemp(opened_path)) {
		fp = VCWD_FOPEN(opened_path, "r+b");
	} else {
		fp = NULL;
	}
#endif
	if (!fp || !opened_path_p) {
		efree(opened_path);
	} else {
		*opened_path_p = opened_path;
	}
	return fp;
}
/* }}} */

/*
 *  Determine where to place temporary files.
 */
const char* get_temporary_directory()
{
	/* Cache the chosen temporary directory. */
	static char* temporary_directory;

	/* Did we determine the temporary directory already? */
	if (temporary_directory) {
		return temporary_directory;
	}

#ifdef PHP_WIN32
	/* We can't count on the environment variables TEMP or TMP,
	 * and so must make the Win32 API call to get the default
	 * directory for temporary files.  Note this call checks
	 * the environment values TMP and TEMP (in order) first.
	 */
	{
		char sTemp[MAX_PATH];
		DWORD n = GetTempPath(sizeof(sTemp),sTemp);
		assert(0 < n);  /* should *never* fail! */
		temporary_directory = strdup(sTemp);
		return temporary_directory;
	}
#else
	/* On Unix use the (usual) TMPDIR environment variable. */
	{
		char* s = getenv("TMPDIR");
		if (s) {
			temporary_directory = strdup(s);
			return temporary_directory;
		}
	}
#ifdef P_tmpdir
	/* Use the standard default temporary directory. */
	if (P_tmpdir) {
		temporary_directory = P_tmpdir;
		return temporary_directory;
	}
#endif
	/* Shouldn't ever(!) end up here ... last ditch default. */
	temporary_directory = "/tmp";
	return temporary_directory;
#endif
}

/* {{{ php_open_temporary_file
 *
 * Unlike tempnam(), the supplied dir argument takes precedence
 * over the TMPDIR environment variable
 * This function should do its best to return a file pointer to a newly created
 * unique file, on every platform.
 */
PHPAPI FILE *php_open_temporary_file(const char *dir, const char *pfx, char **opened_path_p TSRMLS_DC)
{
	FILE* fp = 0;

	if (!pfx) {
		pfx = "tmp.";
	}
	if (opened_path_p) {
		*opened_path_p = NULL;
	}

	/* Try the directory given as parameter. */
	fp = php_do_open_temporary_file(dir, pfx, opened_path_p TSRMLS_CC);
	if (fp) {
		return fp;
	}

	/* Use default temporary directory. */
	fp = php_do_open_temporary_file(get_temporary_directory(), pfx, opened_path_p TSRMLS_CC);
	if (fp) {
		return fp;
	}

	return 0;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
