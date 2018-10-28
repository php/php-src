/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "pageinfo.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
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
PHPAPI void php_statpage(void)
{
	zend_stat_t *pstat;

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

/* {{{ php_getuid
 */
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

/* {{{ proto int getmyuid(void)
   Get PHP script owner's UID */
PHP_FUNCTION(getmyuid)
{
	zend_long uid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	zend_long gid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	zend_long pid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(pid);
	}
}
/* }}} */

/* {{{ proto int getmyinode(void)
   Get the inode of the current script being parsed */
PHP_FUNCTION(getmyinode)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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

/* {{{ proto int getlastmod(void)
   Get time of last page modification */
PHP_FUNCTION(getlastmod)
{
	zend_long lm;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	lm = php_getlastmod();
	if (lm < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(lm);
	}
}
/* }}} */

/*nma
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
