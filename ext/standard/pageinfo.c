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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "pageinfo.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#ifdef HAVE_GRP_H
# include <grp.h>
#endif
#ifdef PHP_WIN32
#undef getgid
#define getgroups(a, b) 0
#define getgid() 1
#define getuid() 1
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifdef PHP_WIN32
#include <process.h>
#endif

#include "ext/standard/basic_functions.h"

/* {{{ php_statpage */
PHPAPI void php_statpage(void)
{
	zend_stat_t *pstat = NULL;

	pstat = sapi_get_stat();

	if (BG(page_uid)==-1 || BG(page_gid)==-1) {
		if(pstat) {
			BG(page_uid)   = pstat->st_uid;
			BG(page_gid)   = pstat->st_gid;
			BG(page_inode) = pstat->st_ino;
			BG(page_mtime) = pstat->st_mtime;
		} else { /* handler for situations where there is no source file, ex. php -r */
			BG(page_uid) = getuid();
			BG(page_gid) = getgid();
		}
	}
}
/* }}} */

/* {{{ php_getuid */
zend_long php_getuid(void)
{
	php_statpage();
	return (BG(page_uid));
}
/* }}} */

zend_long php_getgid(void)
{
	php_statpage();
	return (BG(page_gid));
}

/* {{{ Get PHP script owner's UID */
PHP_FUNCTION(getmyuid)
{
	zend_long uid;

	ZEND_PARSE_PARAMETERS_NONE();

	uid = php_getuid();
	if (uid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(uid);
	}
}
/* }}} */

/* {{{ Get PHP script owner's GID */
PHP_FUNCTION(getmygid)
{
	zend_long gid;

	ZEND_PARSE_PARAMETERS_NONE();

	gid = php_getgid();
	if (gid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(gid);
	}
}
/* }}} */

/* {{{ Get current process ID */
PHP_FUNCTION(getmypid)
{
	zend_long pid;

	ZEND_PARSE_PARAMETERS_NONE();

	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(pid);
	}
}
/* }}} */

/* {{{ Get the inode of the current script being parsed */
PHP_FUNCTION(getmyinode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_statpage();
	if (BG(page_inode) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(BG(page_inode));
	}
}
/* }}} */

PHPAPI time_t php_getlastmod(void)
{
	php_statpage();
	return BG(page_mtime);
}

/* {{{ Get time of last page modification */
PHP_FUNCTION(getlastmod)
{
	zend_long lm;

	ZEND_PARSE_PARAMETERS_NONE();

	lm = php_getlastmod();
	if (lm < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(lm);
	}
}
/* }}} */
