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
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"

#ifndef THREAD_SAFE
php3_request_info request_info;
#endif

#if CGI_BINARY
int php3_init_request_info(void *conf)
{
	char *buf;	/* temporary buffers */
	TLS_VARS;

	GLOBAL(request_info).path_info = getenv("PATH_INFO");
	GLOBAL(request_info).path_translated = getenv("PATH_TRANSLATED");
	GLOBAL(request_info).query_string = getenv("QUERY_STRING");
	GLOBAL(request_info).current_user = NULL;
	GLOBAL(request_info).current_user_length = 0;
	GLOBAL(request_info).request_method = getenv("REQUEST_METHOD");
	GLOBAL(request_info).script_name = getenv("SCRIPT_NAME");
	buf = getenv("CONTENT_LENGTH");
	GLOBAL(request_info).content_length = (buf ? atoi(buf) : 0);
	GLOBAL(request_info).content_type = getenv("CONTENT_TYPE");
	GLOBAL(request_info).cookies = getenv("HTTP_COOKIE");
	GLOBAL(request_info).script_filename = getenv("SCRIPT_FILENAME");
	/* Hack for annoying servers that do not set SCRIPT_FILENAME for us */
	if (!GLOBAL(request_info).script_filename) {
		GLOBAL(request_info).script_filename = GLOBAL(request_info).php_argv0;
	}
#if WIN32|WINNT
	/* FIXME WHEN APACHE NT IS FIXED */
	/* a hack for apache nt because it does not appear to set argv[1] and sets
	   script filename to php.exe thus makes us parse php.exe instead of file.php
	   requires we get the info from path translated.  This can be removed at
	   such a time taht apache nt is fixed */
	else if (GLOBAL(request_info).path_translated) {
		GLOBAL(request_info).script_filename = GLOBAL(request_info).path_translated;
	} else {
		GLOBAL(request_info).script_filename = NULL;
	}
#endif

	/* doc_root configuration variable is currently ignored,
	   as it is with every other access method currently also. */

	/* We always need to emalloc() filename, since it gets placed into
	   the include file hash table, and gets freed with that table.
	   Notice that this means that we don't need to efree() it in
	   php3_destroy_request_info()! */
#if DISCARD_PATH
	if (GLOBAL(request_info).script_filename) {
		GLOBAL(request_info).filename = estrdup(GLOBAL(request_info).script_filename);
	} else {
		GLOBAL(request_info).filename = NULL;
	}
#else
	if (GLOBAL(request_info).path_translated) {
		GLOBAL(request_info).filename = estrdup(GLOBAL(request_info).path_translated);
	} else {
		GLOBAL(request_info).filename = NULL;
	}
#endif
	return SUCCESS;
}

int php3_destroy_request_info(void *conf)
{
	STR_FREE(GLOBAL(request_info).current_user);
	return SUCCESS;
}
#endif

#if FHTTPD
char script_name_resolved_buffer[2048];
const char *method_names[] =
{"Unknown", "GET", "HEAD", "POST", "PUT"};

int php3_init_request_info(void *conf)
{
	static int exit_requested = 0;
	int i, len;
	TLS_VARS;
	req = NULL;

	setalarm(idle_timeout);
	while (checkinput(server->infd) && (req = readrequest(server))) {
		alarm(0);
		if (req->reqtype == FHTTPD_REQUEST) {
			if (headermade)
				php3_fhttpd_free_header();
			response = createresponse(1024, req->id, req->fd, req->ver_major > 0);
			if (response) {
				if (req->script_name && req->script_name_resolved) {
					len = strlen(req->script_name);
					strncpy(script_name_resolved_buffer, req->script_name_resolved, 2047);
					script_name_resolved_buffer[2047] = 0;

					GLOBAL(request_info).path_info = NULL;	/* Not supported */
					GLOBAL(request_info).path_translated = script_name_resolved_buffer;
					GLOBAL(request_info).query_string = req->query_string;
					GLOBAL(request_info).current_user = NULL;
					GLOBAL(request_info).current_user_length = 0;
					GLOBAL(request_info).request_method = method_names[req->method];
					GLOBAL(request_info).script_name = req->script_name;
					GLOBAL(request_info).content_length = req->databuffsize;
					GLOBAL(request_info).content_type = req->content_type;
					GLOBAL(request_info).cookies = NULL;
					for (i = 0; i < req->nlines; i++) {
						if (req->lines[i].paramc > 1) {
							if (req->lines[i].params[0]) {
								if (!strcasecmp(req->lines[i].params[0], "HTTP_COOKIE")) {
									if (req->lines[i].params[1]) {
										GLOBAL(request_info).cookies = req->lines[i].params[1];
									}
								}
							}
						}
					}
					/* doc_root configuration variable is currently ignored,
					   as it is with every other access method currently also. */

					/* We always need to emalloc() filename, since it gets placed into
					   the include file hash table, and gets freed with that table.
					   Notice that this means that we don't need to efree() it in
					   php3_destroy_request_info()! */
					if (GLOBAL(request_info).path_translated)
						GLOBAL(request_info).filename = estrdup(GLOBAL(request_info).path_translated);
					else
						GLOBAL(request_info).filename = NULL;

					return SUCCESS;
				} else {
					deleterequest(req);
					req = NULL;
					setalarm(idle_timeout);
				}
			} else {
				deleterequest(req);
				req = NULL;
				setalarm(idle_timeout);
			}
		} else {
			if (req->reqtype == FHTTPD_EXITOK) {
				exit_status = 1;
				deleterequest(req);
				req = NULL;
				setalarm(idle_timeout);
				return FAILURE;
			}
			deleterequest(req);
			req = NULL;
			setalarm(idle_timeout);
		}
	}
	if (global_alarmflag) {
		if (!exit_requested) {
			requestexit(server, 1);
			exit_requested = 1;
		}
	} else {
	exit_status = 1; 
	}
	return FAILURE;
}
#endif							/* FHTTPD */

#if APACHE
int php3_init_request_info(void *conf)
{
	const char *buf;
	TLS_VARS;

	GLOBAL(request_info).current_user = NULL;
	GLOBAL(request_info).current_user_length = 0;

	GLOBAL(request_info).filename = GLOBAL(php3_rqst)->filename;
	GLOBAL(request_info).request_method = GLOBAL(php3_rqst)->method;
	GLOBAL(request_info).query_string = GLOBAL(php3_rqst)->args;
	GLOBAL(request_info).content_type = table_get(GLOBAL(php3_rqst)->subprocess_env, "CONTENT_TYPE");

	buf = table_get(GLOBAL(php3_rqst)->subprocess_env, "CONTENT_LENGTH");
	GLOBAL(request_info).content_length = (buf ? atoi(buf) : 0);

	GLOBAL(request_info).cookies = table_get(GLOBAL(php3_rqst)->subprocess_env, "HTTP_COOKIE");

	return SUCCESS;
}

#endif

#if USE_SAPI
/* temporary until I figure a beter way to do it */
int php3_init_request_info(void *conf)
{
	TLS_VARS;
	if (GLOBAL(sapi_rqst)->filename)
		GLOBAL(request_info).filename = estrdup(GLOBAL(sapi_rqst)->filename);
	else
		GLOBAL(request_info).filename = NULL;
	GLOBAL(request_info).path_info = GLOBAL(sapi_rqst)->path_info;
	GLOBAL(request_info).path_translated = GLOBAL(sapi_rqst)->path_translated;
	GLOBAL(request_info).query_string = GLOBAL(sapi_rqst)->query_string;
	GLOBAL(request_info).current_user = GLOBAL(sapi_rqst)->current_user;
	GLOBAL(request_info).current_user_length = GLOBAL(sapi_rqst)->current_user_length;
	GLOBAL(request_info).request_method = GLOBAL(sapi_rqst)->request_method;
	GLOBAL(request_info).script_name = GLOBAL(sapi_rqst)->script_name;
	GLOBAL(request_info).content_length = GLOBAL(sapi_rqst)->content_length;
	GLOBAL(request_info).content_type = GLOBAL(sapi_rqst)->content_type;
	GLOBAL(request_info).cookies = GLOBAL(sapi_rqst)->cookies;

	return SUCCESS;
}
#endif

#if !CGI_BINARY
int php3_destroy_request_info(void *conf)
{
	/* see above for why we don't want to efree() request_info.filename */
	return SUCCESS;
}
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
