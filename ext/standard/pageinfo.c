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
	php_stat_t *pstat;

	pstat = sapi_get_stat(TSRMLS_C);

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
php_int_t php_getuid(TSRMLS_D)
{
	php_statpage(TSRMLS_C);
	return (BG(page_uid));
}
/* }}} */

php_int_t php_getgid(TSRMLS_D)
{
	php_statpage(TSRMLS_C);
	return (BG(page_gid));
}

/* {{{ proto int getmyuid(void)
   Get PHP script owner's UID */
PHP_FUNCTION(getmyuid)
{
	php_int_t uid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	uid = php_getuid(TSRMLS_C);
	if (uid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_INT(uid);
	}
}
/* }}} */

/* {{{ proto int getmygid(void)
   Get PHP script owner's GID */
PHP_FUNCTION(getmygid)
{
	php_int_t gid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	gid = php_getgid(TSRMLS_C);
	if (gid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_INT(gid);
	}
}
/* }}} */

/* {{{ proto int getmypid(void)
   Get current process ID */
PHP_FUNCTION(getmypid)
{
	php_int_t pid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_INT(pid);
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

	php_statpage(TSRMLS_C);
	if (BG(page_inode) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_INT(BG(page_inode));
	}
}
/* }}} */

PHPAPI time_t php_getlastmod(TSRMLS_D)
{
	php_statpage(TSRMLS_C);
	return BG(page_mtime);
}

/* {{{ proto int getlastmod(void)
   Get time of last page modification */
PHP_FUNCTION(getlastmod)
{
	php_int_t lm;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	lm = php_getlastmod(TSRMLS_C);
	if (lm < 0) {
		RETURN_FALSE;
	} else {
		RETURN_INT(lm);
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
