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

#ifndef PHP_XSLT_H
#define PHP_XSLT_H

#include "php.h"

#if HAVE_SABLOT_BACKEND

#include "php_xslt.h"

#include <sablot.h>

extern zend_module_entry xslt_module_entry;
#define phpext_xslt_ptr &xslt_module_entry

#ifdef PHP_WIN32
#define PHP_XSLT_API __declspec(dllexport)
#else
#define PHP_XSLT_API
#endif

#define XSLT_SCHEME(handle)    ((handle)->handlers->scheme)
#define XSLT_SAX(handle)       ((handle)->handlers->sax)
#define XSLT_ERROR(handle)     ((handle)->handlers->error)

#define XSLT_PROCESSOR(handle) ((handle)->processor.ptr)

#define XSLT_ERRNO(handle)     ((handle)->err->no)
#define XSLT_ERRSTR(handle)    ((handle)->err->str)
#define XSLT_LOG(handle)       ((handle)->err->log)

#define XSLT_FUNCH_FREE(__var) if (__var) zval_ptr_dtor(&(__var)); 

PHP_MINIT_FUNCTION(xslt);
PHP_MINFO_FUNCTION(xslt);

PHP_FUNCTION(xslt_create);
PHP_FUNCTION(xslt_set_sax_handlers);
PHP_FUNCTION(xslt_set_scheme_handlers);
PHP_FUNCTION(xslt_set_error_handler);
PHP_FUNCTION(xslt_set_base);
PHP_FUNCTION(xslt_set_encoding);
PHP_FUNCTION(xslt_set_log);
PHP_FUNCTION(xslt_process);
PHP_FUNCTION(xslt_error);
PHP_FUNCTION(xslt_errno);
PHP_FUNCTION(xslt_free);
PHP_FUNCTION(xslt_backend_version);
PHP_FUNCTION(xslt_backend_name);

struct scheme_handlers {
	zval *get_all;
	zval *open;
	zval *get;
	zval *put;
	zval *close;
};

struct sax_handlers {
	zval *doc_start;
	zval *element_start;
	zval *element_end;
	zval *namespace_start;
	zval *namespace_end;
	zval *comment;
	zval *pi;
	zval *characters;
	zval *doc_end;
};

struct xslt_handlers {
	struct scheme_handlers   scheme;
	struct sax_handlers      sax;
	zval                    *error;
};

struct xslt_processor {
	SablotHandle ptr;
	long         idx;
};

struct xslt_log {
	char *path;
	int   fd;
	int   do_log;
};

struct xslt_error {
	struct xslt_log  log;
	char            *str;
	int              no;
};

typedef struct {
	struct xslt_handlers  *handlers;
	struct xslt_processor  processor;
	struct xslt_error     *err;
} php_xslt;

#else
#define phpext_xslt_ptr NULL
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
