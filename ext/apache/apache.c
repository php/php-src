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
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   |          David Sklar <sklar@student.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#include "php.h"
#include "ext/standard/head.h"
#include "php_globals.h"
#include "php_ini.h"
#include "SAPI.h"
#include "mod_php3.h"
#include "ext/standard/info.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>
#include <ctype.h>

#if APACHE
#include "http_request.h"
#include "build-defs.h"

extern module *top_module;

void php3_virtual(INTERNAL_FUNCTION_PARAMETERS);
void php3_getallheaders(INTERNAL_FUNCTION_PARAMETERS);
void php3_apachelog(INTERNAL_FUNCTION_PARAMETERS);
void php3_info_apache(ZEND_MODULE_INFO_FUNC_ARGS);
void php3_apache_note(INTERNAL_FUNCTION_PARAMETERS);
void php3_apache_lookup_uri(INTERNAL_FUNCTION_PARAMETERS);

function_entry apache_functions[] = {
	{"virtual",			php3_virtual,		NULL},
	{"getallheaders",		php3_getallheaders,	NULL},
	{"apache_note", php3_apache_note,NULL},
	{"apache_lookup_uri", php3_apache_lookup_uri,NULL},
	{NULL, NULL, NULL}
};


static PHP_INI_MH(OnChangeApacheInt)
{
	long *p;
	char *base = (char *) &php_apache_info;

	p = (long *) (base+(size_t) mh_arg1);

	if (new_value) {
		*p = atoi(new_value);
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


static PHP_INI_MH(OnChangeApacheString)
{
	char **p;
	char *base = (char *) &php_apache_info;

	p = (char **) (base+(size_t) mh_arg1);

	if (new_value) {
		*p = new_value;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY1("xbithack",			"0",				PHP_INI_ALL,		OnChangeApacheInt,		(void *) XtOffsetOf(php_apache_info_struct, xbithack))
	PHP_INI_ENTRY1("engine",				"1",				PHP_INI_ALL,		OnChangeApacheInt,		(void *) XtOffsetOf(php_apache_info_struct, engine))
	PHP_INI_ENTRY1("last_modified",		"0",				PHP_INI_ALL,		OnChangeApacheInt,		(void *) XtOffsetOf(php_apache_info_struct, last_modified))
	PHP_INI_ENTRY1("dav_script",			NULL,				PHP_INI_ALL,		OnChangeApacheString,	(void *) XtOffsetOf(php_apache_info_struct, dav_script))
PHP_INI_END()


static int php_apache_minit(INIT_FUNC_ARGS)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}


static int php_apache_mshutdown(SHUTDOWN_FUNC_ARGS)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}


php3_module_entry apache_module_entry = {
	"Apache", apache_functions, php_apache_minit, php_apache_mshutdown, NULL, NULL, php3_info_apache, STANDARD_MODULE_PROPERTIES
};

/* {{{ proto string apache_note(string note_name [, string note_value])
   Get and set Apache request notes */
void php3_apache_note(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_name,*arg_val;
	char *note_val;
	int arg_count = ARG_COUNT(ht);
	SLS_FETCH();

	if (arg_count<1 || arg_count>2 ||
		getParameters(ht,arg_count,&arg_name,&arg_val) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(arg_name);
	note_val = (char *) table_get(((request_rec *) SG(server_context))->notes,arg_name->value.str.val);
	
	if (arg_count == 2) {
		convert_to_string(arg_val);
		table_set(((request_rec *) SG(server_context))->notes,arg_name->value.str.val,arg_val->value.str.val);
	}

	if (note_val) {
		RETURN_STRING(note_val,1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void php3_info_apache(ZEND_MODULE_INFO_FUNC_ARGS)
{
	module *modp = NULL;
	char output_buf[128];
#if !defined(WIN32) && !defined(WINNT)
	char name[64];
	char *p;
#endif
	server_rec *serv;
	extern char server_root[MAX_STRING_LEN];
	extern uid_t user_id;
	extern char *user_name;
	extern gid_t group_id;
	extern int max_requests_per_child;
	SLS_FETCH();

	serv = ((request_rec *) SG(server_context))->server;
	
    PUTS("<table border=5 width=\"600\">\n");
    php_info_print_table_header(2, "Entry", "Value");
#if WIN32|WINNT
	PUTS("Apache for Windows 95/NT<br>");
#else
	php_info_print_table_row(2, "APACHE_INCLUDE", PHP_APACHE_INCLUDE);
	php_info_print_table_row(2, "APACHE_TARGET", PHP_APACHE_TARGET);
#endif
	php_info_print_table_row(2, "Apache Version", SERVER_VERSION);
#ifdef APACHE_RELEASE
	sprintf(output_buf, "%d", APACHE_RELEASE);
	php_info_print_table_row(2, "Apache Release", output_buf);
#endif
	sprintf(output_buf, "%d", MODULE_MAGIC_NUMBER);
	php_info_print_table_row(2, "Apache API Version", output_buf);
	sprintf(output_buf, "%s:%u", serv->server_hostname,serv->port);
	php_info_print_table_row(2, "Hostname/Port", output_buf);
#if !defined(WIN32) && !defined(WINNT)
	sprintf(output_buf, "%s(%d)/%d", user_name,(int)user_id,(int)group_id);
	php_info_print_table_row(2, "User/Group", output_buf);
	sprintf(output_buf, "per child: %d<br>keep alive: %s<br>max per connection: %d",max_requests_per_child,serv->keep_alive ? "on":"off", serv->keep_alive_max);
	php_info_print_table_row(2, "Max Requests", output_buf);
#endif
	sprintf(output_buf, "connection: %d<br>keep-alive: %d",serv->timeout,serv->keep_alive_timeout);
	php_info_print_table_row(2, "Timeouts", output_buf);
#if !defined(WIN32) && !defined(WINNT)
	php_info_print_table_row(2, "Server Root", server_root);

	
	PUTS("<tr><td valign=\"top\" bgcolor=\"" PHP_ENTRY_NAME_COLOR "\">Loaded modules</td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">");
	for(modp = top_module; modp; modp = modp->next) {
		strncpy(name, modp->name, sizeof(name) - 1);
		if ((p = strrchr(name, '.'))) {
			*p='\0'; /* Cut off ugly .c extensions on module names */
		}
		PUTS(name);
		if (modp->next) {
			PUTS(", ");
		}
	}
#endif
	PUTS("</td></tr>\n");
	PUTS("</table>\n");
}

/* This function is equivalent to <!--#include virtual...-->
 * in mod_include. It does an Apache sub-request. It is useful
 * for including CGI scripts or .shtml files, or anything else
 * that you'd parse through Apache (for .phtml files, you'd probably
 * want to use <?Include>. This only works when PHP is compiled
 * as an Apache module, since it uses the Apache API for doing
 * sub requests.
 */
/* {{{ proto int virtual(string filename)
   Perform an Apache sub-request */
void php3_virtual(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *filename;
	request_rec *rr = NULL;
	SLS_FETCH();

	if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	
	if (!(rr = sub_req_lookup_uri (filename->value.str.val, ((request_rec *) SG(server_context))))) {
		php3_error(E_WARNING, "Unable to include '%s' - URI lookup failed", filename->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	}

	if (rr->status != 200) {
		php3_error(E_WARNING, "Unable to include '%s' - error finding URI", filename->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	}

	/* Cannot include another PHP file because of global conflicts */
	if (rr->content_type &&
		!strcmp(rr->content_type, PHP3_MIME_TYPE)) {
		php3_error(E_WARNING, "Cannot include a PHP file "
			  "(use <code>&lt;?include \"%s\"&gt;</code> instead)", filename->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	}

	if (run_sub_req(rr)) {
		php3_error(E_WARNING, "Unable to include '%s' - request execution failed", filename->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	} else {
		if (rr) destroy_sub_req (rr);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto array getallheaders(void)
   Fetch all HTTP request headers */
void php3_getallheaders(INTERNAL_FUNCTION_PARAMETERS)
{
    array_header *env_arr;
    table_entry *tenv;
    int i;
    SLS_FETCH();
	
    if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
    }
    env_arr = table_elts(((request_rec *) SG(server_context))->headers_in);
    tenv = (table_entry *)env_arr->elts;
    for (i = 0; i < env_arr->nelts; ++i) {
		if (!tenv[i].key ||
			(PG(safe_mode) &&
			 !strncasecmp(tenv[i].key, "authorization", 13))) {
			continue;
		}
		if (add_assoc_string(return_value, tenv[i].key,(tenv[i].val==NULL) ? "" : tenv[i].val, 1)==FAILURE) {
			RETURN_FALSE;
		}
    }
}
/* }}} */

/* {{{ proto class apache_lookup_uri(string URI)
   Perform a partial request of the given URI to obtain information about it */
void php3_apache_lookup_uri(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *filename;
	request_rec *rr=NULL;
	SLS_FETCH();

	if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	if(!(rr = sub_req_lookup_uri(filename->value.str.val, ((request_rec *) SG(server_context))))) {
		php3_error(E_WARNING, "URI lookup failed", filename->value.str.val);
		RETURN_FALSE;
	}
	object_init(return_value);
	add_property_long(return_value,"status",rr->status);
	if (rr->the_request) {
		add_property_string(return_value,"the_request",rr->the_request,1);
	}
	if (rr->status_line) {
		add_property_string(return_value,"status_line",rr->status_line,1);		
	}
	if (rr->method) {
		add_property_string(return_value,"method",rr->method,1);		
	}
	if (rr->content_type) {
		add_property_string(return_value,"content_type",(char *)rr->content_type,1);
	}
	if (rr->handler) {
		add_property_string(return_value,"handler",(char *)rr->handler,1);		
	}
	if (rr->uri) {
		add_property_string(return_value,"uri",rr->uri,1);
	}
	if (rr->filename) {
		add_property_string(return_value,"filename",rr->filename,1);
	}
	if (rr->path_info) {
		add_property_string(return_value,"path_info",rr->path_info,1);
	}
	if (rr->args) {
		add_property_string(return_value,"args",rr->args,1);
	}
	if (rr->boundary) {
		add_property_string(return_value,"boundary",rr->boundary,1);
	}
	add_property_long(return_value,"no_cache",rr->no_cache);
	add_property_long(return_value,"no_local_copy",rr->no_local_copy);
	add_property_long(return_value,"allowed",rr->allowed);
	add_property_long(return_value,"sent_bodyct",rr->sent_bodyct);
	add_property_long(return_value,"bytes_sent",rr->bytes_sent);
	add_property_long(return_value,"byterange",rr->byterange);
	add_property_long(return_value,"clength",rr->clength);

#if MODULE_MAGIC_NUMBER >= 19980324
	if (rr->unparsed_uri) {
		add_property_string(return_value,"unparsed_uri",rr->unparsed_uri,1);
	}
	if(rr->mtime) {
		add_property_long(return_value,"mtime",rr->mtime);
	}
#endif
	if(rr->request_time) {
		add_property_long(return_value,"request_time",rr->request_time);
	}

	destroy_sub_req(rr);
}
/* }}} */

#if 0
This function is most likely a bad idea.  Just playing with it for now.

void php3_apache_exec_uri(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *filename;
	request_rec *rr=NULL;
	SLS_FETCH();

	if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	if(!(rr = ap_sub_req_lookup_uri(filename->value.str.val, ((request_rec *) SG(server_context))))) {
		php3_error(E_WARNING, "URI lookup failed", filename->value.str.val);
		RETURN_FALSE;
	}
	RETVAL_LONG(ap_run_sub_req(rr));
	ap_destroy_sub_req(rr);
}
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
