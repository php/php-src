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

PHP_MINIT_FUNCTION(sablot);
PHP_MINFO_FUNCTION(sablot);
PHP_FUNCTION(xslt_output_begintransform);
PHP_FUNCTION(xslt_output_endtransform);
PHP_FUNCTION(xslt_transform);
PHP_FUNCTION(xslt_process);
PHP_FUNCTION(xslt_create);
PHP_FUNCTION(xslt_run);
PHP_FUNCTION(xslt_fetch_result);
PHP_FUNCTION(xslt_openlog);
PHP_FUNCTION(xslt_closelog);
PHP_FUNCTION(xslt_set_sax_handler);
PHP_FUNCTION(xslt_free);
PHP_FUNCTION(xslt_error);
PHP_FUNCTION(xslt_errno);

struct _php_sablot_error {
	char *key;
	char *value;
	struct _php_sablot_error *next;
};

typedef struct _php_sablot_error php_sablot_error;

typedef struct {
	long index;
	int last_errno;
	php_sablot_error *errors, errors_start;
	zval *startDocHandler;
	zval *startElementHandler;
	zval *endElementHandler;
	zval *startNamespaceHandler;
	zval *endNamespaceHandler;
	zval *commentHandler;
	zval *PIHandler;
	zval *charactersHandler;
	zval *endDocHandler;
	SablotHandle p;
} php_sablot;

typedef struct {
	char *output_transform_file;
	int le_sablot;
	int last_errno;
	int processor;
} php_sablot_globals;


#ifdef ZTS
#define SABLOTG(v) (sablot_globals->v)
#define SABLOTLS_FETCH() php_sablot_globals *sablot_globals = ts_resource(sablot_globals_id)
#else
#define SABLOTG(v) (sablot_globals.v)
#define SABLOTLS_FETCH()
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
