/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#include "php_apache.h"

static request_rec *php_lookup_uri(INTERNAL_FUNCTION_PARAMETERS)
{
	zval **p1;
	php_struct *ctx;
	SLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &p1) == FAILURE)
		return NULL;

	convert_to_string_ex(p1);

	ctx = SG(server_context);
	return ap_sub_req_lookup_uri(Z_STRVAL_PP(p1), ctx->f->r);
}

PHP_FUNCTION(apache_sub_req)
{
	request_rec *rr;

	rr = php_lookup_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	if (!rr)
		WRONG_NUM_ARGS;
	
	if (rr->status == HTTP_OK) {
		ap_run_sub_req(rr);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

#define ADD_LONG(name) \
		add_assoc_long(return_value, #name, rr->name)
#define ADD_STRING(name) \
		add_assoc_string(return_value, #name, rr->name, 1)

PHP_FUNCTION(apache_lookup_uri)
{
	request_rec *rr;

	rr = php_apache_lookup_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (!rr)
		WRONG_NUM_ARGS;
	
	if (rr->status == HTTP_OK) {
		array_init(return_value);

		ADD_LONG(status);
		ADD_STRING(the_request);
		ADD_STRING(status_line);
		ADD_STRING(method);
		ADD_LONG(mtime);
		ADD_LONG(clength);
		ADD_STRING(boundary);
		ADD_STRING(range);
		ADD_LONG(byterange);
		ADD_LONG(chunked);
		ADD_STRING(content_type);
		ADD_STRING(handler);
		ADD_LONG(no_cache);
		ADD_LONG(no_local_copy);
		ADD_STRING(unparsed_uri);
		ADD_STRING(uri);
		ADD_STRING(filename);
		ADD_STRING(path_info);
		ADD_STRING(args);

		return;
	}
	RETURN_FALSE;
}
