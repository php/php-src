/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
   |          David Viner <dviner@php.net>                                |
   |          Lenar Lohmus <flex@php.net>                                 |
   |          Melvyn Sopacua <msopacua@php.net>                           |
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

#ifdef HAVE_SABLOT_CONFIG
#include "php_sab_info.h"
#endif
#include <sablot.h>

#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* functions relating to handlers */
static void register_sax_handler_pair(zval **, zval **, zval ** TSRMLS_DC);

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
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

function_entry xslt_functions[] = {
	PHP_FE(xslt_create,              NULL)
	PHP_FE(xslt_set_sax_handlers,    NULL)
	PHP_FE(xslt_set_scheme_handlers, NULL)
	PHP_FE(xslt_set_error_handler,   NULL)
	PHP_FE(xslt_set_base,		 NULL)
#ifdef HAVE_SABLOT_SET_ENCODING
	PHP_FE(xslt_set_encoding,        NULL)
#endif
	PHP_FE(xslt_set_log,             NULL)
	PHP_FE(xslt_process,             NULL)
	PHP_FE(xslt_error,               NULL)
	PHP_FE(xslt_errno,               NULL)
	PHP_FE(xslt_free,                NULL)
	PHP_FE(xslt_set_object,          second_args_force_ref)
	PHP_FE(xslt_setopt,              NULL)
#ifdef HAVE_SABLOT_GET_OPTIONS
	PHP_FE(xslt_getopt,              NULL)
#endif
	PHP_FE(xslt_backend_version,     NULL)
	PHP_FE(xslt_backend_name,     	 NULL)
    PHP_FE(xslt_backend_info,        NULL)
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

	/* Generic options, which can apply to 'all' xslt processors */
	REGISTER_LONG_CONSTANT("XSLT_OPT_SILENT", SAB_NO_ERROR_REPORTING, CONST_CS | CONST_PERSISTENT);

	/* Error constants, which are useful in userspace. */
	REGISTER_LONG_CONSTANT("XSLT_ERR_UNSUPPORTED_SCHEME", SH_ERR_UNSUPPORTED_SCHEME, CONST_CS | CONST_PERSISTENT);
	/* Sablotron specific options */
	REGISTER_LONG_CONSTANT("XSLT_SABOPT_PARSE_PUBLIC_ENTITIES", SAB_PARSE_PUBLIC_ENTITIES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSLT_SABOPT_DISABLE_ADDING_META", SAB_DISABLE_ADDING_META, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSLT_SABOPT_DISABLE_STRIPPING", SAB_DISABLE_STRIPPING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSLT_SABOPT_IGNORE_DOC_NOT_FOUND", SAB_IGNORE_DOC_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
/* hack: implemented at the same time, so should work.
   Otherwise we need to check the enum type of SablotFlag in <sablot.h> */
#ifdef HAVE_SABLOT_GET_OPTIONS
	REGISTER_LONG_CONSTANT("XSLT_SABOPT_FILES_TO_HANDLER", SAB_FILES_TO_HANDLER, CONST_CS | CONST_PERSISTENT);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xslt)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XSLT support", "enabled");
	php_info_print_table_row(2, "Backend", "Sablotron");
#ifdef SAB_VERSION
	php_info_print_table_row(2, "Sablotron Version", SAB_VERSION);
#endif
#ifdef HAVE_SABLOT_CONFIG
	php_info_print_table_row(2, "Sablotron Information", SAB_INFO);
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource xslt_create(void) 
   Create a new XSLT processor */
PHP_FUNCTION(xslt_create)
{
	php_xslt     *handle;      /* The php -> sablotron handle */
	SablotHandle  processor;   /* The sablotron processor */
	SablotSituation  sit;        /* The sablotron Situation handle */
	int           error;       /* The error container */

	/* Allocate the php-sablotron handle */
	handle                   = ecalloc(1, sizeof(php_xslt));
	handle->handlers         = ecalloc(1, sizeof(struct xslt_handlers));
	handle->err              = ecalloc(1, sizeof(struct xslt_error));
	handle->object           = NULL;
	handle->base_isset       = 0;

	XSLT_LOG(handle).path = NULL;

	/* Allocate the actual processor itself, via sablotron */
	SablotCreateSituation(&sit);
	error = SablotCreateProcessorForSituation(sit, &processor);
	if (error) {
		XSLT_ERRNO(handle) = error;
		RETURN_FALSE;
	}

	/* Save the processor and set the default handlers */
	XSLT_PROCESSOR(handle) = processor;
	XSLT_SITUATION(handle) = sit;
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
	
	/* Convert the sax_handlers_p zval ** to a hash table we can process */
	sax_handlers = HASH_OF(*sax_handlers_p);
	if (!sax_handlers) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expecting an array as the second argument");
		return;
	}

	/* Loop through the HashTable containing the SAX handlers */
	for (zend_hash_internal_pointer_reset(sax_handlers);
	     zend_hash_get_current_data(sax_handlers, (void **) &handler) == SUCCESS;
		 zend_hash_move_forward(sax_handlers)) {

		key_type = zend_hash_get_current_key(sax_handlers, &string_key, &num_key, 0);
		if (key_type == HASH_KEY_IS_LONG) {
			convert_to_string_ex(handler);
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Skipping numerical index %d (with value %s)",
			          num_key, Z_STRVAL_PP(handler));
			continue;
		}

		/* Document handlers (document start, document end) */
		if (strcasecmp(string_key, "document") == 0) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).doc_start, 
			                          &XSLT_SAX(handle).doc_end, 
			                          handler TSRMLS_CC);
		}
		/* Element handlers, start of an element, and end of an element */
		else if (strcasecmp(string_key, "element") == 0) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).element_start, 
			                          &XSLT_SAX(handle).element_end, 
			                          handler TSRMLS_CC);
		}
		/* Namespace handlers, start of a namespace, end of a namespace */
		else if (strcasecmp(string_key, "namespace") == 0) {
			SEPARATE_ZVAL(handler);
			register_sax_handler_pair(&XSLT_SAX(handle).namespace_start, 
			                          &XSLT_SAX(handle).namespace_end, 
			                          handler TSRMLS_CC);
		}
		/* Comment handlers, called when a comment is reached */
		else if (strcasecmp(string_key, "comment") == 0) {
			XSLT_SAX(handle).comment = *handler;
			zval_add_ref(&XSLT_SAX(handle).comment);
		}
		/* Processing instructions handler called when processing instructions
		   (<? ?>) */
		else if (strcasecmp(string_key, "pi") == 0) {
			XSLT_SAX(handle).pi = *handler;
			zval_add_ref(&XSLT_SAX(handle).pi);
		}
		/* Character handler, called when data is found */
		else if (strcasecmp(string_key, "character") == 0) {
			XSLT_SAX(handle).characters = *handler;
			zval_add_ref(&XSLT_SAX(handle).characters);
		}
		/* Invalid handler name, tsk, tsk, tsk :) */
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option: %s", string_key);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "2nd argument must be an array");
		return;
	}

	/* Loop through the scheme handlers array, setting the given
	   scheme handlers */
	for (zend_hash_internal_pointer_reset(scheme_handlers);
	     zend_hash_get_current_data(scheme_handlers, (void **) &handler) == SUCCESS;
	     zend_hash_move_forward(scheme_handlers)) {

		key_type = zend_hash_get_current_key(scheme_handlers, &string_key, &num_key, 0);
		if (key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Numerical key %d (with value %s) being ignored",
					  num_key, Z_STRVAL_PP(handler));
			continue;
		}

		/* Open the URI and return the whole string */
		if (strcasecmp(string_key, "get_all") == 0) {
			assign_handle = &XSLT_SCHEME(handle).sh_get_all;
		}
		/* Open the URI and return a handle */
		else if (strcasecmp(string_key, "open") == 0) {
			assign_handle = &XSLT_SCHEME(handle).sh_open;
		}
		/* Retrieve data from the URI */
		else if (strcasecmp(string_key, "get") == 0) {
			assign_handle = &XSLT_SCHEME(handle).sh_get;
		}
		/* Save data to the URI */
		else if (strcasecmp(string_key, "put") == 0) {
			assign_handle = &XSLT_SCHEME(handle).sh_put;
		}
		/* Close the URI */
		else if (strcasecmp(string_key, "close") == 0) {
			assign_handle = &XSLT_SCHEME(handle).sh_close;
		}
		/* Invalid handler name */
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid option '%s', skipping", string_key);
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
	XSLT_BASE_ISSET(handle) = 1;
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
	
	if (Z_TYPE_PP(logfile) == IS_LONG || Z_TYPE_PP(logfile) == IS_BOOL || Z_TYPE_PP(logfile) == IS_DOUBLE) {
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

/* {{{ proto string xslt_process(resource processor, string xml, string xslt[, mixed result[, array args[, array params]]])
   Perform the xslt transformation */
PHP_FUNCTION(xslt_process)
{
	zval       **processor_p,             /* Resource Pointer to a PHP-XSLT processor */
	           **xml_p,                   /* A zval pointer to the XML data */
	           **xslt_p,                  /* A zval pointer to the XSLT data */
	           **result_p,                /* A zval pointer to the transformation results */
	           **params_p,                /* A zval pointer to the XSLT parameters array */
	           **args_p;                  /* A zval pointer to the XSLT arguments array */
	php_xslt    *handle;                  /* A PHP-XSLT processor */
	char       **params = NULL;           /* A Sablotron parameter array (derived from the zval parameter array) */
	char       **args   = NULL;           /* A Sablotron argument array (derived from the zval argument array) */
	char        *xslt;                    /* The XSLT stylesheet or argument buffer (pointer to xslt_p) */
	char        *xml;                     /* The XML stylesheet or argument buffer (pointer to xml_p) */
	char        *result;                  /* The result file or argument buffer */
	int          argc = ZEND_NUM_ARGS();  /* The number of arguments given */
	int          error;                   /* Our error container */
	int          i;                       /* iterator for Situation */

	if (argc < 3 || argc > 6 ||
	    zend_get_parameters_ex(argc, &processor_p, &xml_p, &xslt_p, &result_p, &args_p, &params_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	convert_to_string_ex(xml_p);
	convert_to_string_ex(xslt_p);

	xml  = Z_STRVAL_PP(xml_p);
	xslt = Z_STRVAL_PP(xslt_p);

	/* Well, no result file was given or result buffer, that means (guess what?)
	   we're returning the result yipp di doo! */
	if (argc < 4 || Z_TYPE_PP(result_p) == IS_NULL) {
		result = "arg:/_result";
	}
	/* The result buffer to place the data into, either a file or an argument buffer, etc. */
	else {
		convert_to_string_ex(result_p);
		result = Z_STRVAL_PP(result_p);
	}

	/* Translate a PHP array into a Sablotron array */
	if (argc > 4) {
		xslt_make_array(args_p, &args);
		/* Can return NULL */
		if (args) {
			TSRMLS_FETCH();
			i=0;
			while (args[i]) {
				/* We can safely add args[i+1] since xslt_make_array sets args[i] to NULL if
					a key on the array is missing. */
				/* For now, we don't care about the error. So don't store it. */
				SablotAddArgBuffer(XSLT_SITUATION(handle), XSLT_PROCESSOR(handle), args[i], args[i+1]);
				i += 2;
			}

			/* Since we have args passed, we need to set the base uri, so pull in executor
				globals and set the base, using the current filename, specifcally for the
				'arg' scheme */
			if(XSLT_BASE_ISSET(handle) == 0)
			{
				char *baseuri;
				spprintf(&baseuri, 0, "file://%s", zend_get_executed_filename(TSRMLS_C));
				SablotSetBaseForScheme(XSLT_PROCESSOR(handle), "arg", baseuri);

				if(baseuri)
					efree(baseuri);
			}
		}
	}
	
	if (argc > 5) {
		xslt_make_array(params_p, &params);
		/* Can return NULL */
		if (params) {
			i=0;
			while (params[i]) {
				SablotAddParam(XSLT_SITUATION(handle), XSLT_PROCESSOR(handle), params[i], params[i+1]);
				i += 2;
			}
		}
	}
	
	/* Perform transformation */
	error = SablotRunProcessorGen(XSLT_SITUATION(handle), XSLT_PROCESSOR(handle), xslt, xml, result);
	if (error) {
		XSLT_ERRNO(handle) = error;

		if (params) xslt_free_array(params);
		if (args)   xslt_free_array(args);

		RETURN_FALSE;
	}

	/* If the result buffer is specified, then we return the results of the XSLT
	   transformation */
	if (!strcmp(result, "arg:/_result")) {
		char *trans_result;

		/* Fetch the result buffer into trans_result */
		error = SablotGetResultArg(handle->processor.ptr, "arg:/_result", &trans_result);
		if (error) {
			/* Save the error number */
			XSLT_ERRNO(handle) = error;
			
			/* Cleanup */
			if (params) xslt_free_array(params);
			if (args)   xslt_free_array(args);
			
			RETURN_FALSE;
		}

		RETVAL_STRING(trans_result, 1);
		SablotFree(trans_result);
	}
	else {
		RETVAL_TRUE;
	}
	
	/* Cleanup */
	if (params) xslt_free_array(params);
	if (args)   xslt_free_array(args);
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

	if (XSLT_ERRSTR(handle)) {
		RETURN_STRING(XSLT_ERRSTR(handle), 1);	
	} else {
		RETURN_FALSE;
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

/* {{{ proto int xslt_set_object(resource parser, object obj)
   sets the object in which to resolve callback functions */
PHP_FUNCTION(xslt_set_object)
{
	zval      *processor_p;  /* Resource pointer to a PHP-XSLT processor */
	zval      *myobj;        /* The object that will handle the callback */
	php_xslt  *handle;       /* A PHP-XSLT processor */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zo", &processor_p, &myobj) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(handle, php_xslt *, &processor_p, -1, le_xslt_name, le_xslt);

	handle->object = myobj;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int xslt_setopt(resource processor, int newmask)
   Set options on a given xsl processor */
PHP_FUNCTION(xslt_setopt)
{
	zval      **processor_p; /* Resource pointer to a PHP-XSLT processor */
	zval      **zbitmask;    /* A bitmask created by through processor specific constants */
	php_xslt  *handle;       /* A PHP-XSLT processor */
	int       error;         /* Error return codes */
	int       newmask;       /* New mask */
#ifdef HAVE_SABLOT_GET_OPTIONS
	int       prevmask;      /* Previous mask */
#endif

	if (ZEND_NUM_ARGS() != 2 ||
		zend_get_parameters_ex(2, &processor_p, &zbitmask) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);
	convert_to_long_ex(zbitmask);

	newmask = Z_LVAL_PP(zbitmask);
	if (newmask < 0) {
		php_error_docref("function.xslt-setopt" TSRMLS_CC, E_WARNING, "Invalid bitmask: %i", newmask);
		RETURN_FALSE;
	}

#ifdef HAVE_SABLOT_GET_OPTIONS
	prevmask = SablotGetOptions(XSLT_SITUATION(handle));
#endif
	error = SablotSetOptions(XSLT_SITUATION(handle), newmask);
	if (error) {
		/* FIXME: Need to analyze the return code to give a more verbose error description */
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to set options");
	}

#ifdef HAVE_SABLOT_GET_OPTIONS
	RETURN_LONG(prevmask);
#else
	RETURN_TRUE;
#endif
}

/* }}} */

#ifdef HAVE_SABLOT_GET_OPTIONS
/* {{{ proto int xslt_getopt(resource processor)
   Get options on a given xsl processor */
PHP_FUNCTION(xslt_getopt)
{
	zval      **processor_p; /* Resource pointer to a PHP-XSLT processor */
	php_xslt  *handle;       /* A PHP-XSLT processor */

	if (ZEND_NUM_ARGS() != 1 ||
		zend_get_parameters_ex(1, &processor_p) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(handle, php_xslt *, processor_p, -1, le_xslt_name, le_xslt);

	RETURN_LONG(SablotGetOptions(XSLT_SITUATION(handle)));
}

/* }}} */
#endif

/* {{{ proto string xslt_backend_version()
   Returns the version number of Sablotron (if available) */
PHP_FUNCTION(xslt_backend_version)
{
#ifdef SAB_VERSION
	RETURN_STRING(SAB_VERSION,1);
#else
	RETURN_FALSE;
#endif	
}
/* }}} */

/* {{{ proto string xslt_backend_name()
   Returns the name of the Backend (here "Sablotron")*/
PHP_FUNCTION(xslt_backend_name)
{
	RETURN_STRING("Sablotron",1);
}
/* }}} */

/* {{{ proto string xslt_backend_info()
   Returns the information on the compilation settings of the backend */
PHP_FUNCTION(xslt_backend_info)
{
#ifdef HAVE_SABLOT_CONFIG
	RETURN_STRING(SAB_INFO, strlen(SAB_INFO));
#else
	RETURN_STRING(XSLT_NO_INFO, strlen(XSLT_NO_INFO));
#endif
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
		SablotDestroySituation(XSLT_SITUATION(handle));
	}

	/* Free Scheme handlers */
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).sh_get_all);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).sh_open);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).sh_get);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).sh_put);
	XSLT_FUNCH_FREE(XSLT_SCHEME(handle).sh_close);
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
static void register_sax_handler_pair(zval **handler1, zval **handler2, zval **handler TSRMLS_DC)
{
	zval **current;   /* The current handler we're grabbing */
	
	/* Grab and assign handler 1 */
	if (zend_hash_index_find(Z_ARRVAL_PP(handler), 0, (void **) &current) == SUCCESS) {
		*handler1 = *current;
		zval_add_ref(handler1);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong format of arguments");
		return;
	}
	
	/* Grab and assign handler 2 */
	if (zend_hash_index_find(Z_ARRVAL_PP(handler), 1, (void **) &current) == SUCCESS) {
		*handler2 = *current;
		zval_add_ref(handler2);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong format of arguments");
		return;
	}
}
/* }}} */

/* {{{ scheme_getall()
   The getall scheme handler */
static int scheme_getall(void *user_data, SablotHandle proc, const char *scheme, const char *rest, char **buffer, int *byte_count)
{
	zval       *argv[3];                           /* Arguments to the scheme getall function */
	zval       *retval;                            /* Return value from the scheme getall function */
	int        result;
	php_xslt   *handle = (php_xslt *) user_data;   /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If the scheme handler get all function doesn't
	   exist, exit out */
	if (!XSLT_SCHEME(handle).sh_get_all) {
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

	xslt_call_function("scheme get all", XSLT_SCHEME(handle).sh_get_all, handle->object, 
	                   3, argv, &retval);

	if (!retval) {
		/* return failure */
		return 1;
	}

	if ((Z_TYPE_P(retval) == IS_BOOL && Z_LVAL_P(retval) == 0) || (Z_TYPE_P(retval) == IS_NULL)) {
		result = 1;
	} else {
		/* Convert the return value to string if needed */
		if (Z_TYPE_P(retval) != IS_STRING)
			convert_to_string_ex(&retval);

		/* Save the return value in the buffer (copying it) */
		*buffer     = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
		*byte_count = Z_STRLEN_P(retval);
		result = 0;
	}

	/* Free return value */
	zval_ptr_dtor(&retval);
		
	return result;
}
/* }}} */

/* {{{ scheme_handler_is_registered()
   Check to see if the scheme handler is registered with the user */
static int scheme_handler_is_registered(php_xslt *handle)
{
	/* If one of the functions is exists, then scheme
	   handlers are registered */
	if (XSLT_SCHEME(handle).sh_get_all  ||
	    XSLT_SCHEME(handle).sh_open     ||
	    XSLT_SCHEME(handle).sh_get      ||
	    XSLT_SCHEME(handle).sh_put      ||
	    XSLT_SCHEME(handle).sh_close)
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
static int  scheme_open(void *user_data, SablotHandle proc, const char *scheme, const char *rest, int *fd)
{
	zval      *argv[3];                            /* Arguments to the scheme open function */
	zval      *retval;                             /* The return value from the scheme open function */
	php_xslt  *handle = (php_xslt *) user_data;    /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no open handler exists, let's exit */
	if (!XSLT_SCHEME(handle).sh_open) {
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
	xslt_call_function("scheme open", XSLT_SCHEME(handle).sh_open, handle->object,
	                   3, argv, &retval);

	if (!retval) {
		/* return failure */
		return 1;
	}

	/* Return value is a resource pointer to an open file */
	*fd = Z_LVAL_P(retval);
		
	/* Free it all up */
	zval_ptr_dtor(&retval);

	if (!*fd) {
		/* return failure - unsupported scheme */
		return SH_ERR_UNSUPPORTED_SCHEME;
	}

	/* return success */
	return 0;
}
/* }}} */

/* {{{ scheme_get()
   Called when data needs to be fetched from the URI */
static int  scheme_get(void *user_data, SablotHandle proc, int fd, char *buffer, int *byte_count)
{
	zval       *argv[3];                           /* Arguments to the scheme get function  */
	zval       *retval;                            /* Return value from the scheme get function */
	php_xslt   *handle = (php_xslt *) user_data;   /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no get handler exists, let's exit */
	if (!XSLT_SCHEME(handle).sh_get) {
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
	xslt_call_function("scheme get", XSLT_SCHEME(handle).sh_get, handle->object,
	                   3, argv, &retval);
	
	if (!retval) {
		/* return failure */
		return 1;
	}
	
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
static int  scheme_put(void *user_data, SablotHandle proc, int fd, const char *buffer, int *byte_count)
{
	zval       *argv[3];                            /* Arguments to the scheme put function */
	zval       *retval;                             /* Return value from the scheme put function */
	php_xslt   *handle = (php_xslt *) user_data;    /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* If no put handler exists, let's exit */
	if (!XSLT_SCHEME(handle).sh_put) {
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
	xslt_call_function("scheme put", XSLT_SCHEME(handle).sh_put, handle->object,
	                   3, argv, &retval);

	if (!retval) {
		/* return failure */
		return 1;
	}
	
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
	zval       *argv[2];                           /* Arguments to the scheme close function*/
	zval       *retval;                            /* Return value from the scheme close function */
	php_xslt   *handle = (php_xslt *) user_data;   /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
	/* if no close handler exists, exit */
	if (!XSLT_SCHEME(handle).sh_close) {
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
	xslt_call_function("scheme close", XSLT_SCHEME(handle).sh_close, handle->object,
	                   2, argv, &retval);

	if (!retval) {
		/* return failure */
		return 1;
	}
	
	/* Free everything up */
	zval_ptr_dtor(&retval);

	/* Return success */
	return 0;
}
/* }}} */

/* {{{ sax_startdoc()
   Called when the document starts to be processed */
static SAX_RETURN sax_startdoc(void *ctx, SablotHandle processor)
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
	xslt_call_function("sax start doc", XSLT_SAX(handle).doc_start, handle->object,
	                   1, argv, &retval);

	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_startelement()
   Called when an element is begun to be processed */
static SAX_RETURN sax_startelement(void *ctx, SablotHandle processor,
                                   const char  *name, 
                                   const char **attr)
{
	zval       *argv[3];                     /* Arguments to the sax start element function */
	zval       *retval;                      /* Return value from the sax start element function */
	php_xslt   *handle = (php_xslt *) ctx;   /* A PHP-XSLT processor */
	char      **p;                           /* Pointer to attributes */
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
	xslt_call_function("sax start element", XSLT_SAX(handle).element_start,  handle->object,
	                   3, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ xslt_sax_endelement()
   Called when an ending XML element is encountered */
static SAX_RETURN sax_endelement(void *ctx, SablotHandle processor, const char *name)
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
	xslt_call_function("sax end element", XSLT_SAX(handle).element_end, handle->object,
	                   2, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_startnamespace()
   Called at the beginning of the parsing of a new namespace */
static SAX_RETURN sax_startnamespace(void *ctx, SablotHandle processor, 
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
	xslt_call_function("sax start namespace", XSLT_SAX(handle).namespace_start, handle->object,
	                   3, argv, &retval);

	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_endnamespace()
   Called when a new namespace is finished being parsed */
static SAX_RETURN sax_endnamespace(void *ctx, SablotHandle processor, const char *prefix)
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
	xslt_call_function("sax end namespace", XSLT_SAX(handle).namespace_end, handle->object,
	                   2, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_comment()
   Called when a comment is found */
static SAX_RETURN sax_comment(void *ctx, SablotHandle processor, const char *contents)
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
	xslt_call_function("sax comment", XSLT_SAX(handle).comment, handle->object,
	                   2, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_pi()
   Called when processing instructions are found */
static SAX_RETURN sax_pi(void *ctx, SablotHandle processor,
                         const char *target, 
                         const char *contents)
{
	zval        *argv[3];                    /* Arguments to the sax processing instruction function */
	zval        *retval;                     /* Return value from the sax processing instruction function */
	php_xslt    *handle = (php_xslt *) ctx;  /* A PHP-XSLT processor */
    TSRMLS_FETCH();
    
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
	xslt_call_function("sax processing instructions", XSLT_SAX(handle).pi, handle->object,
	                   3, argv, &retval);

	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_characters()
   Called when characters are come upon */
static SAX_RETURN sax_characters(void *ctx, SablotHandle processor,
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
	xslt_call_function("sax characters", XSLT_SAX(handle).characters, handle->object,
	                   2, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ sax_enddoc()
   Called when the document is finished being parsed */
static SAX_RETURN sax_enddoc(void *ctx, SablotHandle processor)
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
	xslt_call_function("sax end doc", XSLT_SAX(handle).doc_end, handle->object,
	                   1, argv, &retval);
	
	/* Cleanup */
	if (retval)
		zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ error_makecode()
   Make the error code */
static MH_ERROR error_makecode(void *user_data, SablotHandle proc, int severity, unsigned short facility, unsigned short code)
{
	return 0;
}
/* }}} */

/* {{{ error_log()
   Called when its time to log data */
static MH_ERROR error_log(void *user_data, SablotHandle proc, MH_ERROR code, MH_LEVEL level, char **fields)
{
	php_xslt *handle = (php_xslt *) user_data;                                          /* A PHP-XSLT processor */
	char     *errmsg  = NULL;                                                           /* Error message*/
	char     *errtype = NULL;                                                           /* Error type */
	char     *errline = NULL;                                                           /* Error line */
	char     *msgbuf  = NULL;                                                           /* Message buffer */
	char      msgformat[] = "Sablotron Message on line %s, level %s: %s\n"; /* Message format */
	int       error = 0;                                                                /* Error container */
    TSRMLS_FETCH();

	if (!XSLT_LOG(handle).do_log)
		return 0;
	
	/* Parse the error array */
	/* Loop through the error array */
	if (fields) {
		while (*fields) {
			char *key;  /* Key to for the message */
			char *val;  /* The message itself */
			char *ptr;  /* Pointer to the location of the ':' (separator) */
			int   pos;  /* Position of the ':' (separator) */
			int   len;  /* Length of the string */

			len = strlen(*fields);

			/* Grab the separator's position */
			ptr = strchr(*fields, ':');
			if (!ptr) {
				continue;
			}
			pos = ptr - *fields;

			/* Allocate the key and value and copy the data onto them */
			key = emalloc(pos + 1);
			val = emalloc((len - pos) + 1);

			strlcpy(key, *fields, pos + 1);
			strlcpy(val, *fields + pos + 1, len - pos);

			/* Check to see whether or not we want to save the data */
			if (!strcmp(key, "msg")) {
				errmsg = estrndup(val, len - pos);
			}
			else if (!strcmp(key, "type")) {
				errtype = estrndup(val, len - pos);
			}
			else if (!strcmp(key, "line")) {
				errline = estrndup(val, len - pos);
			}
			/* Haven't seen this yet, but turning it on during dev, to see
				what we can encounter -- MRS
			else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Got key %s with val %s", key, val);
			}
			*/
			
			/* Cleanup */
			if (key) efree(key);
			if (val) efree(val);

			/* Next key:value pair please :) */
			fields++;
		}
	}
	
	/* If no error line is given, then place none in the 
	   file */
	if (!errline) {
		errline = estrndup("none", sizeof("none") - 1);
	}
	
	/* Default type is a log handle */
	if (!errtype) {
		errtype = estrndup("log", sizeof("log") - 1);
	}
	
	/* No error message, no cry */
	if (!errmsg) {
		errmsg = estrndup("unknown error", sizeof("unknown error") - 1);
	}
	
	/* Allocate the message buf and copy the data into it */
	msgbuf = emalloc((sizeof(msgformat) - 6) +
	                 strlen(errmsg) +
	                 strlen(errline) +
	                 strlen(errtype) + 1);
	sprintf(msgbuf, msgformat, errline, errtype, errmsg);

	/* If the error is serious enough, copy it to our error buffer 
	   which will show up when someone calls the xslt_error() function */
	if (level == MH_LEVEL_WARN  ||
	    level == MH_LEVEL_ERROR ||
	    level == MH_LEVEL_CRITICAL) {
		XSLT_REG_ERRMSG(errmsg, handle);
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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot open log file, %s [%d]: %s",
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
	error = write(XSLT_LOG(handle).fd, msgbuf, strlen(msgbuf));
	if (error == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot write data to log file, %s, with fd, %d [%d]: %s",
		          (XSLT_LOG(handle).path ? XSLT_LOG(handle).path : "stderr"),
		          XSLT_LOG(handle).fd,
		          errno,
		          strerror(errno));
		return 0;
	}

	/* Cleanup */
	if (msgbuf)  efree(msgbuf);
	if (errtype) efree(errtype);
	if (errline) efree(errline);
	if (errmsg)  efree(errmsg);
	
	return 0;
}
/* }}} */

/* {{{ error_print()
   Print out an error message or call the error handler */
static MH_ERROR error_print(void *user_data, SablotHandle proc, MH_ERROR code, MH_LEVEL level, char **fields)
{
	php_xslt *handle = (php_xslt *) user_data;   /* A PHP-XSLT processor */
	TSRMLS_FETCH();
	
	if (XSLT_ERROR(handle)) {
		zval   *argv[4];   /* Arguments to the error function */
		zval   *retval;    /* Return value from the error function */
        
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

		if (fields) {
			while (*fields) {
				char *key;  /* Key to for the message */
				char *val;  /* The message itself */
				char *ptr;  /* Pointer to the location of the ':' (separator) */
				int   pos;  /* Position of the ':' (separator) */
				int   len;  /* Length of the string */
				
				len = strlen(*fields);
			
				/* Grab the separator's position */
				ptr = strchr(*fields, ':');
				if (!ptr) {
					continue;
				}
				pos = ptr - *fields;

				/* Allocate the key and value and copy the data onto them */
				key = emalloc(pos + 1);
				val = emalloc((len - pos) + 1);

				strlcpy(key, *fields, pos + 1);
				strlcpy(val, *fields + pos + 1, len - pos);

				/* Add it */				
				add_assoc_stringl_ex(argv[3], key, pos + 1, val, len - pos - 1, 1);

				/* Cleanup */
				efree(key);
				efree(val);

				/* Next field please */
				fields++;
			}
		}

		/* Call the function */
		xslt_call_function("error handler", XSLT_ERROR(handle), handle->object,
		                   4, argv, &retval);

		/* Free up */
		if (retval)
			zval_ptr_dtor(&retval);
	}
	else {
		char *errmsg  = NULL;                                  /* Error message */
		char *errline = NULL;                                  /* Error line */
		char *msgbuf  = NULL;                                  /* Message buffer */
		char  msgformat[] = "Sablotron error on line %s: %s";  /* Message format */

		/* If the error is not serious, exit out */
		if (code == MH_LEVEL_WARN  || 
		    code == MH_LEVEL_ERROR || 
		    code == MH_LEVEL_CRITICAL) {
			return 0;
		}

		/* Loop through and extract the error message and the 
		   error line */
		if (fields) {
			while (fields && *fields) {
				char *key;  /* Key to for the message */
				char *val;  /* The message itself */
				char *ptr;  /* Pointer to the location of the ':' (separator) */
				int   pos;  /* Position of the ':' (separator) */
				int   len;  /* Length of the string */
			
				len = strlen(*fields);
			
				/* Grab the separator's position */
				ptr = strchr(*fields, ':');
				if (!ptr) {
					continue;
				}
				pos = ptr - *fields;
			
				/* Allocate the key and value and copy the data onto them */
				key = emalloc(pos + 1);
				val = emalloc((len - pos) + 1);
			
				strlcpy(key, *fields, pos + 1);
				strlcpy(val, *fields + pos + 1, len - pos);
			
				/* Check to see whether or not we want to save the data */
				if (!strcmp(key, "msg")) {
					errmsg = estrndup(val, len - pos);
				}
				else if (!strcmp(key, "line")) {
					errline = estrndup(val, len - pos);
				}

				/* Cleanup */
				if (key) efree(key);
				if (val) efree(val);
			
				/* Next key:value pair please :) */
				fields++;
			}
		}
		
		if (!errline) {
			errline = estrndup("none", sizeof("none") - 1);
		}

		if (!errmsg) {
			errmsg = estrndup("unkown error", sizeof("unkown error") - 1);
		}

		/* Allocate the message buffer and copy the data onto it */
		msgbuf = emalloc((sizeof(msgformat) - 4) + strlen(errmsg) + strlen(errline) + 1);
		sprintf(msgbuf, msgformat, errline, errmsg);

		XSLT_REG_ERRMSG(errmsg, handle);

		/* Output a warning */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, msgbuf);

		/* Cleanup */
		efree(msgbuf);
		efree(errmsg);
		efree(errline);
	}

	return(0);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
