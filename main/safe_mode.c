/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
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
 * This function has four modes:
 * 
 * 0 - return invalid (0) if file does not exist
 * 1 - return valid (1)  if file does not exist
 * 2 - if file does not exist, check directory
 * 3 - only check directory (needed for mkdir)
 */

PHPAPI int php_checkuid(const char *filename, char *fopen_mode, int mode)
{
	struct stat sb;
	int ret;
	long uid=0L, gid=0L, duid=0L, dgid=0L;
	char *s;
	PLS_FETCH();

	if (!filename) {
		return 0; /* path must be provided */
	}

	if (fopen_mode) {
		if (fopen_mode[0] == 'r') {
			mode = CHECKUID_DISALLOW_FILE_NOT_EXISTS;
		} else {
			mode = CHECKUID_CHECK_FILE_AND_DIR;
		}
	}

	/* 
	 * If given filepath is a URL, allow - safe mode stuff
	 * related to URL's is checked in individual functions
     */	
	if (!strncasecmp(filename,"http://",7) || !strncasecmp(filename,"ftp://",6)) {
		return 1;
	}
		
	if (mode != CHECKUID_ALLOW_ONLY_DIR) {
		ret = VCWD_STAT(filename, &sb);
		if (ret < 0) {
			if (mode == CHECKUID_DISALLOW_FILE_NOT_EXISTS) {
				php_error(E_WARNING, "Unable to access %s", filename);
				return 0;
			} else if (mode == CHECKUID_ALLOW_FILE_NOT_EXISTS)
				php_error(E_WARNING, "Unable to access %s", filename);{
				return 1;
			}
		} else {
			uid = sb.st_uid;
			if (uid == php_getuid()) {
				return 1;
			}
		}
	}
	s = strrchr(filename,'/');

	/* This loop gets rid of trailing slashes which could otherwise be
	 * used to confuse the function.
	 */
	while(s && *(s+1)=='\0' && s>filename) {
		*s='\0';
		s = strrchr(filename,'/');
	}

	if (s) {
		*s='\0';
		ret = VCWD_STAT(filename, &sb);
		*s='/';
		if (ret < 0) {
			php_error(E_WARNING, "Unable to access %s", filename);
			return 0;
		}
		duid = sb.st_uid;
	} else {
		char cwd[MAXPATHLEN];
		if (!VCWD_GETCWD(cwd, MAXPATHLEN)) {
			php_error(E_WARNING, "Unable to access current working directory");
			return 0;
		}
		ret = VCWD_STAT(cwd, &sb);
		if (ret < 0) {
			php_error(E_WARNING, "Unable to access %s", cwd);
			return 0;
		}
		duid = sb.st_uid;
	}
	if (duid == (uid=php_getuid())) {
		return 1;
 	} else if (PG(safe_mode_gid) && dgid == (gid=php_getgid())) {
 		return 1;
	} else {
		SLS_FETCH();

		if (SG(rfc1867_uploaded_files)) {
			if (zend_hash_exists(SG(rfc1867_uploaded_files), (char *) filename, strlen(filename)+1)) {
				return 1;
			}
		}

 		if (PG(safe_mode_gid)) {
 			php_error(E_WARNING, "SAFE MODE Restriction in effect.  The script whose uid/gid is %ld/%ld is not allowed to access %s owned by uid/gid %ld/%ld", uid, gid, filename, duid, dgid);
 		} else {
 			php_error(E_WARNING, "SAFE MODE Restriction in effect.  The script whose uid is %ld is not allowed to access %s owned by uid %ld", uid, filename, duid);
 		}			
		return 0;
	}
}


PHPAPI char *php_get_current_user()
{
	struct passwd *pwd;
	struct stat *pstat;
	SLS_FETCH();

	if (SG(request_info).current_user) {
		return SG(request_info).current_user;
	}

	/* FIXME: I need to have this somehow handled if
	USE_SAPI is defined, because cgi will also be
	interfaced in USE_SAPI */

	pstat = sapi_get_stat();

	if (!pstat) {
		return empty_string;
	}

	if ((pwd=getpwuid(pstat->st_uid))==NULL) {
		return empty_string;
	}
	SG(request_info).current_user_length = strlen(pwd->pw_name);
	SG(request_info).current_user = estrndup(pwd->pw_name, SG(request_info).current_user_length);
	
	return SG(request_info).current_user;		
}	

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
