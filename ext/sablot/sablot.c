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
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_SABLOT

/* Sablotron Includes */
#include <sablot.h>

/* Standard Includes */
#include <string.h>

/* PHP Includes */
#include "ext/standard/info.h"
#include "ext/standard/php_output.h"
#include "php_sablot.h"

/* Functions related to PHP's list handling */
static void _php_sablot_free_processor(zend_rsrc_list_entry *rsrc);

/* SAX Handlers */
static void _php_sablot_handler_pair(php_sablot *, zval **, zval **, zval **);
static void _php_sablot_call_handler_function(zval *, int, zval **, char *);
static SAX_RETURN _php_sablot_sax_startDoc(void *);
static SAX_RETURN _php_sablot_sax_startElement(void *, const char *, const char **);
static SAX_RETURN _php_sablot_sax_endElement(void *, const char *);
static SAX_RETURN _php_sablot_sax_startNamespace(void *, const char *, const char *);
static SAX_RETURN _php_sablot_sax_endNamespace(void *, const char *);
static SAX_RETURN _php_sablot_sax_comment(void *, const char *);
static SAX_RETURN _php_sablot_sax_PI(void *, const char *, const char *);
static SAX_RETURN _php_sablot_sax_characters(void *, const char *, int);
static SAX_RETURN _php_sablot_sax_endDoc(void *);

/* Error Handling Functions */
static MH_ERROR _php_sablot_make_code(void *, SablotHandle, int, unsigned short, unsigned short);
static MH_ERROR _php_sablot_error(void *, SablotHandle, MH_ERROR, MH_LEVEL, char **);

/* PHP Utility Functions */
static void _php_sablot_ht_char(HashTable *, char **);
static zval *_php_sablot_string_zval(const char *);
static zval *_php_sablot_resource_zval(long);

/* Free macros */
#define S_FREE(__var) if (__var) efree(__var);
#define FUNCH_FREE(__var) if (__var) zval_del_ref(&(__var));		

/* ERROR Macro */
#define SEND_ERROR_INFO(__error, __errlevel) \
	php_printf("A %s has occurred, sending all relevant information\n", __error); \
	handle->errors = handle->errors_start.next; \
	while (handle->errors) { \
		php_printf("%s: %s\n", handle->errors->key, handle->errors->value); \
		handle->errors = handle->errors->next; \
	} \
	php_error(__errlevel, "");


static SAXHandler sax = {
       _php_sablot_sax_startDoc,
       _php_sablot_sax_startElement,
       _php_sablot_sax_endElement,
       _php_sablot_sax_startNamespace,
       _php_sablot_sax_endNamespace,
       _php_sablot_sax_comment,
       _php_sablot_sax_PI,
       _php_sablot_sax_characters,
       _php_sablot_sax_endDoc
};

static MessageHandler mh = {
	_php_sablot_make_code,
	_php_sablot_error,
	_php_sablot_error
};

/* {{{ Begin PHP Extension code */

#ifdef ZTS
int sablot_globals_id;
#else
php_sablot_globals sablot_globals;
#endif


static unsigned char sixth_arg_force_ref[] = { 6, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char third_arg_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_NONE };

function_entry sablot_functions[] = {
	PHP_FE(xslt_output_begintransform,    NULL)
	PHP_FE(xslt_output_endtransform,      NULL)
	PHP_FE(xslt_transform,                sixth_arg_force_ref)
	PHP_FE(xslt_process,                  third_arg_force_ref)
	PHP_FE(xslt_create,                   NULL)
	PHP_FE(xslt_run,                      NULL)
	PHP_FE(xslt_fetch_result,             NULL)
	PHP_FE(xslt_openlog,                  NULL)
	PHP_FE(xslt_closelog,                 NULL)
	PHP_FE(xslt_set_sax_handler,          NULL)
	PHP_FE(xslt_free,                     NULL)
	PHP_FE(xslt_error,                    NULL)
	PHP_FE(xslt_errno,                    NULL)
	{NULL, NULL, NULL}
};

zend_module_entry sablot_module_entry = {
	"sablot",
	sablot_functions,
	PHP_MINIT(sablot),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sablot),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SABLOT
ZEND_GET_MODULE(sablot)
#endif

/* }}} */


/* {{{ MINIT and MINFO Functions */
PHP_MINIT_FUNCTION(sablot)
{
	SABLOTLS_FETCH();
	SABLOTG(le_sablot) = zend_register_list_destructors_ex(_php_sablot_free_processor, NULL, "sablotron", module_number);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(sablot)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Sablotron support", "enabled");
	php_info_print_table_end();

}
/* }}} */

/* }}} */

/* {{{ Begin Extension function */

/* {{{ Begin Output Transformation functions */

/* {{{ void xslt_output_begintransform(string file)
   Begin filtering of all data that is being printed out through the XSL file given by the file parameter. */
PHP_FUNCTION(xslt_output_begintransform)
{
	zval **file;
	SABLOTLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(file);
	
	S_FREE(SABLOTG(output_transform_file));
		
	SABLOTG(output_transform_file) = estrndup(Z_STRVAL_PP(file), Z_STRLEN_PP(file));
	php_start_ob_buffer(NULL);
}
/* }}} */

/* {{{ void xslt_output_endtranform(void)
   End filtering that data through the XSL file set by xslt_output_transform() and output the data */
PHP_FUNCTION(xslt_output_endtransform)
{
	char *tRes   = NULL,
	     *buffer = NULL;
	int ret = 0;
	OLS_FETCH();
	SABLOTLS_FETCH();

	if (OG(nesting_level) == 0) {
		RETURN_NULL();
	}

	buffer = estrndup(OG(active_ob_buffer).buffer, OG(active_ob_buffer).text_length);

	if (strlen(buffer)) {
		char *args[] = {"/_xmlinput", buffer,
		                "/_output",   NULL};
		
		ret = SablotProcess(SABLOTG(output_transform_file), "arg:/_xmlinput", "arg:/_output", NULL, args, &tRes);
	}

	if (ret) {
		SABLOTG(last_errno) = ret;
		S_FREE(SABLOTG(output_transform_file));
		return;
	}
		
	php_end_ob_buffer(0);
	
	PUTS(tRes);
	S_FREE(SABLOTG(output_transform_file));
	S_FREE(buffer);

	if (tRes) {
		SablotFree(tRes);
	} else {
		php_end_ob_buffer(0);
	}
}
/* }}} */

/* }}} */
/* {{{ Begin Simple API */

/* {{{ bool xslt_transform(string xslt_uri, string transform_uri, string result_uri[, array xslt_params[, array xslt_args[, string &result]]])
   Transform an XML document, transform_uri, with an XSL stylesheet, xslt_uri with parameters, xslt_params, into the Result buffer, result_uri, xslt_args defines the variables in xslt_uri, transform_uri and result_uri. */
PHP_FUNCTION(xslt_transform)
{
	zval **xslt_uri, 
	     **transform_uri, 
		 **result_uri,
	     **xslt_params, 
		 **xslt_args, 
		 **result;
	char **args   = NULL,
	     **params = NULL,
	     *tResult = NULL;
	int argc = ZEND_NUM_ARGS(), 
	    ret  = 0;
	SABLOTLS_FETCH();
	
	if (argc < 3 || argc > 6 ||
	    zend_get_parameters_ex(argc, &xslt_uri, &transform_uri, &result_uri, &xslt_params, &xslt_args, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	multi_convert_to_string_ex(3, xslt_uri, transform_uri, result_uri);
	
	if (argc > 3) {
		if (Z_TYPE_PP(xslt_params) != IS_LONG || Z_LVAL_PP(xslt_params) != 0) {
			int numelems, 
			    size;
			HashTable *ar = HASH_OF(*xslt_params);

			numelems = zend_hash_num_elements(ar);
			size = (numelems * 2 + 1) * sizeof(char *);
			
			params = (char **)emalloc(size+1);
			memset((char *)params, 0, size);
			
			_php_sablot_ht_char(ar, params);
		}
	}
	
	if (argc > 4) {
		if (Z_TYPE_PP(xslt_args) != IS_LONG || Z_LVAL_PP(xslt_args) != 0) {
			int numelems, 
			    size;
			HashTable *ar = HASH_OF(*xslt_args);

			numelems = zend_hash_num_elements(ar);
			size = (numelems * 2 + 1) * sizeof(char *);
			
			args = (char **)emalloc(size+1);
			memset((char *)args, 0, size);
			
			_php_sablot_ht_char(ar, args);
		}
	}
	
	ret = SablotProcess(Z_STRVAL_PP(xslt_uri), Z_STRVAL_PP(transform_uri),
	                    Z_STRVAL_PP(result_uri), params, args, &tResult);
	
	if (ret) {
		SABLOTG(last_errno) = ret;
		
		S_FREE(params);
		S_FREE(args);
		if (tResult) {
			SablotFree(tResult);
		}
		
		RETURN_FALSE;
	} else { RETVAL_TRUE; }


	if (tResult &&
	    argc == 6) {
		ZVAL_STRING(*result, tResult, 1);
	}
	
	S_FREE(params);
	S_FREE(args);
	
	if (tResult)
		SablotFree(tResult);
}
/* }}} */

/* {{{ bool xslt_process(string xslt, string input_str, string &result[, string base])
   Process data given by input_str through xslt and place the results in the string result.  If base is supplied, it will be used as the base URI. */
PHP_FUNCTION(xslt_process)
{
	zval **xslt, 
	     **input, 
		 **result, 
		 **base;
	char *tRes = NULL;
	int ret  = 0, 
	    argc = ZEND_NUM_ARGS();
	SABLOTLS_FETCH();
	
	if (argc < 3 || argc > 4 ||
	    zend_get_parameters_ex(argc, &xslt, &input, &result, &base) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	multi_convert_to_string_ex(2, xslt, input);

	if (argc > 3) {
		convert_to_string_ex(base);
		ret = SablotProcessStringsWithBase(Z_STRVAL_PP(xslt), Z_STRVAL_PP(input), &tRes, Z_STRVAL_PP(base));
	} else {
		ret = SablotProcessStrings(Z_STRVAL_PP(xslt), Z_STRVAL_PP(input), &tRes);
	}
	
	if (ret) {
		SABLOTG(last_errno) = ret;
		if (tRes) {
			SablotFree(tRes);
		}
		RETURN_FALSE;
	}

	if (tRes) {
		ZVAL_STRING(*result, tRes, 1);
		SablotFree(tRes);
	}
	RETURN_TRUE;
}
/* }}} */

/* }}} */

/* {{{ Begin Advanced Resource API */

/* {{{ proto resource xslt_create(void)
   Create a new XSL processor and return a resource identifier. */
PHP_FUNCTION(xslt_create)
{
	php_sablot *handle;
	SablotHandle p;
	int ret;
	SABLOTLS_FETCH();
	
	ret = SablotCreateProcessor(&p);
	
	if (ret) {
		SABLOTG(last_errno) = ret;
		RETURN_FALSE;
	}
	
	handle = (php_sablot *)emalloc(sizeof(php_sablot));
	if (!handle) {
		php_error(E_WARNING, "Couldn't allocate PHP-Sablotron Handle");
		RETURN_FALSE;
	}
	memset(handle, 0, sizeof(php_sablot));
	
	handle->p = p;
	ret = SablotRegHandler(handle->p, HLR_SAX, (void *)&sax, (void *)handle);
	
	if (ret) {
		SABLOTG(last_errno) = ret;
		RETURN_FALSE;
	}

	ret = SablotRegHandler(handle->p, HLR_MESSAGE, (void *)&mh, (void *)handle);
	
	if (ret) {
		SABLOTG(last_errno) = ret;
		RETURN_FALSE;
	}
	
	ZEND_REGISTER_RESOURCE(return_value, handle, SABLOTG(le_sablot));
	handle->index = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto bool xslt_run(resource xh, string xslt_file, string data_file[, string result[, array xslt_params[, array xslt_args]]])
   Process the file data_file with the XSL stylesheet xslt_file and parameters xslt_parameters place the results in the buffer pointed to by the result parameter (defaults to "/_result"), args contains the values of the variables in the other parameters.  */
PHP_FUNCTION(xslt_run)
{
	zval **xh, 
	     **xslt_file, 
		 **data_file, 
	     **xslt_result, 
		 **xslt_params, 
		 **xslt_args;
	php_sablot *handle;
	char **args   = NULL,
	     **params = NULL,
	     *result  = NULL;
	int argc = ZEND_NUM_ARGS(),
	    ret  = 0;
	SABLOTLS_FETCH();
	
	if (argc < 3 || argc > 6 ||
	    zend_get_parameters_ex(argc, &xh, &xslt_file, &data_file, &xslt_result, &xslt_params, &xslt_args) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron handle", SABLOTG(le_sablot));
	multi_convert_to_string_ex(2, xslt_file, data_file);
	
	if (argc == 3) {
		result = estrdup("arg:/_result");
	}
	
	if (argc > 3) {
		convert_to_string_ex(xslt_result);
		result = estrndup(Z_STRVAL_PP(xslt_result), Z_STRLEN_PP(xslt_result));
	}
	
	if (argc > 4) {
		if (Z_TYPE_PP(xslt_params) != IS_LONG || Z_LVAL_PP(xslt_params) != 0) { 
			int numelems, 
			    size;	
			HashTable *ar = HASH_OF(*xslt_params);

			numelems = zend_hash_num_elements(ar);
			size = (numelems * 2 + 1) * sizeof(char *);
			
			params = (char **)emalloc(size+1);
			memset((char *)params, 0, size);
			
			_php_sablot_ht_char(ar, params);
		}
	}
	
	if (argc > 5) {
		if (Z_TYPE_PP(xslt_args) != IS_LONG || Z_LVAL_PP(xslt_args) != 0) {
			int numelems, 
			    size;
			HashTable *ar = HASH_OF(*xslt_args);

			numelems = zend_hash_num_elements(ar);
			size = (numelems * 2 + 1) * sizeof(char *);
			
			args = (char **)emalloc(size+1);
			memset((char *)args, 0, size);
			
			_php_sablot_ht_char(ar, args);
		}
	}
	
	ret = SablotRunProcessor(handle->p, Z_STRVAL_PP(xslt_file), Z_STRVAL_PP(data_file), result, params, args);
	
	if (ret) {
		handle->last_errno = ret;
		SABLOTG(last_errno) = ret;
		S_FREE(params);
		S_FREE(args);
		S_FREE(result);
		RETURN_FALSE;
	}
	
	S_FREE(params);
	S_FREE(args);
	S_FREE(result);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool xslt_openlog(resource xh, string logfile[, int loglevel])
   Sets a logfile for Sablotron to place all of its error messages */
PHP_FUNCTION(xslt_openlog)
{
	zval **xh, 
	     **logfile, 
		 **opt_loglevel;
	php_sablot *handle;
	int ret      = 0, 
	    loglevel = 0, 
		argc     = ZEND_NUM_ARGS();
    SABLOTLS_FETCH();
	
	if (argc < 2 || argc > 3 ||
	    zend_get_parameters_ex(argc, &xh, &logfile, &opt_loglevel) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron", SABLOTG(le_sablot));
	convert_to_string_ex(logfile);
	
	if (argc > 2) {
		convert_to_long_ex(opt_loglevel);
		loglevel = Z_LVAL_PP(opt_loglevel);
	}
	
	ret = SablotSetLog(handle->p, (const char *)Z_STRVAL_PP(logfile), loglevel);
	
	if (ret) {
		handle->last_errno  = ret;
		SABLOTG(last_errno) = ret;
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool xslt_closelog(resource xh)
   Clear the logfile for a given instance Sablotron */
PHP_FUNCTION(xslt_closelog)
{
	zval **xh;
	php_sablot *handle;
	int ret;
	SABLOTLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &xh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron", SABLOTG(le_sablot));
	
	ret = SablotSetLog(handle->p, (const char *)NULL, 1);
	
	if (ret) {
		RETURN_FALSE;
	}
	RETURN_TRUE; 
}
/* }}} */

/* {{{ proto string xslt_fetch_result(resource xh[, string result_name])
   Fetch a result buffer on process handle, xh, with name, result_name, if name is not given than the "/_result" buffer is fetched. */
PHP_FUNCTION(xslt_fetch_result)
{
	zval **xh, 
	     **result_name;
	php_sablot *handle;
	char *value = NULL, 
	     *rname = NULL;
	int argc = ZEND_NUM_ARGS(), 
	    ret = 0;
	SABLOTLS_FETCH();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &xh, &result_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron Handle", SABLOTG(le_sablot));
	
	if (argc > 1) {
		convert_to_string_ex(result_name);
		rname = estrndup(Z_STRVAL_PP(result_name), Z_STRLEN_PP(result_name));
	} else {
		rname = estrdup("/_result");
	}
	
	ret = SablotGetResultArg(handle->p, rname, &value);
	
	S_FREE(rname);
	
	if (ret) {
		handle->last_errno  = ret;
		SABLOTG(last_errno) = ret;
		if (value) {
			SablotFree(value);
		}
		RETURN_FALSE;
	}
	
	if (value) {
		RETVAL_STRING(value, 1);
		SablotFree(value);
	}
}
/* }}} */

/* {{{ proto void xslt_free(resource xh)
   Free resources associated with a xslt handle. */
PHP_FUNCTION(xslt_free)
{
	zval **xh;
	php_sablot *handle;
	SABLOTLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &xh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron Handle", SABLOTG(le_sablot));
	zend_list_delete(Z_LVAL_PP(xh));
}
/* }}} */

/* {{{ proto void xslt_set_sax_handler(resource xh, array handlers)
   Set SAX Handlers on the resource handle given by xh. */
PHP_FUNCTION(xslt_set_sax_handler)
{
	zval **xh, 
	     **handlers, 
		 **indiv_handlers;
	php_sablot *handle;
	HashTable *handlers_list;
	char *string_key = NULL;
	ulong num_key;
	SABLOTLS_FETCH();
	

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &xh, &handlers) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron Handle", SABLOTG(le_sablot));
	
	handlers_list = HASH_OF(*handlers);
	
	for (zend_hash_internal_pointer_reset(handlers_list);
	     zend_hash_get_current_data(handlers_list, (void **)&indiv_handlers) == SUCCESS;
	     zend_hash_move_forward(handlers_list)) {
		
		SEPARATE_ZVAL(indiv_handlers);
		
		if (zend_hash_get_current_key(handlers_list, &string_key, &num_key) == HASH_KEY_IS_LONG) {
			php_error(E_WARNING, "The Keys of the first dimension of your array must be strings");
			RETURN_FALSE;
		}
		
		
		if (!strcasecmp("document", string_key)) {
			_php_sablot_handler_pair(handle, 
			                         &handle->startDocHandler, &handle->endDocHandler,
			                         indiv_handlers);
		} else if (!strcasecmp("element", string_key)) {
			_php_sablot_handler_pair(handle, 
			                         &handle->startElementHandler, &handle->endElementHandler, 
			                         indiv_handlers);
		} else if (!strcasecmp("namespace", string_key)) {
			_php_sablot_handler_pair(handle,
			                         &handle->startNamespaceHandler, &handle->endNamespaceHandler, 
			                         indiv_handlers);
		} else if (!strcasecmp("comment", string_key)) {
			zval_add_ref(indiv_handlers);
			handle->commentHandler = *indiv_handlers;
		} else if (!strcasecmp("pi", string_key)) {
			zval_add_ref(indiv_handlers);
			handle->PIHandler = *indiv_handlers;
		} else if (!strcasecmp("characters", string_key)) {
			zval_add_ref(indiv_handlers);
			handle->charactersHandler = *indiv_handlers;
		} else {		
			php_error(E_WARNING, "Invalid option: %s", string_key);
		}
		
		S_FREE(string_key);
	}
}
/* }}} */

/* }}} */

/* {{{ Begin Error Handling functions */

/* {{{ proto mixed xslt_error([int xh])
   Return the error string related to a given error handle.  If no handle is given the last error that occurred anywhere is returned. */
PHP_FUNCTION(xslt_error)
{
	zval **xh;
	php_sablot *handle;
	int serrno = 0,
	    argc = ZEND_NUM_ARGS();
	SABLOTLS_FETCH();
    
	if (argc < 0 || argc > 1 ||
	    zend_get_parameters_ex(argc, &xh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (argc) {
		ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron Handle", SABLOTG(le_sablot));

		if (handle->errors) {
			if (array_init(return_value) == FAILURE) {
				php_error(E_WARNING, "Cannot initialize return array from xslt_error()");
				RETURN_FALSE;
			}
		
			handle->errors = handle->errors_start.next;
			while (handle->errors) {
				add_assoc_string(return_value, handle->errors->key, handle->errors->value, 0);
				handle->errors = handle->errors->next;
			}
			add_assoc_string(return_value, "errstr", (char *)SablotGetMsgText(handle->last_errno), 1);
			add_assoc_long(return_value, "errno", handle->last_errno);
			return;
		}
	
		serrno = handle->last_errno;
	} else {
		serrno = SABLOTG(last_errno);
	}

	RETURN_STRING((char *)SablotGetMsgText(errno), 1);
}
/* }}} */

/* {{{ proto int xslt_errno([int xh])
   Return the error number related to a given error handle.  If no handle is given the last error number that occurred anywhere is returned. */
PHP_FUNCTION(xslt_errno)
{
	zval **xh;
	php_sablot *handle;
	int argc = ZEND_NUM_ARGS();
	
	SABLOTLS_FETCH();

	if (argc < 0 || argc > 1 ||
	    zend_get_parameters_ex(argc, &xh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (argc) {
		ZEND_FETCH_RESOURCE(handle, php_sablot *, xh, -1, "PHP-Sablotron Handle", SABLOTG(le_sablot));
		RETURN_LONG(handle->last_errno);
	} else {
		RETURN_LONG(SABLOTG(last_errno));
	}
}
/* }}} */

/* }}} */

/* }}} */

/* {{{ BEGIN HANDLER FUNCTIONS */

/* {{{ Begin SAX Handler functions */

/* {{{ _php_sablot_handler_pair()
   Set the handler functions from a two item array */
static void _php_sablot_handler_pair(php_sablot *handle, zval **first_func, zval **second_func, zval **indiv_handlers)
{
	HashTable *second_level = HASH_OF(*indiv_handlers);
	zval **handler;
	int item = 0;

	for (zend_hash_internal_pointer_reset(second_level);
	     zend_hash_get_current_data(second_level, (void **)&handler) == SUCCESS;
	     zend_hash_move_forward(second_level)) {
		
		SEPARATE_ZVAL(handler);
		zval_add_ref(handler);
	
		switch (item)
		{
			case 0:
				*first_func  = *handler;
				break;
			case 1:
				*second_func = *handler;
				break;
			default:
				convert_to_string_ex(handler);
				php_error(E_WARNING, "Sorry, too many elements, %s discarded", Z_STRVAL_PP(handler));
				zval_del_ref(handler);
				break;
		}
		item++;
	}
}
/* }}} */

/* {{{ _php_sablot_call_handler_function()
   Call a sablot call handler function, wrapper for call_user_function() */
static void _php_sablot_call_handler_function(zval *handlerName, int argc, zval **args, char *function_name)
{
	zval *retval;
	int i;
	ELS_FETCH();
	MAKE_STD_ZVAL(retval);
	
	if (call_user_function(EG(function_table), NULL, handlerName, retval, argc, args) == FAILURE) {
		php_error(E_WARNING, "Sorry, couldn't call %s handler", function_name);
	}
	
	zval_dtor(retval);
	efree(retval);
	
	for (i=0; i<argc; i++) {
		zval_del_ref(&(args[i]));
	}
}
/* }}} */

/* {{{ _php_sablot_startDoc() 
   Start document handler */
static SAX_RETURN _php_sablot_sax_startDoc(void *userData)
{
	php_sablot *handle = (php_sablot *)userData;

	if (handle->startDocHandler) {
		zval *args[1];
		args[0] = _php_sablot_resource_zval(handle->index);

		_php_sablot_call_handler_function(handle->startDocHandler, 1, args, "start document");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_startElement() 
   Start Element handler */
static SAX_RETURN _php_sablot_sax_startElement(void *userData, const char *name, const char **attributes)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->startElementHandler) {
		zval *args[3];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(name);

		MAKE_STD_ZVAL(args[2]);
		if (array_init(args[2]) == FAILURE) {
			php_error(E_WARNING, "Couldn't initialize array to be passed to start element handler");
			return;
		}

		while (attributes && *attributes) {
			char *key = (char *)attributes[0];
			zval *value = _php_sablot_string_zval(attributes[1]);
			
			add_assoc_string(args[2], key, Z_STRVAL_P(value), 0);
			attributes += 2;
		}
	
		_php_sablot_call_handler_function(handle->startElementHandler, 3, args, "start element");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_endElement() 
   End element handler */
static SAX_RETURN _php_sablot_sax_endElement(void *userData, const char *name)
{
	php_sablot *handle = (php_sablot *)userData;

	if (handle->endElementHandler) {
		zval *args[2];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(name);
		
		_php_sablot_call_handler_function(handle->endElementHandler, 2, args, "end element");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_startNamespace() 
   Start Namespace handler */
static SAX_RETURN _php_sablot_sax_startNamespace(void *userData, const char *prefix, const char *uri)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->startNamespaceHandler) {
		zval *args[3];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(prefix);
		args[2] = _php_sablot_string_zval(uri);
		
		_php_sablot_call_handler_function(handle->startNamespaceHandler, 3, args, "start namespace");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_endNamespace() 
   End Namespace Handler */
static SAX_RETURN _php_sablot_sax_endNamespace(void *userData, const char *prefix)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->endNamespaceHandler) {
		zval *args[2];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(prefix);
		
		_php_sablot_call_handler_function(handle->endNamespaceHandler, 2, args, "end namespace");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_comment() 
   Comment Handler */
static SAX_RETURN _php_sablot_sax_comment(void *userData, const char *contents)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->commentHandler) {
		zval *args[2];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(contents);
		
		_php_sablot_call_handler_function(handle->commentHandler, 2, args, "comment");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_PI() 
   Processing Instruction Handler */
static SAX_RETURN _php_sablot_sax_PI(void *userData, const char *target, const char *contents)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->PIHandler) {
		zval *args[3];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(target);
		args[2] = _php_sablot_string_zval(contents);
		
		_php_sablot_call_handler_function(handle->PIHandler, 3, args, "PI Handler");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_characters() 
   Character handler */
static SAX_RETURN _php_sablot_sax_characters(void *userData, const char *contents, int length)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->charactersHandler) {
		zval *args[2];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		args[1] = _php_sablot_string_zval(contents);
		
		_php_sablot_call_handler_function(handle->charactersHandler, 2, args, "characters");
	}
}
/* }}} */

/* {{{ _php_sablot_sax_endDoc() 
   End document handler */
static SAX_RETURN _php_sablot_sax_endDoc(void *userData)
{
	php_sablot *handle = (php_sablot *)userData;
	
	if (handle->endDocHandler) {
		zval *args[1];
		
		args[0] = _php_sablot_resource_zval(handle->index);
		
		_php_sablot_call_handler_function(handle->endDocHandler, 1, args, "end document");
	}
}
/* }}} */


/* }}} */

/* {{{ Sablotron Error Handling */

/* {{{ _php_sablot_make_code() 
   Makes an error code, currently does nothing */
static MH_ERROR _php_sablot_make_code(void *userData, SablotHandle p, int severity, unsigned short facility, unsigned short code)
{
	return(code);
}
/* }}} */

/* {{{ _php_sablot_error()
   Handle Sablotron errors */
static MH_ERROR _php_sablot_error(void *userData, SablotHandle p, MH_ERROR code, MH_LEVEL level, char **fields)
{
	php_sablot *handle = (php_sablot *)userData;

	char *sep;

	int idx, 
	    len;

	if (!fields) {
		if (code) {
			handle->last_errno = (int)code;
		}
		return(0);
	}	
	
	if (handle->errors) {
		handle->errors = handle->errors_start.next;
		while (handle->errors) {
			S_FREE(handle->errors->key);
			S_FREE(handle->errors->value);
			handle->errors = handle->errors->next;
		}
		S_FREE(handle->errors);
	}

	handle->errors_start.next = NULL;
	handle->errors = &(handle->errors_start);

	while (fields && *fields) {
		handle->errors->next = (php_sablot_error *)emalloc(sizeof(php_sablot_error));
		handle->errors = handle->errors->next;
	
		sep = strchr(fields[0], ':');
		
		idx = sep - fields[0];
		len = strlen(fields[0]);
		
		handle->errors->key   = emalloc(idx+1);
		handle->errors->value = emalloc((len - idx) + 1);
		memcpy(handle->errors->key, fields[0], idx);
		memcpy(handle->errors->value, fields[0] + idx + 1, len - idx - 1);
		
		handle->errors->next = NULL;
		fields++;
	}
	handle->last_errno = (int)code;
	
	switch (code) {
		case MH_LEVEL_CRITICAL:
			SEND_ERROR_INFO("Fatal Error", E_CORE);
			return (0);
		case MH_LEVEL_ERROR:
			SEND_ERROR_INFO("Serious Error", E_ERROR);
			return (0);
		case MH_LEVEL_WARN:
			SEND_ERROR_INFO("Warning", E_WARNING);
			break;
	}

	return(0);
}
/* }}} */

/* }}} */

/* {{{ List Handling functions */

/* {{{ _php_sablot_free_processor()
   Free a Sablot handle */
static void _php_sablot_free_processor(zend_rsrc_list_entry *rsrc)
{
	php_sablot *handle = (php_sablot *)rsrc->ptr;
	if (handle->p) {
		SablotUnregHandler(handle->p, HLR_MESSAGE, NULL, NULL);
		SablotUnregHandler(handle->p, HLR_SAX, NULL, NULL);
		SablotDestroyProcessor(handle->p);
	}
	
	FUNCH_FREE(handle->startDocHandler);
	FUNCH_FREE(handle->startElementHandler);
	FUNCH_FREE(handle->endElementHandler);
	FUNCH_FREE(handle->startNamespaceHandler);
	FUNCH_FREE(handle->endNamespaceHandler);
	FUNCH_FREE(handle->commentHandler);
	FUNCH_FREE(handle->PIHandler);
	FUNCH_FREE(handle->charactersHandler);
	FUNCH_FREE(handle->endDocHandler);

	if (handle->errors) {
		handle->errors = handle->errors_start.next;
		while (handle->errors) {
			S_FREE(handle->errors->key);
			S_FREE(handle->errors->value);
			handle->errors = handle->errors->next;
		}
		S_FREE(handle->errors);
	}
	S_FREE(handle);
}
/* }}} */

/* }}} */

/* {{{ Conversion Helper functions */

/* {{{ _php_sablot_zval_char()
   Translates a PHP array to a Sablotron character array */
static void _php_sablot_ht_char(HashTable *php, char **sablot)
{
	zval **value;
	char *string_key = NULL;
	ulong num_key;
	int i = 0;
	
	for (zend_hash_internal_pointer_reset(php);
	     zend_hash_get_current_data(php, (void **)&value) == SUCCESS;
	     zend_hash_move_forward(php)) {
		SEPARATE_ZVAL(value);
		convert_to_string_ex(value);
		
		switch (zend_hash_get_current_key(php, &string_key, &num_key)) {
			case HASH_KEY_IS_LONG:
				sablot[i++] = Z_STRVAL_PP(value);
				break;
			case HASH_KEY_IS_STRING:
				sablot[i++] = string_key;
				sablot[i++] = Z_STRVAL_PP(value);
				break;
		}
	}
	sablot[i++] = NULL;
}

/* {{{ _php_sablot_string_zval()
   Converts a Sablotron string to a zval */
static zval *_php_sablot_string_zval(const char *str)
{
	zval *ret;
	int len = strlen(str);

	MAKE_STD_ZVAL(ret);
	
	Z_TYPE_P(ret)   = IS_STRING;
	Z_STRLEN_P(ret) = len;
	Z_STRVAL_P(ret) = estrndup(str, len);

	return(ret);
}
/* }}} */

/* {{{ _php_sablot_resource_zval()
   Converts a long identifier to a resource id */
static zval *_php_sablot_resource_zval(long value)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	
	Z_TYPE_P(ret) = IS_RESOURCE;
	Z_LVAL_P(ret) = value;
	
	zend_list_addref(value);

	return(ret);
}
/* }}} */

/* }}} */

/* }}} */
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
