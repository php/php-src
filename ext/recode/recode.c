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
   | Author: Kristian Koehntopp <kris@koehntopp.de>					              |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

/* {{{ includes & prototypes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_recode.h"
#include "php_streams.h"

#if HAVE_LIBRECODE
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "zend_list.h"


#ifdef HAVE_BROKEN_RECODE
extern char *program_name;
char *program_name = "php";
#endif
/* }}} */

#define SAFE_STRING(s) ((s)?(s):"")

php_recode_globals recode_globals;
extern int le_fp,le_pp;

/* {{{ module stuff */
static zend_function_entry php_recode_functions[] = {
	PHP_FE(recode_string, NULL)
	PHP_FE(recode_file, NULL)
	PHP_FALIAS(recode, recode_string, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry recode_module_entry = {
    STANDARD_MODULE_HEADER,
	"recode", 
	php_recode_functions, 
	PHP_MINIT(recode), 
	PHP_MSHUTDOWN(recode), 
	NULL,
	NULL, 
	PHP_MINFO(recode), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#if APACHE
extern void timeout(int sig);
#endif

#ifdef COMPILE_DL_RECODE
ZEND_GET_MODULE(recode)
#endif

PHP_MINIT_FUNCTION(recode)
{
	ReSG(outer)	  = recode_new_outer(true);
	if (ReSG(outer) == NULL)
		return FAILURE;
	
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(recode)
{
	if (ReSG(outer)) {
		recode_delete_outer(ReSG(outer));
	}
	return SUCCESS;
}


PHP_MINFO_FUNCTION(recode)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Recode Support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");
	php_info_print_table_end();

}

/* {{{ proto string recode_string(string request, string str)
   Recode string str according to request string */

PHP_FUNCTION(recode_string)
{
	RECODE_REQUEST request = NULL;
	char *r = NULL;
	pval **str;
	pval **req;
	bool success;
	int r_len=0, r_alen =0;

	if (ZEND_NUM_ARGS() != 2
	 || zend_get_parameters_ex(2, &req, &str) == FAILURE) {
	 	WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_string_ex(req);

	request = recode_new_request(ReSG(outer));
	if (request == NULL) {
		php_error(E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}
	
	success = recode_scan_request(request, Z_STRVAL_PP(req));
	if (!success) {
		php_error(E_WARNING, "Illegal recode request '%s'", Z_STRVAL_PP(req));
		goto error_exit;
	}
	
	recode_buffer_to_buffer(request, Z_STRVAL_PP(str), Z_STRLEN_PP(str), &r, &r_len, &r_alen);
	if (!r) {
		php_error(E_WARNING, "Recoding failed.");
		goto error_exit;
	}
	
	RETVAL_STRINGL(r, r_len, 1);
	free(r);
	/* FALLTHROUGH */

error_exit:
	if (request)
		recode_delete_request(request);

	if (!r)	
		RETURN_FALSE;

	return;
}
/* }}} */

/* {{{ proto bool recode_file(string request, resource input, resource output)
   Recode file input into file output according to request */
PHP_FUNCTION(recode_file)
{
	RECODE_REQUEST request = NULL;
	int success;
	pval **req;
	pval **input, **output;
	php_stream * instream, *outstream;
	FILE  *in_fp,  *out_fp;
	int    in_type, out_type;


	if (ZEND_NUM_ARGS() != 3
	 || zend_get_parameters_ex(3, &req, &input, &output) == FAILURE) {
	 	WRONG_PARAM_COUNT;
	}

	instream = zend_fetch_resource(input TSRMLS_CC,-1, "File-Handle", &in_type, 
		1, php_file_le_stream());

	if (!instream) {
		php_error(E_WARNING,"Unable to find input file identifier");
		RETURN_FALSE;
	}

	if (!php_stream_cast(instream, PHP_STREAM_AS_STDIO, (void**)&in_fp, REPORT_ERRORS))	{
		RETURN_FALSE;
	}
	
	outstream = zend_fetch_resource(output TSRMLS_CC,-1, "File-Handle", &out_type,
		1, php_file_le_stream());
	if (!outstream) {
		php_error(E_WARNING,"Unable to find output file identifier");
		RETURN_FALSE;
	}
	if (!php_stream_cast(outstream, PHP_STREAM_AS_STDIO, (void**)&out_fp, REPORT_ERRORS))	{
		RETURN_FALSE;
	}
	
	convert_to_string_ex(req);

	request = recode_new_request(ReSG(outer));
	if (request == NULL) {
		php_error(E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}

	success = recode_scan_request(request, Z_STRVAL_PP(req));
	if (!success) {
		php_error(E_WARNING, "Illegal recode request '%s'", Z_STRVAL_PP(req));
		goto error_exit;
	}
	
	success = recode_file_to_file(request, in_fp, out_fp);
	if (!success) {
		php_error(E_WARNING, "Recoding failed.");
		goto error_exit;
	}

	if (request)
		recode_delete_request(request);
	RETURN_TRUE;

error_exit:
	if (request)
		recode_delete_request(request);
	
	RETURN_FALSE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
