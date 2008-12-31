/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#include <stdio.h>
#include <stdlib.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include "ext/standard/pageinfo.h"
#include "safe_mode.h"
#include "SAPI.h"
#include "php_globals.h"

/*
 * php_checkuid
 *
 * This function has six modes:
 * 
 * 0 - return invalid (0) if file does not exist
 * 1 - return valid (1)  if file does not exist
 * 2 - if file does not exist, check directory
 * 3 - only check directory (needed for mkdir)
 * 4 - check mode and param
 * 5 - only check file
 */

PHPAPI int php_checkuid_ex(const char *filename, const char *fopen_mode, int mode, int flags)
{
	struct stat sb;
	int ret, nofile=0;
	long uid=0L, gid=0L, duid=0L, dgid=0L;
	char path[MAXPATHLEN];
	char *s, filenamecopy[MAXPATHLEN];
	TSRMLS_FETCH();

	path[0] = '\0';

	if (!filename) {
		return 0; /* path must be provided */
	}

	if (strlcpy(filenamecopy, filename, MAXPATHLEN)>=MAXPATHLEN) {
		return 0;
	}
	filename=(char *)&filenamecopy;

	if (fopen_mode) {
		if (fopen_mode[0] == 'r') {
			mode = CHECKUID_DISALLOW_FILE_NOT_EXISTS;
		} else {
			mode = CHECKUID_CHECK_FILE_AND_DIR;
		}
	}
		
	/* First we see if the file is owned by the same user...
	 * If that fails, passthrough and check directory...
	 */
	if (mode != CHECKUID_ALLOW_ONLY_DIR) {
#if HAVE_BROKEN_GETCWD
		char ftest[MAXPATHLEN];

		strcpy(ftest, filename);
		if (VCWD_GETCWD(ftest, sizeof(ftest)) == NULL) {
			strcpy(path, filename);
		} else
#endif
		expand_filepath(filename, path TSRMLS_CC);

		ret = VCWD_STAT(path, &sb);
		if (ret < 0) {
			if (mode == CHECKUID_DISALLOW_FILE_NOT_EXISTS) {
				if ((flags & CHECKUID_NO_ERRORS) == 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to access %s", filename);
				}
				return 0;
			} else if (mode == CHECKUID_ALLOW_FILE_NOT_EXISTS) {
				if ((flags & CHECKUID_NO_ERRORS) == 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to access %s", filename);
				}
				return 1;
			} 
			nofile = 1;
		} else {
			uid = sb.st_uid;
			gid = sb.st_gid;
			if (uid == php_getuid()) {
				return 1;
 			} else if (PG(safe_mode_gid) && gid == php_getgid()) {
 				return 1;
			}
		}

		/* Trim off filename */
		if ((s = strrchr(path, DEFAULT_SLASH))) {
			if (*(s + 1) == '\0' && s != path) { /* make sure that the / is not the last character */
				*s = '\0';
				s = strrchr(path, DEFAULT_SLASH);
			}
			if (s) {
				if (s == path) {
					path[1] = '\0';
				} else {
					*s = '\0';
				}
			}
		}
	} else { /* CHECKUID_ALLOW_ONLY_DIR */
		s = strrchr(filename, DEFAULT_SLASH);

		if (s == filename) {
			/* root dir */
			path[0] = DEFAULT_SLASH;
			path[1] = '\0';
		} else if (s && *(s + 1) != '\0') { /* make sure that the / is not the last character */
			*s = '\0';
			VCWD_REALPATH(filename, path);
			*s = DEFAULT_SLASH;
		} else {
			/* Under Solaris, getcwd() can fail if there are no
			 * read permissions on a component of the path, even
			 * though it has the required x permissions */
			path[0] = '.';
			path[1] = '\0';
			VCWD_GETCWD(path, sizeof(path));
 		}
	} /* end CHECKUID_ALLOW_ONLY_DIR */
	
	if (mode != CHECKUID_ALLOW_ONLY_FILE) {
		/* check directory */
		ret = VCWD_STAT(path, &sb);
		if (ret < 0) {
			if ((flags & CHECKUID_NO_ERRORS) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to access %s", filename);
			}
			return 0;
		}
		duid = sb.st_uid;
		dgid = sb.st_gid;
		if (duid == php_getuid()) {
			return 1;
 		} else if (PG(safe_mode_gid) && dgid == php_getgid()) {
 			return 1;
		} else {
			if (SG(rfc1867_uploaded_files)) {
				if (zend_hash_exists(SG(rfc1867_uploaded_files), (char *) filename, strlen(filename)+1)) {
					return 1;
				}
			}
		}
	}

	if (mode == CHECKUID_ALLOW_ONLY_DIR) {
		uid = duid;
		gid = dgid;
		if (s) {
			*s = 0;
		}
	}
	
	if (nofile) {
		uid = duid;
		gid = dgid;
		filename = path;
	}

	if ((flags & CHECKUID_NO_ERRORS) == 0) {
		if (PG(safe_mode_gid)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SAFE MODE Restriction in effect.  The script whose uid/gid is %ld/%ld is not allowed to access %s owned by uid/gid %ld/%ld", php_getuid(), php_getgid(), filename, uid, gid);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SAFE MODE Restriction in effect.  The script whose uid is %ld is not allowed to access %s owned by uid %ld", php_getuid(), filename, uid);
		}			
	}

	return 0;
}

PHPAPI int php_checkuid(const char *filename, const char *fopen_mode, int mode)
{
#ifdef NETWARE
/* NetWare don't have uid*/
	return 1;
#else
	return php_checkuid_ex(filename, fopen_mode, mode, 0);
#endif
}

PHPAPI char *php_get_current_user(void)
{
	struct stat *pstat;
	TSRMLS_FETCH();

	if (SG(request_info).current_user) {
		return SG(request_info).current_user;
	}

	/* FIXME: I need to have this somehow handled if
	USE_SAPI is defined, because cgi will also be
	interfaced in USE_SAPI */

	pstat = sapi_get_stat(TSRMLS_C);

	if (!pstat) {
		return "";
	} else {
#ifdef PHP_WIN32
		char name[256];
		DWORD len = sizeof(name)-1;

		if (!GetUserName(name, &len)) {
			return "";
		}
		name[len] = '\0';
		SG(request_info).current_user_length = len;
		SG(request_info).current_user = estrndup(name, len);
		return SG(request_info).current_user;		
#else
		struct passwd *pwd;
#if defined(ZTS) && defined(HAVE_GETPWUID_R) && defined(_SC_GETPW_R_SIZE_MAX)
		struct passwd _pw;
		struct passwd *retpwptr = NULL;
		int pwbuflen = sysconf(_SC_GETPW_R_SIZE_MAX);
		char *pwbuf;

		if (pwbuflen < 1) {
			return "";
		}
		pwbuf = emalloc(pwbuflen);
		if (getpwuid_r(pstat->st_uid, &_pw, pwbuf, pwbuflen, &retpwptr) != 0) {
			efree(pwbuf);
			return "";
		}
		pwd = &_pw;
#else
		if ((pwd=getpwuid(pstat->st_uid))==NULL) {
			return "";
		}
#endif
		SG(request_info).current_user_length = strlen(pwd->pw_name);
		SG(request_info).current_user = estrndup(pwd->pw_name, SG(request_info).current_user_length);
#if defined(ZTS) && defined(HAVE_GETPWUID_R) && defined(_SC_GETPW_R_SIZE_MAX)
		efree(pwbuf);
#endif
		return SG(request_info).current_user;		
#endif
	}	
}	

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
