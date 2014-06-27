/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

extern zend_module_entry dom_module_entry;
#define phpext_dom_ptr &dom_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#include <libxml/hash.h>
#include <libxml/c14n.h>
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
#ifdef PHP_WIN32
#ifndef DOM_EXPORTS
#define DOM_EXPORTS
#endif
#endif

#include "xml_common.h"
#include "ext/libxml/php_libxml.h"
#include "zend_exceptions.h"
#include "dom_ce.h"
/* DOM API_VERSION, please bump it up, if you change anything in the API
    therefore it's easier for the script-programmers to check, what's working how
   Can be checked with phpversion("dom");
*/
#define DOM_API_VERSION "20031129"
/* Define a custom type for iterating using an unused nodetype */
#define DOM_NODESET XML_XINCLUDE_START

typedef struct _dom_xpath_object {
	zend_object  std;
	void *ptr;
	php_libxml_ref_obj *document;
	HashTable *prop_handler;
	zend_object_handle handle;
	int registerPhpFunctions;
	HashTable *registered_phpfunctions;
	HashTable *node_list;
} dom_xpath_object;

typedef struct _dom_nnodemap_object {
	dom_object *baseobj;
	int nodetype;
	xmlHashTable *ht;
	xmlChar *local;
	xmlChar *ns;
	zval *baseobjptr;
} dom_nnodemap_object;

typedef struct {
	zend_object_iterator     intern;
	zval *curobj;
} php_dom_iterator;

#include "dom_fe.h"

dom_object *dom_object_get_data(xmlNodePtr obj);
dom_doc_propsptr dom_get_doc_props(php_libxml_ref_obj *document);
zend_object_value dom_objects_new(zend_class_entry *class_type TSRMLS_DC);
zend_object_value dom_nnodemap_objects_new(zend_class_entry *class_type TSRMLS_DC);
#if defined(LIBXML_XPATH_ENABLED)
zend_object_value dom_xpath_objects_new(zend_class_entry *class_type TSRMLS_DC);
#endif
int dom_get_strict_error(php_libxml_ref_obj *document);
void php_dom_throw_error(int error_code, int strict_error TSRMLS_DC);
void php_dom_throw_error_with_message(int error_code, char *error_message, int strict_error TSRMLS_DC);
void node_list_unlink(xmlNodePtr node TSRMLS_DC);
int dom_check_qname(char *qname, char **localname, char **prefix, int uri_len, int name_len);
xmlNsPtr dom_get_ns(xmlNodePtr node, char *uri, int *errorcode, char *prefix);
void dom_set_old_ns(xmlDoc *doc, xmlNs *ns);
xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName);
void dom_normalize (xmlNodePtr nodep TSRMLS_DC);
xmlNode *dom_get_elements_by_tag_name_ns_raw(xmlNodePtr nodep, char *ns, char *local, int *cur, int index);
void php_dom_create_implementation(zval **retval  TSRMLS_DC);
int dom_hierarchy(xmlNodePtr parent, xmlNodePtr child);
int dom_has_feature(char *feature, char *version);
int dom_node_is_read_only(xmlNodePtr node);
int dom_node_children_valid(xmlNodePtr node);
void php_dom_create_interator(zval *return_value, int ce_type TSRMLS_DC);
void dom_namednode_iter(dom_object *basenode, int ntype, dom_object *intern, xmlHashTablePtr ht, xmlChar *local, xmlChar *ns TSRMLS_DC);
xmlNodePtr create_notation(const xmlChar *name, const xmlChar *ExternalID, const xmlChar *SystemID);
xmlNode *php_dom_libxml_hash_iter(xmlHashTable *ht, int index);
xmlNode *php_dom_libxml_notation_iter(xmlHashTable *ht, int index);
zend_object_iterator *php_dom_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);
int dom_set_doc_classmap(php_libxml_ref_obj *document, zend_class_entry *basece, zend_class_entry *ce TSRMLS_DC);

#define REGISTER_DOM_CLASS(ce, name, parent_ce, funcs, entry) \
INIT_CLASS_ENTRY(ce, name, funcs); \
ce.create_object = dom_objects_new; \
entry = zend_register_internal_class_ex(&ce, parent_ce, NULL TSRMLS_CC);

#define DOM_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = (dom_object *)zend_object_store_get_object(__id TSRMLS_CC); \
	if (__intern->ptr == NULL || !(__ptr = (__prtype)((php_libxml_node_ptr *)__intern->ptr)->node)) { \
  		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", __intern->std.ce->name);\
  		RETURN_NULL();\
  	} \
}

#define DOM_NO_ARGS() \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	}

#define DOM_NOT_IMPLEMENTED() \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not yet implemented"); \
	return;

#define DOM_NODELIST 0
#define DOM_NAMEDNODEMAP 1

PHP_MINIT_FUNCTION(dom);
PHP_MSHUTDOWN_FUNCTION(dom);
PHP_MINFO_FUNCTION(dom);

#endif /* PHP_DOM_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
