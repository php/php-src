/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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
   |          Stig Sæther Bakken <ssb@fast.no>                            |
   |          David Sklar <sklar@student.net>                             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php_apache_http.h"

#if defined(PHP_WIN32) || defined(NETWARE)
#include "zend.h"
#include "ap_compat.h"
#else
#include "build-defs.h"
#endif

#ifdef ZTS
int php_apache_info_id;
#else
php_apache_info_struct php_apache_info;
#endif

#define SECTION(name)  PUTS("<h2 class=\"section\">" name "</h2>\n")

extern module *top_module;

PHP_FUNCTION(virtual);
PHP_FUNCTION(apache_request_headers);
PHP_FUNCTION(apache_response_headers);
PHP_FUNCTION(apachelog);
PHP_FUNCTION(apache_note);
PHP_FUNCTION(apache_lookup_uri);
PHP_FUNCTION(apache_child_terminate);
PHP_FUNCTION(apache_setenv);

PHP_MINFO_FUNCTION(apache);

function_entry apache_functions[] = {
	PHP_FE(virtual,									NULL)
	PHP_FE(apache_request_headers,					NULL)
	PHP_FE(apache_note,								NULL)
	PHP_FE(apache_lookup_uri,						NULL)
	PHP_FE(apache_child_terminate,					NULL)
	PHP_FE(apache_setenv,							NULL)
	PHP_FE(apache_response_headers,					NULL)
	PHP_FALIAS(getallheaders, apache_request_headers, NULL)
	{NULL, NULL, NULL}
};


PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("xbithack",			"0",				PHP_INI_ALL,		OnUpdateInt,		xbithack, php_apache_info_struct, php_apache_info)
	STD_PHP_INI_ENTRY("engine",				"1",				PHP_INI_ALL,		OnUpdateInt,		engine, php_apache_info_struct, php_apache_info)
	STD_PHP_INI_ENTRY("last_modified",		"0",				PHP_INI_ALL,		OnUpdateInt,		last_modified, php_apache_info_struct, php_apache_info)
	STD_PHP_INI_ENTRY("child_terminate",	"0",				PHP_INI_ALL,		OnUpdateInt,		terminate_child, php_apache_info_struct, php_apache_info)
PHP_INI_END()



static void php_apache_globals_ctor(php_apache_info_struct *apache_globals TSRMLS_DC)
{
	apache_globals->in_request = 0;
}


static PHP_MINIT_FUNCTION(apache)
{
#ifdef ZTS
	ts_allocate_id(&php_apache_info_id, sizeof(php_apache_info_struct), (ts_allocate_ctor) php_apache_globals_ctor, NULL);
#else
	php_apache_globals_ctor(&php_apache_info TSRMLS_CC);
#endif
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}


static PHP_MSHUTDOWN_FUNCTION(apache)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

zend_module_entry apache_module_entry = {
	STANDARD_MODULE_HEADER,
	"apache", 
	apache_functions, 
	PHP_MINIT(apache), 
	PHP_MSHUTDOWN(apache), 
	NULL, 
	NULL, 
	PHP_MINFO(apache), 
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

/* {{{ proto bool apache_child_terminate(void)
   Terminate apache process after this request */
PHP_FUNCTION(apache_child_terminate)
{
#ifndef MULTITHREAD
	if (AP(terminate_child)) {
		ap_child_terminate( ((request_rec *)SG(server_context)) );
		RETURN_TRUE;
	} else { /* tell them to get lost! */
		php_error(E_WARNING, "apache.child_terminate is disabled");
		RETURN_FALSE;
	}
#else
		php_error(E_WARNING, "apache_child_terminate() is not supported in this build");
		RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto string apache_note(string note_name [, string note_value])
   Get and set Apache request notes */
PHP_FUNCTION(apache_note)
{
	pval **arg_name, **arg_val;
	char *note_val;
	int arg_count = ARG_COUNT(ht);

	if (arg_count<1 || arg_count>2 ||
		zend_get_parameters_ex(arg_count, &arg_name, &arg_val) ==FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(arg_name);
	note_val = (char *) table_get(((request_rec *)SG(server_context))->notes, (*arg_name)->value.str.val);
	
	if (arg_count == 2) {
		convert_to_string_ex(arg_val);
		table_set(((request_rec *)SG(server_context))->notes, (*arg_name)->value.str.val, (*arg_val)->value.str.val);
	}

	if (note_val) {
		RETURN_STRING(note_val, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(apache)
{
	module *modp = NULL;
	char output_buf[128];
#if !defined(WIN32) && !defined(WINNT)
	char name[64];
	char modulenames[1024];
	char *p;
#endif
	server_rec *serv;
	extern char server_root[MAX_STRING_LEN];
	extern uid_t user_id;
	extern char *user_name;
	extern gid_t group_id;
	extern int max_requests_per_child;

	serv = ((request_rec *) SG(server_context))->server;


	php_info_print_table_start();

#ifdef PHP_WIN32
	php_info_print_table_row(1, "Apache for Windows 95/NT");
	php_info_print_table_end();
	php_info_print_table_start();
#elif defined(NETWARE)
	php_info_print_table_row(1, "Apache for NetWare");
	php_info_print_table_end();
	php_info_print_table_start();
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
	sprintf(output_buf, "%s:%u", serv->server_hostname, serv->port);
	php_info_print_table_row(2, "Hostname:Port", output_buf);
#if !defined(WIN32) && !defined(WINNT)
	sprintf(output_buf, "%s(%d)/%d", user_name, (int)user_id, (int)group_id);
	php_info_print_table_row(2, "User/Group", output_buf);
	sprintf(output_buf, "Per Child: %d - Keep Alive: %s - Max Per Connection: %d", max_requests_per_child, serv->keep_alive ? "on":"off", serv->keep_alive_max);
	php_info_print_table_row(2, "Max Requests", output_buf);
#endif
	sprintf(output_buf, "Connection: %d - Keep-Alive: %d", serv->timeout, serv->keep_alive_timeout);
	php_info_print_table_row(2, "Timeouts", output_buf);
#if !defined(WIN32) && !defined(WINNT)
/*
	This block seems to be working on NetWare; But it seems to be showing
	all modules instead of just the loaded ones
*/
	php_info_print_table_row(2, "Server Root", server_root);

	strcpy(modulenames, "");
	for(modp = top_module; modp; modp = modp->next) {
		strlcpy(name, modp->name, sizeof(name));
		if ((p = strrchr(name, '.'))) {
			*p='\0'; /* Cut off ugly .c extensions on module names */
		}
		strcat(modulenames, name);
		if (modp->next) {
			strcat(modulenames, ", ");
		}
	}
	php_info_print_table_row(2, "Loaded Modules", modulenames);
#endif

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

	{
		register int i;
		array_header *arr;
		table_entry *elts;
		request_rec *r;

		r = ((request_rec *) SG(server_context));
		arr = table_elts(r->subprocess_env);
		elts = (table_entry *)arr->elts;
		
		SECTION("Apache Environment");
		php_info_print_table_start();	
		php_info_print_table_header(2, "Variable", "Value");
		for (i=0; i < arr->nelts; i++) {
			php_info_print_table_row(2, elts[i].key, elts[i].val);
		}
		php_info_print_table_end();	
	}

	{
		array_header *env_arr;
		table_entry *env;
		int i;
		request_rec *r;
		
		r = ((request_rec *) SG(server_context));
		SECTION("HTTP Headers Information");
		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "HTTP Request Headers");
		php_info_print_table_row(2, "HTTP Request", r->the_request);
		env_arr = table_elts(r->headers_in);
		env = (table_entry *)env_arr->elts;
		for (i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key && (!PG(safe_mode) || (PG(safe_mode) && strncasecmp(env[i].key, "authorization", 13)))) {
				php_info_print_table_row(2, env[i].key, env[i].val);
			}
		}
		php_info_print_table_colspan_header(2, "HTTP Response Headers");
		env_arr = table_elts(r->headers_out);
		env = (table_entry *)env_arr->elts;
		for(i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key) {
				php_info_print_table_row(2, env[i].key, env[i].val);
			}
		}
		php_info_print_table_end();
	}
}
/* }}} */

/* {{{ proto bool virtual(string filename)
   Perform an Apache sub-request */
/* This function is equivalent to <!--#include virtual...-->
 * in mod_include. It does an Apache sub-request. It is useful
 * for including CGI scripts or .shtml files, or anything else
 * that you'd parse through Apache (for .phtml files, you'd probably
 * want to use <?Include>. This only works when PHP is compiled
 * as an Apache module, since it uses the Apache API for doing
 * sub requests.
 */
PHP_FUNCTION(virtual)
{
	pval **filename;
	request_rec *rr = NULL;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	
	if (!(rr = sub_req_lookup_uri ((*filename)->value.str.val, ((request_rec *) SG(server_context))))) {
		php_error(E_WARNING, "Unable to include '%s' - URI lookup failed", (*filename)->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	}

	if (rr->status != 200) {
		php_error(E_WARNING, "Unable to include '%s' - error finding URI", (*filename)->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	}

	php_end_ob_buffers(1 TSRMLS_CC);
	php_header();

	if (run_sub_req(rr)) {
		php_error(E_WARNING, "Unable to include '%s' - request execution failed", (*filename)->value.str.val);
		if (rr) destroy_sub_req (rr);
		RETURN_FALSE;
	} else {
		if (rr) destroy_sub_req (rr);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto array getallheaders(void)
   Alias for apache_request_headers() */
/* }}} */
/* {{{ proto array apache_request_headers(void)
   Fetch all HTTP request headers */
PHP_FUNCTION(apache_request_headers)
{
    array_header *env_arr;
    table_entry *tenv;
    int i;
	
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
		if (add_assoc_string(return_value, tenv[i].key, (tenv[i].val==NULL) ? "" : tenv[i].val, 1)==FAILURE) {
			RETURN_FALSE;
		}
    }
}
/* }}} */

/* {{{ proto array apache_response_headers(void)
   Fetch all HTTP response headers */
PHP_FUNCTION(apache_response_headers)
{
    array_header *env_arr;
    table_entry *tenv;
    int i;

    if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
    }
    env_arr = table_elts(((request_rec *) SG(server_context))->headers_out);
    tenv = (table_entry *)env_arr->elts;
    for (i = 0; i < env_arr->nelts; ++i) {
		if (!tenv[i].key) continue;
		if (add_assoc_string(return_value, tenv[i].key, (tenv[i].val==NULL) ? "" : tenv[i].val, 1)==FAILURE) {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto bool apache_setenv(string variable, string value [, bool walk_to_top])
   Set an Apache subprocess_env variable */
PHP_FUNCTION(apache_setenv)
{
	int var_len, val_len, top=0;
	char *var = NULL, *val = NULL;
	request_rec *r = (request_rec *) SG(server_context);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &var, &var_len, &val, &val_len, &top) == FAILURE) {
        RETURN_FALSE;
	}

	while(top) {
		if(r->prev) r = r->prev;
		else break;
	}

	ap_table_setn(r->subprocess_env, ap_pstrndup(r->pool, var, var_len), ap_pstrndup(r->pool, val, val_len));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object apache_lookup_uri(string URI)
   Perform a partial request of the given URI to obtain information about it */
PHP_FUNCTION(apache_lookup_uri)
{
	pval **filename;
	request_rec *rr=NULL;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if(!(rr = sub_req_lookup_uri((*filename)->value.str.val, ((request_rec *) SG(server_context))))) {
		php_error(E_WARNING, "URI lookup failed", (*filename)->value.str.val);
		RETURN_FALSE;
	}
	object_init(return_value);
	add_property_long(return_value,"status", rr->status);
	if (rr->the_request) {
		add_property_string(return_value,"the_request", rr->the_request, 1);
	}
	if (rr->status_line) {
		add_property_string(return_value,"status_line", (char *)rr->status_line, 1);		
	}
	if (rr->method) {
		add_property_string(return_value,"method", (char *)rr->method, 1);		
	}
	if (rr->content_type) {
		add_property_string(return_value,"content_type", (char *)rr->content_type, 1);
	}
	if (rr->handler) {
		add_property_string(return_value,"handler", (char *)rr->handler, 1);		
	}
	if (rr->uri) {
		add_property_string(return_value,"uri", rr->uri, 1);
	}
	if (rr->filename) {
		add_property_string(return_value,"filename", rr->filename, 1);
	}
	if (rr->path_info) {
		add_property_string(return_value,"path_info", rr->path_info, 1);
	}
	if (rr->args) {
		add_property_string(return_value,"args", rr->args, 1);
	}
	if (rr->boundary) {
		add_property_string(return_value,"boundary", rr->boundary, 1);
	}
	add_property_long(return_value,"no_cache", rr->no_cache);
	add_property_long(return_value,"no_local_copy", rr->no_local_copy);
	add_property_long(return_value,"allowed", rr->allowed);
	add_property_long(return_value,"sent_bodyct", rr->sent_bodyct);
	add_property_long(return_value,"bytes_sent", rr->bytes_sent);
	add_property_long(return_value,"byterange", rr->byterange);
	add_property_long(return_value,"clength", rr->clength);

#if MODULE_MAGIC_NUMBER >= 19980324
	if (rr->unparsed_uri) {
		add_property_string(return_value,"unparsed_uri", rr->unparsed_uri, 1);
	}
	if(rr->mtime) {
		add_property_long(return_value,"mtime", rr->mtime);
	}
#endif
	if(rr->request_time) {
		add_property_long(return_value,"request_time", rr->request_time);
	}

	destroy_sub_req(rr);
}
/* }}} */


#if 0
This function is most likely a bad idea.  Just playing with it for now.

PHP_FUNCTION(apache_exec_uri)
{
	pval **filename;
	request_rec *rr=NULL;
	TSRMLS_FETCH();

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if(!(rr = ap_sub_req_lookup_uri((*filename)->value.str.val, ((request_rec *) SG(server_context))))) {
		php_error(E_WARNING, "URI lookup failed", (*filename)->value.str.val);
		RETURN_FALSE;
	}
	RETVAL_LONG(ap_run_sub_req(rr));
	ap_destroy_sub_req(rr);
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
