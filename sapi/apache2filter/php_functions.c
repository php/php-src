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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "SAPI.h"

#include "apr_strings.h"
#include "apr_time.h"
#include "ap_config.h"
#include "util_filter.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
#include "util_script.h"
#include "http_core.h"

#include "php_apache.h"

static request_rec *php_apache_lookup_uri(INTERNAL_FUNCTION_PARAMETERS)
{
	zval **p1;
	php_struct *ctx;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &p1) == FAILURE)
		return NULL;

	convert_to_string_ex(p1);

	ctx = SG(server_context);
	return ap_sub_req_lookup_uri(Z_STRVAL_PP(p1), ctx->f->r, ctx->f->next);
}

/* {{{ proto bool virtual(string uri)
 Perform an apache sub-request */
PHP_FUNCTION(virtual)
{
	request_rec *rr;

	rr = php_apache_lookup_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	if (!rr)
		WRONG_PARAM_COUNT;
	
	if (rr->status == HTTP_OK) {
		ap_run_sub_req(rr);
		ap_destroy_sub_req(rr);
		RETURN_TRUE;
	}
	ap_destroy_sub_req(rr);
	RETURN_FALSE;
}

#define ADD_LONG(name) \
		add_property_long(return_value, #name, rr->name)
#define ADD_TIME(name) \
		add_property_long(return_value, #name, rr->name / APR_USEC_PER_SEC);
#define ADD_STRING(name) \
		if (rr->name) add_property_string(return_value, #name, (char *) rr->name, 1)

PHP_FUNCTION(apache_lookup_uri)
{
	request_rec *rr;

	rr = php_apache_lookup_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (!rr)
		WRONG_PARAM_COUNT;
	
	if (rr->status == HTTP_OK) {
		object_init(return_value);

		ADD_LONG(status);
		ADD_STRING(the_request);
		ADD_STRING(status_line);
		ADD_STRING(method);
		ADD_TIME(mtime);
		ADD_LONG(clength);
#if MODULE_MAGIC_NUMBER < 20020506
		ADD_STRING(boundary);
#endif
		ADD_STRING(range);
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
		ADD_LONG(allowed);
		ADD_LONG(sent_bodyct);
		ADD_LONG(bytes_sent);
		ADD_LONG(request_time);
		ADD_LONG(mtime);
		ADD_TIME(request_time);

		ap_destroy_sub_req(rr);
		return;
	}
	ap_destroy_sub_req(rr);
	RETURN_FALSE;
}

/* {{{ proto array getallheaders(void)
   Fetch all HTTP request headers */
PHP_FUNCTION(getallheaders)
{
	php_struct *ctx;
	const apr_array_header_t *arr;
	char *key, *val;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	ctx = SG(server_context);
	arr = apr_table_elts(ctx->f->r->headers_in);

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) val = empty_string;
		add_assoc_string(return_value, key, val, 1);
	APR_ARRAY_FOREACH_CLOSE()
}
/* }}} */

PHP_MINFO_FUNCTION(apache)
{
}

static function_entry apache_functions[] = {
	PHP_FE(apache_lookup_uri, NULL)
	PHP_FE(virtual, NULL)
	PHP_FE(getallheaders, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_apache_module = {
	STANDARD_MODULE_HEADER,
	"Apache 2.0",
	apache_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(apache),
	NULL,
	STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
