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

/* {{{ find_xslt_argument()
   Find and return an xslt argument from the argument buffer */
static char *_find_xslt_argument(const char **argv, const char *key)
{
	char  **ptr;                  /* Pointer to the passed char ** array */
	char   *return_value = NULL;  /* Value to return from the function */

	/* Loop through the array searching for the value */
	ptr = (char **) argv;
	while (ptr && *ptr) {
		/* If we have a match, save the value and exit out */
		if (! strcmp(*ptr, key)) {
			return_value = estrdup(*ptr);
			break;
		}

		ptr++;
	}

	return return_value;
}
/* }}} */

/* {{{ xslt_make_array() 
   Make an XSLT array (char **) from a zval array (HashTable *) */
extern void xslt_make_array(zval **zarr, char ***carr)
{
	zval      **current;
	HashTable  *arr;
	int         idx = 0;

	arr = HASH_OF(*zarr);
	if (! arr) {
		php_error(E_WARNING, "Invalid argument or parameter array to %s",
		          get_active_function_name());
		return;
	}

	*carr = emalloc((zend_hash_num_elements(arr) * 2) + 1);
	
	for (zend_hash_internal_pointer_reset(arr);
	     zend_hash_get_current_data(arr, (void **) &current) == SUCCESS;
	     zend_hash_move_forward(arr)) {
		char  *string_key = NULL;
		ulong  num_key;		
		int    type;

		SEPARATE_ZVAL(current);
		convert_to_string_ex(current);

		type = zend_hash_get_current_key(arr, &string_key, &num_key, 0);
		if (type == HASH_KEY_IS_LONG) {
			php_error(E_WARNING, "Invalid argument or parameter array to %s",
			          get_active_function_name());
			return;
		}

		(*carr)[idx++] = estrdup(string_key);
		(*carr)[idx++] = estrndup(Z_STRVAL_PP(current), Z_STRLEN_PP(current));
	}

	(*carr)[idx] = NULL;
}
/* }}} */

/* {{{ xslt_free_array()
   Free an xslt array built by xslt_make_array() */
extern void xslt_free_array(char **arr)
{
	char **ptr = arr;

	while (*ptr != NULL) {
		efree(*ptr);
		ptr++;
	}

	efree(arr);
}
/* }}} */

/* {{{ xslt_parse_arguments()
   Parse an XSLT argument buffer */
extern xslt_args *xslt_parse_arguments(char  *xml, 
                                       char  *xsl, 
                                       char  *result, 
                                       char **argv)
{
	xslt_args *return_value;  /* The value to return from the function */

	return_value = emalloc(sizeof(xslt_args));

	/* The xml argument */
	if (! strncasecmp(xml, "arg:", 4)) {
		char *key = xml + 5;

		return_value->xml.type = XSLT_IS_DATA;
		return_value->xml.ptr  = _find_xslt_argument((const char **) argv, 
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
		return_value->xsl.ptr  = _find_xslt_argument((const char **) argv, 
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
		return_value->result.ptr  = _find_xslt_argument((const char **) argv, 
		                                                (const char *)  key);
	}
	else {
		return_value->result.type = XSLT_IS_FILE;
		return_value->result.ptr  = estrdup(result);
	}

	return return_value;
}
/* }}} */

/* {{{ free_xslt_arguments()
   Free's an XSLT argument list returned from parse_xslt_arguments() */
extern void xslt_free_arguments(xslt_args *to_free)
{
	if (to_free->xml.ptr) {
		efree(to_free->xml.ptr);
	}
	
	if (to_free->xsl.ptr) {
		efree(to_free->xsl.ptr);
	}
	
	if (to_free->result.ptr) {
		efree(to_free->result.ptr);
	}
	
	efree(to_free);
}
/* }}} */

/* {{{ call_xslt_function()
   Call an XSLT handler */
extern void xslt_call_function(char *name, 
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


extern void xslt_free_handler(struct xslt_function *func)
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

extern void xslt_assign_handler(struct xslt_function **func, zval **zfunc)
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
