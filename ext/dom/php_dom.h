/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>							  |
   |          Marcus Borger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_DOM_H
#define PHP_DOM_H

#if HAVE_DOM
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#if defined(LIBXML_HTML_ENABLED)
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#endif
#if defined(LIBXML_XPATH_ENABLED)
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif
#if defined(LIBXML_XPTR_ENABLED)
#include <libxml/xpointer.h>
#endif

#include "xml_common.h"

/* DOM API_VERSION, please bump it up, if you change anything in the API
    therefore it's easier for the script-programmers to check, what's working how
   Can be checked with phpversion("dom");
*/
#define DOM_API_VERSION "20030413"
        
extern zend_module_entry dom_module_entry;

#define dom_module_ptr &dom_module_entry

#include "dom_fe.h"

void php_dom_set_object(dom_object *wrapper, void *obj TSRMLS_DC);
dom_object *dom_object_get_data(xmlNodePtr obj);
void php_dom_throw_error(int error_code, zval **retval TSRMLS_DC);
void node_free_resource(xmlNodePtr node TSRMLS_DC);
void node_list_unlink(xmlNodePtr node TSRMLS_DC);
int increment_document_reference(dom_object *object, xmlDocPtr docp TSRMLS_DC);
int decrement_document_reference(dom_object *object TSRMLS_DC);
xmlNsPtr dom_get_ns(char *uri, char *qName, int uri_len, int qName_len, int *errorcode, char **localname);
void dom_set_old_ns(xmlDoc *doc, xmlNs *ns);
xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName);
void dom_normalize (xmlNodePtr nodep TSRMLS_DC);
void dom_get_elements_by_tag_name_ns_raw(xmlNodePtr nodep, char *ns, char *local, zval **retval, dom_object *intern  TSRMLS_DC);
void php_dom_create_implementation(zval **retval  TSRMLS_DC);
int dom_hierarchy(xmlNodePtr parent, xmlNodePtr child);
int dom_has_feature(char *feature, char *version);

#define DOM_NO_ARGS() \
	if (ZEND_NUM_ARGS() != 0) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects exactly 0 parameters, %d given", ZEND_NUM_ARGS()); \
		return; \
	}

PHP_MINIT_FUNCTION(dom);
PHP_MSHUTDOWN_FUNCTION(dom);
PHP_MINFO_FUNCTION(dom);
#else
#define dom_module_ptr NULL

#endif /* HAVE_DOM */
#define phpext_dom_ptr dom_module_ptr

#endif /* _PHP_DIR_H */
