/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "pageinfo.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#elif defined(NETWARE)
#ifdef ZTS
extern int basic_globals_id;
#endif
#include "netware/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_GRP_H
# ifdef PHP_WIN32
#  include "win32/grp.h"
# else
#  include <grp.h>
# endif
#endif
#ifdef PHP_WIN32
#undef getgid
#define getgroups(a, b) 0
#define getgid() 1
#define getuid() 1
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifdef PHP_WIN32
#include <process.h>
#endif

#include "ext/standard/basic_functions.h"

/* {{{ php_statpage
 */
PHPAPI void php_statpage(TSRMLS_D)
{
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	struct stat_libc *pstat;
#else
	struct stat *pstat;
#endif

	pstat = sapi_get_stat(TSRMLS_C);

	if (BG(page_uid)==-1 || BG(page_gid)==-1) {
		if(pstat) {
			BG(page_uid)   = pstat->st_uid;
			BG(page_gid)   = pstat->st_gid;
			BG(page_inode) = pstat->st_ino;
#if defined(NETWARE) && defined(NEW_LIBC)
			BG(page_mtime) = (pstat->st_mtime).tv_nsec;
#else
			BG(page_mtime) = pstat->st_mtime;
#endif
		} else { /* handler for situations where there is no source file, ex. php -r */
			BG(page_uid) = getuid();
			BG(page_gid) = getgid();
		}
	}
}
/* }}} */

/* {{{ php_getuid
 */
long php_getuid(void)
{
	TSRMLS_FETCH();

	php_statpage(TSRMLS_C);
	return (BG(page_uid));
}
/* }}} */

long php_getgid(void)
{
	TSRMLS_FETCH();

	php_statpage(TSRMLS_C);
	return (BG(page_gid));
}

/* {{{ proto int getmyuid(void)
   Get PHP script owner's UID */
PHP_FUNCTION(getmyuid)
{
	long uid;
	
	uid = php_getuid();
	if (uid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(uid);
	}
}
/* }}} */

/* {{{ proto int getmygid(void)
   Get PHP script owner's GID */
PHP_FUNCTION(getmygid)
{
	long gid;
	
	gid = php_getgid();
	if (gid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(gid);
	}
}
/* }}} */

/* {{{ proto int getmypid(void)
   Get current process ID */
PHP_FUNCTION(getmypid)
{
	int pid;
	
	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG((long) pid);
	}
}
/* }}} */

/* {{{ proto int getmyinode(void)
   Get the inode of the current script being parsed */
PHP_FUNCTION(getmyinode)
{
	php_statpage(TSRMLS_C);
	if (BG(page_inode) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(BG(page_inode));
	}
}
/* }}} */

PHPAPI long php_getlastmod(TSRMLS_D)
{
	php_statpage(TSRMLS_C);
	return BG(page_mtime);
}

/* {{{ proto int getlastmod(void)
   Get time of last page modification */
PHP_FUNCTION(getlastmod)
{
	long lm = php_getlastmod(TSRMLS_C);
	if (lm < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(lm);
	}
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
