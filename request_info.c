/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "SAPI.h"

#ifndef THREAD_SAFE
PHPAPI php_request_info request_info;
#endif

#if CGI_BINARY
int php_init_request_info(void *conf)
{
	request_info.current_user = NULL;
	request_info.current_user_length = 0;
	request_info.script_name = getenv("SCRIPT_NAME");
	request_info.script_filename = getenv("SCRIPT_FILENAME");
	/* Hack for annoying servers that do not set SCRIPT_FILENAME for us */
	if (!request_info.script_filename) {
		request_info.script_filename = request_info.php_argv0;
	}
#if PHP_WIN32
	/* FIXME WHEN APACHE NT IS FIXED */
	/* a hack for apache nt because it does not appear to set argv[1] and sets
	   script filename to php.exe thus makes us parse php.exe instead of file.php
	   requires we get the info from path translated.  This can be removed at
	   such a time taht apache nt is fixed */
	  else {
		request_info.script_filename = getenv("PATH_TRANSLATED");
	}
#endif

	/* doc_root configuration variable is currently ignored,
	   as it is with every other access method currently also. */

	/* We always need to emalloc() filename, since it gets placed into
	   the include file hash table, and gets freed with that table.
	   Notice that this means that we don't need to efree() it in
	   php_destroy_request_info()! */
#if DISCARD_PATH
	if (request_info.script_filename) {
		SLS_FETCH();
		SG(request_info).path_translated = estrdup(request_info.script_filename);
	} else {
		SLS_FETCH();
		SG(request_info).path_translated = NULL;
	}
#endif
	return SUCCESS;
}

int php_destroy_request_info(void *conf)
{
	STR_FREE(request_info.current_user);
	return SUCCESS;
}
#endif


#if APACHE
int php_init_request_info(void *conf)
{
	request_rec *r;
	SLS_FETCH();

	r = ((request_rec *) SG(server_context));
	request_info.current_user = NULL;
	request_info.current_user_length = 0;

	return SUCCESS;
}

#endif

#if !CGI_BINARY
int php_destroy_request_info(void *conf)
{
	/* see above for why we don't want to efree() request_info.filename */
	STR_FREE(request_info.current_user);
	return SUCCESS;
}
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
