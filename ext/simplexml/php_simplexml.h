/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SIMPLEXML_H
#define PHP_SIMPLEXML_H

extern zend_module_entry simplexml_module_entry;
#define phpext_simplexml_ptr &simplexml_module_entry

#include "php_version.h"
#define PHP_SIMPLEXML_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

#include "ext/libxml/php_libxml.h"
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xpointer.h>
#include <libxml/xmlschemas.h>

PHP_MINIT_FUNCTION(simplexml);
PHP_MSHUTDOWN_FUNCTION(simplexml);
PHP_MINFO_FUNCTION(simplexml);

typedef enum {
	SXE_ITER_NONE     = 0,
	SXE_ITER_ELEMENT  = 1,
	SXE_ITER_CHILD    = 2,
	SXE_ITER_ATTRLIST = 3
} SXE_ITER;

typedef struct {
	php_libxml_node_ptr *node;
	php_libxml_ref_obj *document;
	HashTable *properties;
	xmlXPathContextPtr xpath;
	struct {
		zend_string           *name;
		zend_string           *nsprefix;
		int                   isprefix;
		SXE_ITER              type;
		zval                  data;
	} iter;
	zval tmp;
	zend_function *fptr_count;
	zend_object zo;
} php_sxe_object;

#ifdef PHP_WIN32
#	ifdef PHP_SIMPLEXML_EXPORTS
#		define PHP_SXE_API __declspec(dllexport)
#	else
#		define PHP_SXE_API __declspec(dllimport)
#	endif
#else
#	define PHP_SXE_API ZEND_API
#endif

extern PHP_SXE_API zend_class_entry *ce_SimpleXMLIterator;
extern PHP_SXE_API zend_class_entry *ce_SimpleXMLElement;

PHP_SXE_API zend_class_entry *sxe_get_element_class_entry(void);

#endif
