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
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_xslt.h"
#include "php_sablot.h"
#include "ext/standard/info.h"

#if HAVE_SABLOT_BACKEND

#include <sablot.h>

#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* functions relating to handlers */
static void register_sax_handler_pair(zval **, zval **, zval **);

/* Free processor */
static void free_processor(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* Scheme handler functions */
static int  scheme_getall(void *, SablotHandle, const char *, const char *, char **, int *);
static int  scheme_freememory(void *, SablotHandle, char *);
static int  scheme_open(void *, SablotHandle, const char *, const char *, int *);
static int  scheme_get(void *, SablotHandle, int, char *, int *);
static int  scheme_put(void *, SablotHandle, int, const char *, int *);
static int  scheme_close(void *, SablotHandle, int);

/* Sax handler functions */
static SAX_RETURN sax_startdoc(void *, SablotHandle);
static SAX_RETURN sax_startelement(void *, SablotHandle, const char *, const char **);
static SAX_RETURN sax_endelement(void *, SablotHandle, const char *);
static SAX_RETURN sax_startnamespace(void *, SablotHandle, const char *, const char *);
static SAX_RETURN sax_endnamespace(void *, SablotHandle, const char *);
static SAX_RETURN sax_comment(void *, SablotHandle, const char *);
static SAX_RETURN sax_pi(void *, SablotHandle, const char *, const char *);
static SAX_RETURN sax_characters(void *, SablotHandle, const char *, int);
static SAX_RETURN sax_enddoc(void *, SablotHandle);

/* Error handlers */
static MH_ERROR error_makecode(void *, SablotHandle, int, unsigned short, unsigned short);
static MH_ERROR error_log(void *, SablotHandle, MH_ERROR, MH_LEVEL, char **);
static MH_ERROR error_print(void *, SablotHandle, MH_ERROR, MH_LEVEL, char **);

/* Resource related */
#define le_xslt_name "XSLT Processor"
static int  le_xslt;

/* {{{ xslt_functions[]
 */
function_entry xslt_functions[] = {
	PHP_FE(xslt_create,              NULL)
	PHP_FE(xslt_set_sax_handlers,    NULL)
	PHP_FE(xslt_set_scheme_handlers, NULL)
	PHP_FE(xslt_process,             NULL)
	PHP_FE(xslt_set_base,            NULL)
	PHP_FE(xslt_set_log,             NULL)
	PHP_FE(xslt_set_error_handler,   NULL)
#ifdef HAVE_SABLOT_SET_ENCODING
	PHP_FE(xslt_set_encoding,        NULL)
#endif
	PHP_FE(xslt_free,                NULL)

	PHP_FE(xslt_error,                   NULL)
	PHP_FE(xslt_errno,                   NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ xslt_module_entry
 */
zend_module_entry xslt_module_entry = {
	STANDARD_MODULE_HEADER,
	"xslt",
	xslt_functions,
	PHP_MINIT(xslt),
	NULL,
	NULL,	
	NULL,
	PHP_MINFO(xslt),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XSLT
ZEND_GET_MODULE(xslt)
#endif

/* {{{ handler structs
   A structure containing the sax handlers, automatically 
   registered whether the user defines them or not */
static SAXHandler sax_handlers = 
{
	sax_startdoc,
	sax_startelement,
	sax_endelement,
	sax_startnamespace,
	sax_endnamespace,
	sax_comment,
	sax_pi,
	sax_characters,
	sax_enddoc
};

/* Error handlers, automatically registered */
static MessageHandler message_handler = {
	error_makecode,
	error_log,
	error_print
};

/* Scheme handlers automatically registered */
static SchemeHandler scheme_handler = {
	scheme_getall,
	scheme_freememory,
	scheme_open,
	scheme_get,
	scheme_put,
	scheme_close
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xslt)
{
	le_xslt = zend_register_list_destructors_ex(free_processor, NULL, le_xslt_name, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xslt)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XSLT support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource xslt_create(void) 
   Create a new XSLT processor */
PHP_FUNCTION(xslt_create)
{
	php_xslt        *handle;      /* The php -> sablotron handle */
	SablotSituation  ctx;         /* Sablotron situation */
	SablotHandle     processor;   /* The sablotron processor */
	int              error;       /* The error container */

	/* Allocate the php-sablotron handle */
	handle                   = ecalloc(1, sizeof(php_xslt));
	handle->handlers         = ecalloc(1, sizeof(struct xslt_handlers));
	handle->err              = ecalloc(1, sizeof(struct xslt_error));

	XSLT_LOG(handle).path = NULL;

	/* Unless SAX handlers are  */
	handle->cacheable = 1;

	/* Allocate the processor ctx */
	error = SablotCreateSituation(&ctx);
	if (error) {
		XSLT_ERRNO(handle) = error;
		RETURN_FALSE;
	}

	XSLT_CONTEXT(handle) = ctx;

	/* Allocate the actual processor itself, via sablotron */
	error = SablotCreateProcessor(&processor);
	if (error) {
		XSLT_ERRNO(handle) = error;
		RETURN_FALSE;
	}

	/* Save the processor and set the default handlers */
	XSLT_PROCESSOR(handle) = processor;
	SablotRegHandler(XSLT_PROCESSOR(handle), HLR_SAX,     (void *) &sax_handlers,    (void *) handle);
	SablotRegHandler(XSLT_PROCESSOR(handle), HLR_MESSAGE, (void *) &message_handler, (void *) handle);
	SablotRegHandler(XSLT_PROCESSOR(handle), HLR_SCHEME,  (void *) &scheme_handler,  (void *) handle);

	/* Register the processor as a resource and return it to the user */
	ZEND_REGISTER_RESOURCE(return_value, handle, le_xslt);
	
	/* The resource index, save it for later use */
	handle->processor.idx = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto void xslt_set_sax_handlers(resource processor, array handlers)
   Set the SAX handlers to be called when the XML document gets processed */
PHP_FUNCTION(xslt_set_sax_handlers)
{
	zval       **processor_p,      /* Resource pointer to the php->sablotron handle */
	           **sax_handlers_p,   /* Pointer to the sax handlers php array */
	           **handler;          /* Individual handler, or handler pair */
	HashTable   *sax_handlers;     /* PHP array of sax handlers */
	php_xslt    *handle;           /* Pointer to a php_xslt handle */
	char        *string_key;       /* String key for the hash */
	ulong        num_key;          /* (unused) hash's numerical key */
	int          key_type;         /* The type of the current hash key */

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &processor_p, &sax_handlers_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	
	handle->cacheable = 0;

	/* Convert the sax_handlers_p zval ** to a hash table we can process */
	sax_handlers = HASH_OF(*sax_handlers_p);
	if (!sax_handlers) {
		php_error(E_WARNING, "Expecting an array as the second argument to xslt_set_sax_handlers()");
		return;
	}

	/* Loop through the HashTable containing the SAX handlers */
	for (zend_hash_internal_pointer_reset(sax_handlers);
	     zend_hash_get_current_data(sax_handlers, (void **) &handler) == SUCCESS;
		 zend_hash_move_forward(sax_handlers)) {

		key_type = zend_hash_get_current_key(sax_handlers, &string_key, &num_key, 0);
		if (key_type == HASH_KEY_IS_LONG) {
			convert_to_string_ex(handler);
			php_error(E_NOTICE, "Skipping numerical index %d (with value %s) in xslt_set_sax_handlers()",
			          num_key, Z_STRVAL_PP(handler));
			continue;
		}

		/* Document handlers (document start, document end) */
		if (!strcmp(string_key, "document")) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).doc_start, 
			                          &XSLT_SAX(handle).doc_end, 
			                          handler);
		}
		/* Element handlers, start of an element, and end of an element */
		else if (!strcmp(string_key, "element")) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).element_start, 
			                          &XSLT_SAX(handle).element_end, 
			                          handler);
		}
		/* Namespace handlers, start of a namespace, end of a namespace */
		else if (!strcmp(string_key, "namespace")) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).namespace_start, 
			                          &XSLT_SAX(handle).namespace_end, 
			                          handler);
		}
		/* Comment handlers, called when a comment is reached */
		else if (!strcmp(string_key, "comment")) {
			XSLT_SAX(handle).comment = *handler;
			zval_add_ref(&XSLT_SAX(handle).comment);
		}
		/* Processing instructions handler called when processing instructions
		   (<? ?>) */
		else if (!strcmp(string_key, "pi")) {
			XSLT_SAX(handle).pi = *handler;
			zval_add_ref(&XSLT_SAX(handle).pi);
		}
		/* Character handler, called when data is found */
		else if (!strcmp(string_key, "character")) {
			XSLT_SAX(handle).characters = *handler;
			zval_add_ref(&XSLT_SAX(handle).characters);
		}
		/* Invalid handler name, tsk, tsk, tsk :) */
		else {
			php_error(E_WARNING, "Invalid option to xslt_set_sax_handlers(): %s", string_key);
		}
	}
}
/* }}} */

/* {{{ proto void xslt_set_scheme_handlers(resource processor, array handlers)
   Set the scheme handlers for the XSLT processor */
PHP_FUNCTION(xslt_set_scheme_handlers)
{
	zval                   **processor_p,       /* Resource pointer to the php->sablotron handle */
	                       **scheme_handlers_p, /* Pointer to the scheme handler array */
	                       **handler;           /* Individual scheme handler */
	zval                   **assign_handle;     /* The handler to assign to */
	HashTable               *scheme_handlers;   /* Scheme handler array */
	php_xslt                *handle;            /* php->sablotron handle */
	char                    *string_key;        /* Hash key (string) */
	ulong                    num_key;           /* (unused) Hash key (number) */
	int                      key_type;          /* The type of the current key */

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &processor_p, &scheme_handlers_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	scheme_handlers = HASH_OF(*scheme_handlers_p);
	if (!scheme_handlers) {
		php_error(E_WARNING, "2nd argument to xslt_set_scheme_handlers() must be an array");
		return;
	}

	/* Loop through the scheme handlers array, setting the given
	   scheme handlers */
	for (zend_hash_internal_pointer_reset(scheme_handlers);
	     zend_hash_get_current_data(scheme_handlers, (void **) &handler) == SUCCESS;
	     zend_hash_move_forward(scheme_handlers)) {

		key_type = zend_hash_get_current_key(scheme_handlers, &string_key, &num_key, 0);
		if (key_type == HASH_KEY_IS_LONG) {
			php_error(E_NOTICE, "Numerical key %d (with value %s) being ignored "
					  "in xslt_set_scheme_handlers()",
					  num_key, Z_STRVAL_PP(handler));
			continue;
		}

		/* Open the URI and return the whole string */
		if (!strcmp(string_key, "get_all")) {
			assign_handle = &XSLT_SCHEME(handle).get_all;
		}
		/* Open the URI and return a handle */
		else if (!strcmp(string_key, "open")) {
			assign_handle = &XSLT_SCHEME(handle).open;
		}
		/* Retrieve data from the URI */
		else if (!strcmp(string_key, "get")) {
			assign_handle = &XSLT_SCHEME(handle).get;
		}
		/* Save data to the URI */
		else if (!strcmp(string_key, "put")) {
			assign_handle = &XSLT_SCHEME(handle).put;
		}
		/* Close the URI */
		else if (!strcmp(string_key, "close")) {
			assign_handle = &XSLT_SCHEME(handle).close;
		}
		/* Invalid handler name */
		else {
			php_error(E_WARNING, "%s() invalid option '%s', skipping", 
					  get_active_function_name(TSRMLS_C), string_key);
			continue;
		}

		*assign_handle = *handler;
		zval_add_ref(assign_handle);
	}
}
/* }}} */

/* {{{ proto void xslt_set_error_handler(resource processor, mixed error_func)
   Set the error handler, to be called when an XSLT error happens */
PHP_FUNCTION(xslt_set_error_handler)
{
	zval      **processor_p,   /* Resource Pointer to a PHP-XSLT processor */
	          **error_func;    /* Name of the user defined error function */
	php_xslt   *handle;        /* A PHP-XSLT processor */
	
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &processor_p, &error_func) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	XSLT_ERROR(handle) = *error_func;
	zval_add_ref(&XSLT_ERROR(handle));
}
/* }}} */

/* {{{ proto void xslt_set_base(resource processor, string base)
   Sets the base URI for all XSLT transformations */
PHP_FUNCTION(xslt_set_base)
{
	zval     **processor_p,  /* Resource Pointer to a PHP-XSLT processor */
	         **base;         /* The base URI for the transformation */
	php_xslt  *handle;       /* A PHP-XSLT processor */

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &processor_p, &base) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	convert_to_string_ex(base);

	/* Set the base */
	SablotSetBase(XSLT_PROCESSOR(handle), Z_STRVAL_PP(base));
}
/* }}} */

/* {{{ proto void xslt_set_encoding(resource processor, string encoding)
   Set the output encoding for the current stylesheet */
PHP_FUNCTION(xslt_set_encoding)
{
/* The user has to explicitly compile sablotron with sablotron 
   encoding functions in order for SablotSetEncoding to be 
   enabled.  config.m4 automatically checks for this... */

#ifdef HAVE_SABLOT_SET_ENCODING
	zval       **processor_p,  /* Resource Pointer to a PHP-XSLT processor */
	           **encoding;     /* The encoding to use for the output */
	php_xslt    *handle;       /* A PHP-XSLT processor */

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &processor_p, &encoding) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	convert_to_string_ex(encoding);

	/* Set the encoding */
	SablotSetEncoding(XSLT_PROCESSOR(handle), Z_STRVAL_PP(encoding));
#else
	php_error(E_WARNING, "Sablotron not compiled with encoding support");
#endif

}
/* }}} */

/* {{{ proto void xslt_set_log(resource processor, string logfile)
   Set the log file to write the errors to (defaults to stderr) */
PHP_FUNCTION(xslt_set_log)
{
	zval      **processor_p,             /* Resource pointer to a PHP-XSLT processor */
	          **logfile;                 /* Path to the logfile */
	php_xslt   *handle;                  /* A PHP-XSLT processor */
	int         argc = ZEND_NUM_ARGS();  /* Argument count */

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &processor_p, &logfile) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	
	if (Z_TYPE_PP(logfile) == IS_LONG || 
		Z_TYPE_PP(logfile) == IS_BOOL || 
		Z_TYPE_PP(logfile) == IS_DOUBLE) {
		XSLT_LOG(handle).do_log = Z_LVAL_PP(logfile);
		RETURN_NULL();
	}
	else {
		convert_to_string_ex(logfile);
	}

	/* If the log file already exists, free it */
	if (XSLT_LOG(handle).path) {
		efree(XSLT_LOG(handle).path);
	}
	
	/* Copy the path */
	XSLT_LOG(handle).path = estrndup(Z_STRVAL_PP(logfile),
	                                 Z_STRLEN_PP(logfile));
}
/* }}} */

/* {{{ S_DOM *_php_sablot_dom_proc(php_xslt *, char *, size_t, zval **)
 */
static SDOM_Document 
_php_sablot_dom_proc(php_xslt *h, char *datap, size_t data_len, zval **args)
{
	SDOM_Document domtree;

	if (!strncmp(datap, "arg:", 4)) {
		zval **data;

		if (zend_hash_find(Z_ARRVAL_PP(args), 
						   datap + 4, 
						   data_len - 3,
						   (void **) &data) == FAILURE && 
			zend_hash_find(Z_ARRVAL_PP(args),
						   datap + 5,
						   data_len - 4,
						   (void **) &data) == FAILURE) {
			php_error(E_WARNING, "Cannot find argument: %s", datap);
			return NULL;
		}

		SablotParseBuffer(XSLT_CONTEXT(h), Z_STRVAL_PP(data), &domtree);
	}
	else {
		SablotParse(XSLT_CONTEXT(h), datap, &domtree);
	}

	return domtree;
}
/* }}} */

/* {{{ proto string xslt_process(resource processor, string xml, string xslt[, mixed result[, array args[, array params]]])
   Perform the xslt transformation */
PHP_FUNCTION(xslt_process)
{
	zval          **processor_p;    /* Resource Pointer to a PHP-XSLT processor */
	zval          **xml_p;          /* A zval pointer to the XML data */
	zval          **xsl_p;          /* A zval pointer to the XSL data */
	zval          **result_p;       /* A zval pointer to the transformation results */
	zval          **params_p;       /* A zval pointer to the XSL parameters array */
	zval          **args_p;         /* A zval pointer to the XSL arguments array */
	zval          **data;           /* Parameter data */
	php_xslt       *handle;         /* A PHP-XSLT processor */
	SDOM_Document   xml_dom;        /* XML DOM tree to SablotRunProcessorGen */
	SDOM_Document   xsl_dom;        /* XSL DOM tree to SablotRunProcessorGen */
	char           *string_key;     /* String key from xslt_process() */
	char           *result;         /* The result file or argument buffer */
	int             argc;           /* The number of arguments given */
	int             error;          /* Our error container */
	ulong           num_key;        /* Numerical key */

	argc = ZEND_NUM_ARGS();

	if (argc < 3 || argc > 6 ||
	    zend_get_parameters_ex(argc, &processor_p, &xml_p, &xsl_p, 
							   &result_p, &args_p, &params_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	/* Convert the XML and XSLT strings to S_DOM * handles */
	convert_to_string_ex(xml_p);
	convert_to_string_ex(xsl_p);

	xml_dom = _php_sablot_dom_proc(handle, 
								   Z_STRVAL_PP(xml_p), 
								   Z_STRLEN_PP(xml_p), 
								   args_p);
	if (!xml_dom) {
		RETURN_FALSE;
	}

	xsl_dom = _php_sablot_dom_proc(handle, 
								   Z_STRVAL_PP(xsl_p),
								   Z_STRLEN_PP(xsl_p), 
								   args_p);
	if (!xsl_dom) {
		RETURN_FALSE;
	}

	SablotAddArgTree(handle->processor.ctx,
					 XSLT_PROCESSOR(handle),
					 "xml",
					 xml_dom);
	SablotAddArgTree(XSLT_CONTEXT(handle),
					 XSLT_PROCESSOR(handle),
					 "xsl",
					 xsl_dom);

	/* Well, no result file was given or result buffer, that means (guess what?)
	 * we're returning the result yipp di doo! 
	 */
	if (argc < 4 || Z_TYPE_PP(result_p) == IS_NULL) {
		result = "arg:/result";
		SablotAddArgBuffer(XSLT_CONTEXT(handle), 
						   XSLT_PROCESSOR(handle), 
						   result, 
						   NULL);
	}
	/* The result buffer to place the data into, either a file or 
	 * an argument buffer, etc. 
	 */
	else {
		convert_to_string_ex(result_p);
		result = Z_STRVAL_PP(result_p);
	}

	/* Add the XSLT parameters */
	if (argc > 5) {
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(params_p));
			 zend_hash_get_current_data(Z_ARRVAL_PP(params_p), (void **) &data) == FAILURE;
			 zend_hash_move_forward(Z_ARRVAL_PP(params_p))) {
			if (zend_hash_get_current_key(Z_ARRVAL_PP(params_p), 
										  &string_key, 
										  &num_key, 
										  0) == HASH_KEY_IS_LONG) {
				php_error(E_WARNING, "Only string keys are allowed as XSLT parameters");
				RETURN_FALSE;
			}
			
			SablotAddParam(XSLT_CONTEXT(handle), 
						   XSLT_PROCESSOR(handle), 
						   string_key, 
						   Z_STRVAL_PP(data));
		}
	}

	/* Perform transformation */
	error = SablotRunProcessorGen(XSLT_CONTEXT(handle),
								  XSLT_PROCESSOR(handle), 
								  "arg:/xsl",
								  "arg:/xml",
								  result);
	if (error) {
		XSLT_ERRNO(handle) = error;
		RETURN_FALSE;
	}

	SablotDestroyDocument(XSLT_CONTEXT(handle), xml_dom);
	SablotDestroyDocument(XSLT_CONTEXT(handle), xsl_dom);

	/* If the result buffer is specified, then we return the results of the XSLT
	   transformation */
	if (!strcmp(result, "arg:/result")) {
		char *trans_result;

		/* Fetch the result buffer into trans_result */
		error = SablotGetResultArg(XSLT_PROCESSOR(handle), 
								   "arg:/result", 
								   &trans_result);
		if (error) {
			/* Save the error number */
			XSLT_ERRNO(handle) = error;
			RETURN_FALSE;
		}

		RETVAL_STRING(trans_result, 1);
		SablotFree(trans_result);
	}
	else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto void xslt_free(resource processor)
   Free the xslt processor up */
PHP_FUNCTION(xslt_free)
{
	zval     **processor_p;   /* Resource pointer to a php-xslt processor */
	php_xslt  *handle;        /* A PHP-XSLT processor */
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &processor_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	
	/* Remove the entry from the list */
	zend_list_delete(Z_LVAL_PP(processor_p));
}
/* }}} */

/* {{{ proto int xslt_errno(resource processor)
   Error number */
PHP_FUNCTION(xslt_errno)
{
	zval        **processor_p;   /* Resource pointer to a PHP-XSLT processor */
	php_xslt     *handle;        /* A PHP-XSLT processor */

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &processor_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	RETURN_LONG(XSLT_ERRNO(handle));
}
/* }}} */

/* {{{ proto string xslt_error(resource processor)
   Error string */
PHP_FUNCTION(xslt_error)
{
	zval      **processor_p;  /* Resource pointer to a PHP-XSLT processor */
	php_xslt   *handle;       /* A PHP-XSLT processor */

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &processor_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	if(XSLT_ERRSTR(handle)) {
		RETURN_STRING(XSLT_ERRSTR(handle), 1);	
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ free_processor()
   Free an XSLT processor */
static void free_processor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_xslt *handle = (php_xslt *) rsrc->ptr;     /* A PHP-XSLT processor */
	
	/* Free the processor */
	if (XSLT_PROCESSOR(handle)) {
		SablotUnregHandler(XSLT_PROCESSOR(handle), HLR_MESSAGE, NULL, NULL);
		SablotUnregHandler(XSLT_PROCESSOR(handle), HLR_SAX,     NULL, NULL);
		SablotUnregHandler(XSLT_PROCESSOR(handle), HLR_SCHEME,  NULL, NULL);
		SablotDestroyProcessor(XSLT_PROCESSOR(handle));
	}

	if (XSLT_CONTEXT(handle)) {
		SablotDestroySituation(XSLT_CONTEXT(handle));
	}

	/* Free Scheme handlers */
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).get_all);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).open);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).get);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).put);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).close);
	/* Free SAX handlers */
	XSLT_FUNCH_FREE(XSLT_SAX(handle).doc_start);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).element_start);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).element_end);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).namespace_start);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).namespace_end);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).comment);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).pi);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).characters);
	XSLT_FUNCH_FREE(XSLT_SAX(handle).doc_end);
	/* Free error handler */
	XSLT_FUNCH_FREE(XSLT_ERROR(handle));

	/* Free error message, if any */
	if (XSLT_ERRSTR(handle)) {
		efree(XSLT_ERRSTR(handle));
	}

	/* Close log file */
	if (XSLT_LOG(handle).fd) {
		close(XSLT_LOG(handle).fd);
	}
	
	/* Free log file path */
	if (XSLT_LOG(handle).path) {
		efree(XSLT_LOG(handle).path);
	}

	/* Free up the handle */
	efree(handle->handlers);
	efree(handle->err);
	efree(handle);
}
/* }}} */

/* {{{ register_sax_handler_pair()
   Register a pair of sax handlers */
static void register_sax_handler_pair(zval **handler1, zval **handler2, zval **handler)
{
	zval **current;   /* The current handler we're grabbing */
	
	/* Grab and assign handler 1 */
	if (zend_hash_index_find(Z_ARRVAL_PP(handler), 0, (void **) &current) == SUCCESS) {
		*handler1 = *current;
		zval_add_ref(handler1);
	}
	else {
		php_error(E_WARNING, "Wrong format of arguments to xslt_set_sax_handlers()");
		return;
	}
	
	/* Grab and assign handler 2 */
	if (zend_hash_index_find(Z_ARRVAL_PP(handler), 1, (void **) &current) == SUCCESS) {
		*handler2 = *current;
		zval_add_ref(handler2);
	}
	else {
		php_error(E_WARNING, "Wrong format of arguments to xslt_set_sax_handlers()");
		return;
	}
}
/* }}} */

/* {{{ scheme_getall()
   The getall scheme handler */
static int scheme_getall(void *user_data, 
						 SablotHandle proc, 
						 const char *scheme, 
						 const char *rest, 
						 char **buffer, 
						 int *byte_count)
{
	zval       *argv[3];                         /* Arguments to the scheme getall function */
	zval       *retval;                          /* Return value from the scheme getall function */
	php_xslt   *handle = (php_xslt *) user_data; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If the scheme handler get all function doesn't
	   exist, exit out */
	if (!XSLT_SCHEME(handle).get_all) {
		return 0;
	}

	/* Allocate and initialize */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: Scheme         (string)
	   Argument 3: Rest           (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) scheme, 1);
	ZVAL_STRING(argv[2], (char *) rest, 1);

	xslt_call_function("scheme get all", XSLT_SCHEME(handle).get_all, 
	                   3, argv, &retval);

	/* Save the return value in the buffer (copying it) */
	*buffer     = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
	*byte_count = Z_STRLEN_P(retval);

	/* Free return value */
	zval_ptr_dtor(&retval);

	return 0;
}
/* }}} */

/* {{{ scheme_handler_is_registered()
   Check to see if the scheme handler is registered with the user */
static int scheme_handler_is_registered(php_xslt *handle)
{
	/* If one of the functions is exists, then scheme
	   handlers are registered */
	if (XSLT_SCHEME(handle).get_all  ||
	    XSLT_SCHEME(handle).open     ||
	    XSLT_SCHEME(handle).get      ||
	    XSLT_SCHEME(handle).put      ||
	    XSLT_SCHEME(handle).close)
	 	return 1;
	/* otherwise, no cigar */
	else
		return 0;
}
/* }}} */

/* {{{ scheme_freememory()
   Called when sablotron needs to free memory related to scheme handling */
static int scheme_freememory(void *user_data, SablotHandle proc, char *buffer)
{
	/* If we don't have any scheme handler's registered, then emalloc() wasn't
	   used, and if emalloc() wasn't then efree shouldn't be used */
	if (!scheme_handler_is_registered((php_xslt *) user_data)) {
		return 0;
	}

	/* Free the memory using efree() and remove it from the register */
	efree(buffer);
	
	return 0;
}
/* }}} */

/* {{{ scheme_open()
   Called when the URI needs to be opened */
static int  scheme_open(void *user_data, 
						SablotHandle proc, 
						const char *scheme, 
						const char *rest, 
						int *fd)
{
	zval      *argv[3];                         /* Arguments to the scheme open function */
	zval      *retval;                          /* The return value from the scheme open function */
	php_xslt  *handle = (php_xslt *) user_data; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no open handler exists, let's exit */
	if (!XSLT_SCHEME(handle).open) {
		return 0;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	/* Argument 1: XSLT Processor (resource)
	   Argument 2: Scheme         (string)
	   Argument 3: Rest           (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) scheme, 1);
	ZVAL_STRING(argv[2], (char *) rest, 1);
	
	/* Call the function */
	xslt_call_function("scheme open", XSLT_SCHEME(handle).open,
	                   3, argv, &retval);

	/* Return value is a resource pointer to an open file */
	*fd = Z_LVAL_P(retval);

	/* Free it all up */
	zval_ptr_dtor(&retval);

	/* return success */
	return 0;
}
/* }}} */

/* {{{ scheme_get()
   Called when data needs to be fetched from the URI */
static int  scheme_get(void *user_data, SablotHandle proc, int fd, char *buffer, int *byte_count)
{
	zval       *argv[3];                         /* Arguments to the scheme get function  */
	zval       *retval;                          /* Return value from the scheme get function */
	php_xslt   *handle = (php_xslt *) user_data; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no get handler exists, let's exit */
	if (!XSLT_SCHEME(handle).get) {
		return 0;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	/* Argument 1: XSLT Processor (resource)
	   Argument 2: File Pointer   (resource)
	   Argument 3: Data           (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_RESOURCE(argv[1], fd);
	zend_list_addref(fd);
	ZVAL_STRINGL(argv[2], buffer, *byte_count, 0);
	
	/* Call the function */
	xslt_call_function("scheme get", XSLT_SCHEME(handle).get,
	                   3, argv, &retval);
	
	/* Returns the number of bytes read */
	*byte_count = Z_LVAL_P(retval);

	/* Free things up */
	zval_ptr_dtor(&retval);

	/* return success */
	return 0;
}
/* }}} */

/* {{{ scheme_put()
   Called when data needs to be written */
static int  scheme_put(void *user_data, 
					   SablotHandle proc, 
					   int fd, 
					   const char *buffer, 
					   int *byte_count)
{
	zval       *argv[3];                         /* Arguments to the scheme put function */
	zval       *retval;                          /* Return value from the scheme put function */
	php_xslt   *handle = (php_xslt *) user_data; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no put handler exists, let's exit */
	if (!XSLT_SCHEME(handle).put) {
		return 0;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: File pointer   (resource)
	   Argument 3: Data           (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_RESOURCE(argv[1], fd);
	zend_list_addref(fd);
	ZVAL_STRINGL(argv[2], (char *) buffer, *byte_count, 1);
	
	/* Call the scheme put function already */
	xslt_call_function("scheme put", XSLT_SCHEME(handle).put,
	                   3, argv, &retval);

	/* The return value is the number of bytes written */
	*byte_count = Z_LVAL_P(retval);

	/* Free everything up */
	zval_ptr_dtor(&retval);

	/* Return success */
	return 0;
}
/* }}} */

/* {{{ scheme_close()
   Called when its time to close the fd */
static int  scheme_close(void *user_data, SablotHandle proc, int fd)
{
	zval       *argv[2];                         /* Arguments to the scheme close function*/
	zval       *retval;                          /* Return value from the scheme close function */
	php_xslt   *handle = (php_xslt *) user_data; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* if no close handler exists, exit */
	if (!XSLT_SCHEME(handle).close) {
		return 0;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: File pointer   (resource)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_RESOURCE(argv[1], fd);
	zend_list_addref(fd);
	
	/* Call the scheme handler close function */
	xslt_call_function("scheme close", XSLT_SCHEME(handle).close,
	                   2, argv, &retval);

	/* Free everything up */
	zval_ptr_dtor(&retval);

	/* Return success */
	return 0;
}
/* }}} */

/* {{{ sax_startdoc()
   Called when the document starts to be processed */
static SAX_RETURN sax_startdoc(void *ctx, SablotHandle proc)
{
	zval       *argv[1];                    /* Arguments to the sax start doc function */
	zval       *retval;                     /* Return value from sax start doc function */
	php_xslt   *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* if no document start function exists, exit */
	if (!XSLT_SAX(handle).doc_start) {
		return;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);

	/* Argument 1:  XSLT processor (resource) */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	
	/* Call the Sax start doc function */
	xslt_call_function("sax start doc", XSLT_SAX(handle).doc_start,
	                   1, argv, &retval);

	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_startelement()
   Called when an element is begun to be processed */
static SAX_RETURN sax_startelement(void *ctx, 
								   SablotHandle proc,
                                   const char  *name, 
                                   const char **attr)
{
	zval       *argv[3];                   /* Arguments to the sax start element function */
	zval       *retval;                    /* Return value from the sax start element function */
	php_xslt   *handle = (php_xslt *) ctx; /* A PHP-XSLT processor */
	char      **p;                         /* Pointer to attributes */
    TSRMLS_FETCH();
    
	/* If no element start function is found, exit */
	if (!XSLT_SAX(handle).element_start) {
		return;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);
	array_init(argv[2]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: Element name   (string)
	   Argument 3: Element attributes (array)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) name, 1);

	/* loop through the attributes array, copying it onto our
	   php array */
	p = (char **) attr;
	while (p && *p) {
		add_assoc_string(argv[2], *p, *(p + 1), 1);
		p += 2;
	}

	/* Call the sax element start function */
	xslt_call_function("sax start element", XSLT_SAX(handle).element_start, 
	                   3, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_endelement()
   Called when an ending XML element is encountered */
static SAX_RETURN sax_endelement(void *ctx, SablotHandle proc, const char *name)
{
	zval        *argv[2];                   /* Arguments to the sax end element function */
	zval        *retval;                    /* Return value from the sax end element function */
	php_xslt    *handle = (php_xslt *) ctx; /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no element end function exists, exit */
	if (!XSLT_SAX(handle).element_end) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: Element name   (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) name, 1);

	/* Call the sax end element function */
	xslt_call_function("sax end element", XSLT_SAX(handle).element_end,
	                   2, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_startnamespace()
   Called at the beginning of the parsing of a new namespace */
static SAX_RETURN sax_startnamespace(void *ctx, 
									 SablotHandle proc,
                                     const char *prefix, 
                                     const char *uri)
{
	zval       *argv[3];                    /* Arguments to the sax start namespace function */
	zval       *retval;                     /* Return value from the sax start namespace function */
	php_xslt   *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* if no namespace start function exists, exit */
	if (!XSLT_SAX(handle).namespace_start) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: Prefix         (string)
	   Argument 3: URI            (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) prefix, 1);
	ZVAL_STRING(argv[2], (char *) uri, 1);

	/* Call the sax start namespace function */
	xslt_call_function("sax start namespace", XSLT_SAX(handle).namespace_start,
	                   3, argv, &retval);

	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_endnamespace()
   Called when a new namespace is finished being parsed */
static SAX_RETURN sax_endnamespace(void *ctx, SablotHandle proc, const char *prefix)
{
	zval        *argv[2];                    /* Arguments to the sax end namespace function */
	zval        *retval;                     /* Return value from the sax end namespace function */
	php_xslt    *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no namespace end function exists, exit */
	if (!XSLT_SAX(handle).namespace_end) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	
	/* Argument 1: XSLT processor (resource)
	   Argument 2: Prefix         (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) prefix, 1);
	
	/* Call the sax end namespace function */
	xslt_call_function("sax end namespace", XSLT_SAX(handle).namespace_end,
	                   2, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_comment()
   Called when a comment is found */
static SAX_RETURN sax_comment(void *ctx, SablotHandle proc, const char *contents)
{
	zval        *argv[2];                    /* Arguments to the sax comment function */
	zval        *retval;                     /* Return value from the sax comment function */
	php_xslt    *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* if no comment function exists, exit */
	if (!XSLT_SAX(handle).comment) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	
	/* Argument 1: XSLT processor   (resource)
	   Argument 2: Comment contents (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) contents, 1);
	
	/* Call the sax comment function */
	xslt_call_function("sax comment", XSLT_SAX(handle).comment,
	                   2, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_pi()
   Called when processing instructions are found */
static SAX_RETURN sax_pi(void *ctx, 
						 SablotHandle proc,
                         const char *target, 
                         const char *contents)
{
	zval        *argv[3]; /* Arguments to the sax processing instruction function */
	zval        *retval;  /* Return value from the sax processing instruction function */
	php_xslt    *handle;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();

	handle = (php_xslt *) ctx;
    
	/* If no processing instructions function exists, exit */
	if (!XSLT_SAX(handle).pi) {
		return;
	}

	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);
	
	/* Argument 1: XSLT processor     (resource)
	   Argument 2: Target of the pi   (string)
	   Argument 3: Contents of the pi (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRING(argv[1], (char *) target, 1);
	ZVAL_STRING(argv[2], (char *) contents, 1);

	/* Call processing instructions function */
	xslt_call_function("sax processing instructions", XSLT_SAX(handle).pi,
	                   3, argv, &retval);

	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_characters()
   Called when characters are come upon */
static SAX_RETURN sax_characters(void *ctx,
								 SablotHandle proc,
                                 const char *contents, 
                                 int length)
{
	zval         *argv[2];                    /* Arguments to the sax characters function */
	zval         *retval;                     /* Return value to the sax characters function */
	php_xslt     *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no characters function exists, exit */
	if (!XSLT_SAX(handle).characters) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);

	/* Argument 1: XSLT processor (resource)
	   Argument 2: Contents       (string)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	ZVAL_STRINGL(argv[1], (char *) contents, length, 1);

	/* Call characters function */
	xslt_call_function("sax characters", XSLT_SAX(handle).characters,
	                   2, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_enddoc()
   Called when the document is finished being parsed */
static SAX_RETURN sax_enddoc(void *ctx, SablotHandle proc)
{
	zval        *argv[1];                    /* Arguments to the end document function */
	zval        *retval;                     /* Return value from the end document function */
	php_xslt    *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();

	/* If no end document function exists, exit */
	if (!XSLT_SAX(handle).doc_end) {
		return;
	}
	
	/* Allocate and initialize arguments */
	MAKE_STD_ZVAL(argv[0]);

	/* Argument 1: XSLT Processor (resource)
	 */
	ZVAL_RESOURCE(argv[0], handle->processor.idx);
	zend_list_addref(handle->processor.idx);
	
	/* Call the function */
	xslt_call_function("sax end doc", XSLT_SAX(handle).doc_end,
	                   1, argv, &retval);
	
	/* Cleanup */
	zval_ptr_dtor(&retval);
}
/* }}} */


/* {{{ _error_parse()
   Parse a char ** array into an _error_fields structure */
struct _error_fields {
	char *message;
	char *type;
	char *line;
};

static void
_error_parse (char **fields, struct _error_fields *xse)
{
	char *val;
	char *p;
	int   pos;
	int   len;

	/* Prep structure */
	memset(xse, 0, sizeof(struct _error_fields));

	while (*fields) {
		p = strchr(*fields, ':');
		if (p == NULL)
			continue;

		len = strlen(*fields);

		pos = p - *fields;
		val = estrndup(*fields + pos + 1, len - pos);

		if (!strncmp(*fields, "msg", MIN(3, pos+1))) {
			xse->message = val;
		}
		else if (!strncmp(*fields, "type", MIN(4, pos+1))) {
			xse->type = val;
		}
		else if (!strncmp(*fields, "line", MIN(4, pos+1))) {
			xse->line = val;
		}

		++fields;
	}

	if (!xse->type) 
		xse->type = estrdup("unknown error type");

	if (!xse->line)
		xse->line = estrdup("unknown line");

	if (!xse->message)
		xse->message = estrdup("unknown error");
}
/* }}} */

/* {{{ _free_error_field_struct()
   Free an error field structure created by _error_parse() */
static void
_free_error_field_struct(struct _error_fields *xse)
{
	if (xse->message) efree(xse->message);
	if (xse->type) efree(xse->type);
	if (xse->line) efree(xse->line);
}
/* }}} */

/* {{{ error_makecode()
   Make the error code */
static MH_ERROR error_makecode(void *user_data, 
							   SablotHandle proc, 
							   int severity, 
							   unsigned short facility, 
							   unsigned short code)
{
	return 0;
}
/* }}} */

/* {{{ error_log()
   Called when its time to log data */
static MH_ERROR error_log(void *user_data, 
						  SablotHandle proc, 
						  MH_ERROR code, 
						  MH_LEVEL level, 
						  char **fields)
{
	php_xslt             *handle = (php_xslt *) user_data;  /* A PHP-XSLT processor */
	struct _error_fields  err;                              /* Error structure */
	char                 *msgbuf;                           /* Message buffer */
	int                   msgbuf_len;                       /* Message buffer len */
	int                   error = 0;                        /* Error container */

#define msgformat "Sablotron message on line %s, level %s: %s\n"

	/* Skip, if logging is disabled */
	if (!XSLT_LOG(handle).do_log)
		return 0;
	
	/* parse error list into a structure */
	_error_parse(fields, &err);

	/* Allocate the message buf and copy the data into it */
	msgbuf_len = (sizeof(msgformat) - 6) +
	              strlen(err.message) +
	              strlen(err.line) +
	              strlen(err.type);
	msgbuf = emalloc(msgbuf_len + 1);
	snprintf(msgbuf, msgbuf_len, msgformat, err.line, err.type, err.message);

	/* If the error is serious enough, copy it to our error buffer 
	   which will show up when someone calls the xslt_error() function */
	if (level == MH_LEVEL_WARN  ||
	    level == MH_LEVEL_ERROR ||
	    level == MH_LEVEL_CRITICAL) {
		if (XSLT_ERRSTR(handle))
			efree(XSLT_ERRSTR(handle));
		
		XSLT_ERRSTR(handle) = estrndup(msgbuf, msgbuf_len);
	}

	/* If we haven't allocated and opened the file yet */
	if (!XSLT_LOG(handle).fd) {
		/* Lets open up a file */
		if (XSLT_LOG(handle).path && strcmp(XSLT_LOG(handle).path, "php://stderr")) {
			/* open for append */
			XSLT_LOG(handle).fd = open(XSLT_LOG(handle).path, 
			                           O_WRONLY|O_CREAT|O_APPEND,
			                           S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR);
			if (XSLT_LOG(handle).fd == -1) {
				php_error(E_WARNING, "Cannot open log file, %s [%d]: %s",
				          XSLT_LOG(handle).path, errno, strerror(errno));
				XSLT_LOG(handle).fd = 0;
			}
		}
		/* Default is stderr, or if the user provided "php://stderr" that's the
		   stream */
		else {
			XSLT_LOG(handle).fd = 2;
		}
	}
	
	/* Write the error to the file */
	error = write(XSLT_LOG(handle).fd, msgbuf, msgbuf_len);
	if (error == -1) {
		php_error(E_WARNING, "Cannot write data to log file, %s, with fd, %d [%d]: %s",
		          (XSLT_LOG(handle).path ? XSLT_LOG(handle).path : "stderr"),
		          XSLT_LOG(handle).fd,
		          errno,
		          strerror(errno));
		return 0;
	}

	/* Cleanup */
	if (msgbuf) efree(msgbuf);
	_free_error_field_struct(&err);
	
	return 0;
}
/* }}} */

/* {{{ error_print()
   Print out an error message or call the error handler */
static MH_ERROR error_print(void *user_data, 
							SablotHandle proc, 
							MH_ERROR code, 
							MH_LEVEL level, 
							char **fields)
{
	php_xslt             *handle = (php_xslt *) user_data;  /* A PHP-XSLT processor */
	struct _error_fields  err;                              /* Error field structure */
	
	_error_parse(fields, &err);

	if (XSLT_ERROR(handle)) {
		zval   *argv[4];   /* Arguments to the error function */
		zval   *retval;    /* Return value from the error function */
        TSRMLS_FETCH();
        
		/* Allocate and initialize */
		MAKE_STD_ZVAL(argv[0]);
		MAKE_STD_ZVAL(argv[1]);
		MAKE_STD_ZVAL(argv[2]);
		MAKE_STD_ZVAL(argv[3]);
		array_init(argv[3]);

		/* Argument 1: XSLT Processor (resource)
		   Argument 2: Error level    (long)
		   Argument 3: Error code     (long)
		   Argument 4: Error messages (array)
		 */
		ZVAL_RESOURCE(argv[0], handle->processor.idx);
		zend_list_addref(handle->processor.idx);
		ZVAL_LONG(argv[1], level);
		ZVAL_LONG(argv[2], code);

		add_assoc_string_ex(argv[3], "type", sizeof("type") - 1, err.type, 0);
		add_assoc_string_ex(argv[3], "message", sizeof("message") - 1, err.message, 0);
		add_assoc_long_ex(argv[3], "line", sizeof("line") - 1, atoi(err.line));

		/* Call the function */
		xslt_call_function("error handler", XSLT_ERROR(handle),
		                   4, argv, &retval);

		/* Free up */
		zval_ptr_dtor(&retval);
	}
	else {
		char   *msgbuf;     /* Message buffer */
		size_t  msgbuf_len; /* Message buffer length */

#define msgformat "Sablotron error on line %s: %s"

		/* If the error is not serious, exit out */
		if (code == MH_LEVEL_WARN  || 
		    code == MH_LEVEL_ERROR || 
		    code == MH_LEVEL_CRITICAL) {
			return 0;
		}

		/* Allocate the message buffer and copy the data onto it */
		msgbuf_len = (sizeof(msgformat) - 4) + 
			strlen(err.message) + 
			strlen(err.line);
		msgbuf = emalloc(msgbuf_len + 1);

		snprintf(msgbuf, msgbuf_len, msgformat, err.line, err.message);

		/* Copy the error message onto the handle for use when 
		   the xslt_error function is called */
		XSLT_ERRSTR(handle) = estrndup(msgbuf, msgbuf_len);

		/* Output a warning */
		php_error(E_WARNING, msgbuf);

		/* Cleanup */
		efree(msgbuf);
		_free_error_field_struct(&err);
	}

	return 0;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
