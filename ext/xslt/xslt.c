/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_xslt.h"

#if HAVE_XSLT

#include <string.h>

static int debug = 0;

/* {{{ xslt_debug() 
   Output a debug message if debugging is enabled */
extern void xslt_debug(char *function_name, char *format, ...)
{
	if (debug) {
		va_list argv;
		char    buffer[1024];

		va_start(argv, format);
		vsnprintf(buffer, sizeof(buffer) - 1, format, argv);
		va_end(argv);

		buffer[sizeof(buffer) - 1] = '\0';

		php_printf("<b>XSLT Debug</b>: %s: %s<br>\n", 
		           function_name, buffer);
	}
}
/* }}} */

static char *find_xslt_argument(const char **argv, const char *key)
{
	char  **ptr;
	char   *return_value;

	ptr = (char **) argv;
	while (ptr && *ptr) {
		if (! strcmp(*ptr, key)) {
			return_value = estrdup(*ptr);
			return return_value;
		}

		ptr++;
	}

	if (! return_value) {
		return NULL;
	}
}

/* {{{ parse_xslt_arguments()
   Parse an XSLT argument buffer */
extern xslt_args *parse_xslt_arguments(char *xml, 
                                       char *xsl, 
									   char *result, 
									   char **argv)
{
	xslt_args *return_value;

	return_value = emalloc(sizeof(xslt_args));

	/* The xml argument */
	if (! strncasecmp(xml, "arg:", 4)) {
		char *key = xml + 5;

		return_value->xml.type = XSLT_IS_DATA;
		return_value->xml.ptr  = find_xslt_argument((const char **) argv, 
		                                            (const char *)  key);
	}
	else {
		return_value->xml.type = XSLT_IS_FILE;
		return_value->xml.ptr  = estrdup(xml);
	}

	/* The xslt arguments */
	if (! strncasecmp(xsl, "arg:", 4)) {
		char *key = xsl + 5;

		return_value->xsl.type = XSLT_IS_DATA;
		return_value->xsl.ptr  = find_xslt_argument((const char **) argv, 
		                                            (const char *)  key);
	}
	else {
		return_value->xsl.type = XSLT_IS_FILE;
		return_value->xsl.ptr  = estrdup(xsl);
	}

	/* The result argument */
	if (! strncasecmp(result, "arg:", 4)) {
		char *key = result + 5;

		return_value->result.type = XSLT_IS_DATA;
		return_value->result.ptr  = find_xslt_argument((const char **) argv, 
		                                               (const char *)  key);
	}
	else {
		return_value->result.type = XSLT_IS_FILE;
		return_value->result.ptr  = estrdup(result);
	}

	return return_value;
}
/* }}} */

/* {{{ call_xslt_function()
   Call an XSLT handler */
extern void call_xslt_function(char *name, 
                               struct xslt_function *fptr, 
                               int argc, 
                               zval **argv, 
                               zval **retval)
{
	int    error;  /* Error container */
	int    idx;    /* Idx, when looping through and free'ing the arguments */
	ELS_FETCH();   /* For TS mode, fetch the executor globals */

	/* Allocate and initialize return value from the function */
	MAKE_STD_ZVAL(*retval);

	/* Call the function */
	error = call_user_function(EG(function_table),
	                           XSLT_OBJ(fptr),
							   XSLT_FUNC(fptr),
							   *retval, argc, argv);
	if (error == FAILURE) {
		php_error(E_WARNING, "Cannot call the %s handler: %s", 
		          name, Z_STRVAL_P(XSLT_FUNC(fptr)));
	}

	/* Cleanup arguments */
	for (idx = 0; idx < argc; idx++) {
		/* Decrease refcount and free if refcount is <= 0 */
		zval_ptr_dtor(&argv[idx]);
	}
}
/* }}} */


extern void free_xslt_handler(struct xslt_function *func)
{
	if (!func) {
		return;
	}

	if (func->obj) {
		efree(func->obj);
	}

	if (func->func) {
		efree(func->func);
	}

	efree(func);
}

extern void assign_xslt_handler(struct xslt_function **func, zval **zfunc)
{
	char error[] = "Invalid function passed to %s";

	*func = emalloc(sizeof(struct xslt_function));

	if (Z_TYPE_PP(zfunc) == IS_STRING) {
		(*func)->obj = NULL;
		
		zval_add_ref(zfunc);
		(*func)->func = *zfunc;
	}
	else if (Z_TYPE_PP(zfunc) == IS_ARRAY) {
		zval **obj;
		zval **function;

		if (zend_hash_index_find(Z_ARRVAL_PP(zfunc), 0, (void **) &obj) == FAILURE) {
			efree(*func);
			php_error(E_WARNING, error, get_active_function_name());
			return;
		}

		if (zend_hash_index_find(Z_ARRVAL_PP(zfunc), 1, (void **) &function) == FAILURE) {
			efree(*func);
			php_error(E_WARNING, error, get_active_function_name());
			return;
		}

		zval_add_ref(obj);
		zval_add_ref(function);

		(*func)->obj  = *obj;
		(*func)->func = *function;
	}
	else {
		efree(*func);
		php_error(E_WARNING, error, get_active_function_name());
	}
}

#endif
