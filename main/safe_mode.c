/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
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

/*
 * _php3_checkuid
 *
 * This function has four modes:
 * 
 * 0 - return invalid (0) if file does not exist
 * 1 - return valid (1)  if file does not exist
 * 2 - if file does not exist, check directory
 * 3 - only check directory (needed for mkdir)
 */
PHPAPI int _php3_checkuid(const char *fn, int mode) {
	struct stat sb;
	int ret;
	long uid=0L, duid=0L;
	char *s;

	if (!fn) return(0); /* path must be provided */

	/* 
	 * If given filepath is a URL, allow - safe mode stuff
	 * related to URL's is checked in individual functions
     */	
	if (!strncasecmp(fn,"http://",7) || !strncasecmp(fn,"ftp://",6)) {
		return(1);
	}
		
	if (mode<3) {
		ret = stat(fn,&sb);
		if (ret<0 && mode < 2) {
			php3_error(E_WARNING,"Unable to access %s",fn);
			return(mode);
		}
		if (ret>-1) {
			uid=sb.st_uid;
			if (uid==_php3_getuid()) return(1);
		}
	}
	s = strrchr(fn,'/');

	/* This loop gets rid of trailing slashes which could otherwise be
	 * used to confuse the function.
	 */
	while(s && *(s+1)=='\0' && s>fn) {
		s='\0';
		s = strrchr(fn,'/');
	}

	if (s) {
		*s='\0';
		ret = stat(fn,&sb);
		*s='/';
		if (ret<0) {
			php3_error(E_WARNING, "Unable to access %s",fn);
			return(0);
		}
		duid = sb.st_uid;
	} else {
		s = emalloc(MAXPATHLEN+1);
		if (!getcwd(s,MAXPATHLEN)) {
			php3_error(E_WARNING, "Unable to access current working directory");
			return(0);
		}
		ret = stat(s,&sb);
		efree(s);
		if (ret<0) {
			php3_error(E_WARNING, "Unable to access %s",s);
			return(0);
		}
		duid = sb.st_uid;
	}
	if (duid == (uid=_php3_getuid())) return(1);
	else {
		php3_error(E_WARNING, "SAFE MODE Restriction in effect.  The script whose uid is %ld is not allowed to access %s owned by uid %ld",uid,fn,duid);
		return(0);
	}
}


PHPAPI char *_php3_get_current_user()
{
#if CGI_BINARY || USE_SAPI || FHTTPD
	struct stat statbuf;
#endif
	struct passwd *pwd;
	int uid;
	SLS_FETCH();

	if (request_info.current_user) {
		return request_info.current_user;
	}

	/* FIXME: I need to have this somehow handled if
	USE_SAPI is defined, because cgi will also be
	interfaced in USE_SAPI */
#if CGI_BINARY || USE_SAPI || FHTTPD
	if (!SG(request_info).path_translated || (stat(SG(request_info).path_translated,&statbuf)==-1)) {
		return empty_string;
	}
	uid = statbuf.st_uid;
#endif
#if APACHE
	uid = ((request_rec *) SG(server_context))->finfo.st_uid;
#endif

	if ((pwd=getpwuid(uid))==NULL) {
		return empty_string;
	}
	request_info.current_user_length = strlen(pwd->pw_name);
	request_info.current_user = estrndup(pwd->pw_name,request_info.current_user_length);
	
	return request_info.current_user;		
}	
