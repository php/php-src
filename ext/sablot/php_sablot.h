/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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

#ifndef PHP_SABLOT_H
#define PHP_SABLOT_H

#if HAVE_SABLOT

#include <sablot.h>

extern zend_module_entry sablot_module_entry;
#define phpext_sablot_ptr &sablot_module_entry

#ifdef PHP_WIN32
#define PHP_SABLOT_API __declspec(dllexport)
#else
#define PHP_SABLOT_API
#endif

/* Module functions */
PHP_MINIT_FUNCTION(sablot);
PHP_MINFO_FUNCTION(sablot);
PHP_MSHUTDOWN_FUNCTION(sablot);

/* Request functions */
PHP_RSHUTDOWN_FUNCTION(sablot);

/* Output transformation functions */
PHP_FUNCTION(xslt_output_begintransform);
PHP_FUNCTION(xslt_output_endtransform);

/* Basic transformation functions */
PHP_FUNCTION(xslt_transform);
PHP_FUNCTION(xslt_process);

/* Advanced API transformation functions */
PHP_FUNCTION(xslt_create);
PHP_FUNCTION(xslt_run);
PHP_FUNCTION(xslt_set_sax_handler);
PHP_FUNCTION(xslt_set_scheme_handler);
PHP_FUNCTION(xslt_set_error_handler);
#ifdef HAVE_SABLOT_SET_ENCODING
PHP_FUNCTION(xslt_set_encoding);
#endif
PHP_FUNCTION(xslt_set_base);
PHP_FUNCTION(xslt_fetch_result);
PHP_FUNCTION(xslt_free);

/* Error Handling functions */
PHP_FUNCTION(xslt_error);
PHP_FUNCTION(xslt_errno);
PHP_FUNCTION(xslt_openlog);
PHP_FUNCTION(xslt_closelog);

/* Sablotron error structure */
struct _php_sablot_error {
	char *key;
	char *value;
	struct _php_sablot_error *next;
};

typedef struct _php_sablot_error php_sablot_error;


/* Sablotron Handle */
typedef struct {

    /* Error Handling */
    zval *errorHandler;
	php_sablot_error *errors;
	php_sablot_error errors_start;
	int last_errno;
	
    /* SAX Handling */
	long index;
	zval *startDocHandler;
	zval *startElementHandler;
	zval *endElementHandler;
	zval *startNamespaceHandler;
	zval *endNamespaceHandler;
	zval *commentHandler;
	zval *PIHandler;
	zval *charactersHandler;
	zval *endDocHandler;
	
    /* Scheme Handling */
	zval *getAllHandler;

	/* Sablotron Related */
	SablotHandle p;

} php_sablot;


/* Sablotron Globals */
ZEND_BEGIN_MODULE_GLOBALS(sablot)
    zval *errorHandler;
    php_sablot_error *errors;
    php_sablot_error errors_start;
	char *output_transform_file; /* For output transformations */
	int last_errno;              /* Global last_errno, if no handle is found */
	SablotHandle processor;
ZEND_END_MODULE_GLOBALS(sablot)


#ifdef ZTS
#define SABLOTG(v) TSRMG(sablot_globals_id, zend_sablot_globals *, v)
#else
#define SABLOTG(v) (sablot_globals.v)
#endif

#else
#define phpext_sablot_ptr NULL
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
