/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_XSL_H
#define PHP_XSL_H

extern zend_module_entry xsl_module_entry;
#define phpext_xsl_ptr &xsl_module_entry

#include "php_version.h"
#define PHP_XSL_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

#include <libxslt/xsltconfig.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/security.h>
#ifdef HAVE_XSL_EXSLT
#include <libexslt/exslt.h>
#include <libexslt/exsltconfig.h>
#endif

#include "../dom/xml_common.h"

#include <libxslt/extensions.h>
#include <libxml/xpathInternals.h>

#define XSL_SECPREF_NONE 0
#define XSL_SECPREF_READ_FILE 2
#define XSL_SECPREF_WRITE_FILE 4
#define XSL_SECPREF_CREATE_DIRECTORY 8
#define XSL_SECPREF_READ_NETWORK 16
#define XSL_SECPREF_WRITE_NETWORK 32
/* Default == disable all write access ==  XSL_SECPREF_WRITE_NETWORK | XSL_SECPREF_CREATE_DIRECTORY | XSL_SECPREF_WRITE_FILE */
#define XSL_SECPREF_DEFAULT 44

typedef struct _xsl_object {
	void *ptr;
	HashTable *prop_handler;
	zval handle;
	HashTable *parameter;
	int hasKeys;
	int registerPhpFunctions;
	HashTable *registered_phpfunctions;
	HashTable *node_list;
	php_libxml_node_object *doc;
	char *profiling;
	zend_long securityPrefs;
	int securityPrefsSet;
	zend_object  std;
} xsl_object;

static inline xsl_object *php_xsl_fetch_object(zend_object *obj) {
	return (xsl_object *)((char*)(obj) - XtOffsetOf(xsl_object, std));
}

#define Z_XSL_P(zv) php_xsl_fetch_object(Z_OBJ_P((zv)))

void php_xsl_set_object(zval *wrapper, void *obj);
void xsl_objects_free_storage(zend_object *object);
void php_xsl_create_object(xsltStylesheetPtr obj, zval *wrapper_in, zval *return_value );

void xsl_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs);
void xsl_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs);

PHP_MINIT_FUNCTION(xsl);
PHP_MSHUTDOWN_FUNCTION(xsl);
PHP_RINIT_FUNCTION(xsl);
PHP_RSHUTDOWN_FUNCTION(xsl);
PHP_MINFO_FUNCTION(xsl);

#endif	/* PHP_XSL_H */
