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

#include "php.h"
#include "SAPI.h"

#ifndef THREAD_SAFE
PHPAPI php3_request_info request_info;
#endif

#if CGI_BINARY
int php3_init_request_info(void *conf)
{
	char *buf;	/* temporary buffers */
	SLS_FETCH();

	request_info.path_info = getenv("PATH_INFO");
	request_info.path_translated = getenv("PATH_TRANSLATED");
	request_info.current_user = NULL;
	request_info.current_user_length = 0;
	request_info.request_method = getenv("REQUEST_METHOD");
	request_info.script_name = getenv("SCRIPT_NAME");
	buf = getenv("CONTENT_LENGTH");
	request_info.content_length = (buf ? atoi(buf) : 0);
	request_info.content_type = getenv("CONTENT_TYPE");
	request_info.cookies = getenv("HTTP_COOKIE");
	request_info.script_filename = getenv("SCRIPT_FILENAME");
	/* Hack for annoying servers that do not set SCRIPT_FILENAME for us */
	if (!request_info.script_filename) {
		request_info.script_filename = request_info.php_argv0;
	}
#if WIN32|WINNT
	/* FIXME WHEN APACHE NT IS FIXED */
	/* a hack for apache nt because it does not appear to set argv[1] and sets
	   script filename to php.exe thus makes us parse php.exe instead of file.php
	   requires we get the info from path translated.  This can be removed at
	   such a time taht apache nt is fixed */
	else if (request_info.path_translated) {
		request_info.script_filename = request_info.path_translated;
	} else {
		request_info.script_filename = NULL;
	}
#endif

	/* doc_root configuration variable is currently ignored,
	   as it is with every other access method currently also. */

	/* We always need to emalloc() filename, since it gets placed into
	   the include file hash table, and gets freed with that table.
	   Notice that this means that we don't need to efree() it in
	   php3_destroy_request_info()! */
#if DISCARD_PATH
	if (request_info.script_filename) {
		request_info.filename = estrdup(request_info.script_filename);
	} else {
		request_info.filename = NULL;
	}
#else
	if (request_info.path_translated) {
		request_info.filename = estrdup(request_info.path_translated);
	} else {
		request_info.filename = NULL;
	}
#endif
	return SUCCESS;
}

int php3_destroy_request_info(void *conf)
{
	STR_FREE(request_info.current_user);
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

					request_info.path_info = NULL;	/* Not supported */
					request_info.path_translated = script_name_resolved_buffer;
					request_info.query_string = req->query_string;
					request_info.current_user = NULL;
					request_info.current_user_length = 0;
					request_info.request_method = method_names[req->method];
					request_info.script_name = req->script_name;
					request_info.content_length = req->databuffsize;
					request_info.content_type = req->content_type;
					request_info.cookies = NULL;
					for (i = 0; i < req->nlines; i++) {
						if (req->lines[i].paramc > 1) {
							if (req->lines[i].params[0]) {
								if (!strcasecmp(req->lines[i].params[0], "HTTP_COOKIE")) {
									if (req->lines[i].params[1]) {
										request_info.cookies = req->lines[i].params[1];
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
					if (request_info.path_translated)
						request_info.filename = estrdup(request_info.path_translated);
					else
						request_info.filename = NULL;

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
	request_rec *r;
	SLS_FETCH();

	r = ((request_rec *) SG(server_context));
	SG(request_info).query_string = r->args;
	request_info.current_user = NULL;
	request_info.current_user_length = 0;

	request_info.filename = r->filename;
	request_info.request_method = r->method;
	request_info.content_type = table_get(r->subprocess_env, "CONTENT_TYPE");

	buf = table_get(r->subprocess_env, "CONTENT_LENGTH");
	request_info.content_length = (buf ? atoi(buf) : 0);

	request_info.cookies = table_get(r->subprocess_env, "HTTP_COOKIE");

	return SUCCESS;
}

#endif

#if USE_SAPI
/* temporary until I figure a beter way to do it */
int php3_init_request_info(void *conf)
{
	if (sapi_rqst->filename)
		request_info.filename = estrdup(sapi_rqst->filename);
	else
		request_info.filename = NULL;
	request_info.path_info = sapi_rqst->path_info;
	request_info.path_translated = sapi_rqst->path_translated;
	request_info.current_user = sapi_rqst->current_user;
	request_info.current_user_length = sapi_rqst->current_user_length;
	request_info.request_method = sapi_rqst->request_method;
	request_info.script_name = sapi_rqst->script_name;
	request_info.content_length = sapi_rqst->content_length;
	request_info.content_type = sapi_rqst->content_type;
	request_info.cookies = sapi_rqst->cookies;

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
