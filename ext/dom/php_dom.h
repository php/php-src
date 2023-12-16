/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>							  |
   |          Marcus Borger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

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
#ifdef LIBXML_HTML_ENABLED
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#endif
#ifdef LIBXML_XPATH_ENABLED
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif
#ifdef LIBXML_XPTR_ENABLED
#include <libxml/xpointer.h>
#endif
#ifdef PHP_WIN32
#ifndef DOM_EXPORTS
#define DOM_EXPORTS
#endif
#endif

#include "xml_common.h"
#include "ext/libxml/php_libxml.h"
#include "xpath_callbacks.h"
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
	php_dom_xpath_callbacks xpath_callbacks;
	int register_node_ns;
	dom_object dom;
} dom_xpath_object;

static inline dom_xpath_object *php_xpath_obj_from_obj(zend_object *obj) {
	return (dom_xpath_object*)((char*)(obj)
		- XtOffsetOf(dom_xpath_object, dom) - XtOffsetOf(dom_object, std));
}

#define Z_XPATHOBJ_P(zv)  php_xpath_obj_from_obj(Z_OBJ_P((zv)))

typedef struct _dom_nnodemap_object {
	dom_object *baseobj;
	zval baseobj_zv;
	int nodetype;
	int cached_length;
	xmlHashTable *ht;
	xmlChar *local;
	xmlChar *ns;
	php_libxml_cache_tag cache_tag;
	dom_object *cached_obj;
	int cached_obj_index;
	bool free_local : 1;
	bool free_ns : 1;
} dom_nnodemap_object;

typedef struct {
	zend_object_iterator intern;
	zval curobj;
	HashPosition pos;
	php_libxml_cache_tag cache_tag;
} php_dom_iterator;

typedef struct {
	/* This may be a fake object that isn't actually in the children list of the parent.
	 * This is because some namespace declaration nodes aren't stored on the parent in libxml2, so we have to fake it.
	 * We could use a zval for this, but since this is always going to be an object let's save space... */
	dom_object *parent_intern;
	dom_object dom;
} dom_object_namespace_node;

static inline dom_object_namespace_node *php_dom_namespace_node_obj_from_obj(zend_object *obj) {
	return (dom_object_namespace_node*)((char*)(obj) - XtOffsetOf(dom_object_namespace_node, dom.std));
}

#include "domexception.h"

#define DOM_HTML_NO_DEFAULT_NS (1U << 31)

dom_object *dom_object_get_data(xmlNodePtr obj);
dom_doc_propsptr dom_get_doc_props(php_libxml_ref_obj *document);
libxml_doc_props const* dom_get_doc_props_read_only(const php_libxml_ref_obj *document);
zend_object *dom_objects_new(zend_class_entry *class_type);
zend_object *dom_nnodemap_objects_new(zend_class_entry *class_type);
#ifdef LIBXML_XPATH_ENABLED
zend_object *dom_xpath_objects_new(zend_class_entry *class_type);
#endif
int dom_get_strict_error(php_libxml_ref_obj *document);
void php_dom_throw_error(int error_code, int strict_error);
void php_dom_throw_error_with_message(int error_code, char *error_message, int strict_error);
void node_list_unlink(xmlNodePtr node);
int dom_check_qname(char *qname, char **localname, char **prefix, int uri_len, int name_len);
xmlNsPtr dom_get_ns(xmlNodePtr node, char *uri, int *errorcode, char *prefix);
xmlNsPtr dom_get_ns_unchecked(xmlNodePtr nodep, char *uri, char *prefix);
void dom_reconcile_ns(xmlDocPtr doc, xmlNodePtr nodep);
void dom_reconcile_ns_list(xmlDocPtr doc, xmlNodePtr nodep, xmlNodePtr last);
xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName);
void dom_normalize (xmlNodePtr nodep);
xmlNode *dom_get_elements_by_tag_name_ns_raw(xmlNodePtr basep, xmlNodePtr nodep, char *ns, char *local, int *cur, int index);
void php_dom_create_implementation(zval *retval);
int dom_hierarchy(xmlNodePtr parent, xmlNodePtr child);
bool dom_has_feature(zend_string *feature, zend_string *version);
int dom_node_is_read_only(xmlNodePtr node);
int dom_node_children_valid(xmlNodePtr node);
void php_dom_create_iterator(zval *return_value, int ce_type);
void dom_namednode_iter(dom_object *basenode, int ntype, dom_object *intern, xmlHashTablePtr ht, const char *local, size_t local_len, const char *ns, size_t ns_len);
xmlNodePtr create_notation(const xmlChar *name, const xmlChar *ExternalID, const xmlChar *SystemID);
xmlNode *php_dom_libxml_hash_iter(xmlHashTable *ht, int index);
xmlNode *php_dom_libxml_notation_iter(xmlHashTable *ht, int index);
zend_object_iterator *php_dom_get_iterator(zend_class_entry *ce, zval *object, int by_ref);
void dom_set_doc_classmap(php_libxml_ref_obj *document, zend_class_entry *basece, zend_class_entry *ce);
xmlNodePtr php_dom_create_fake_namespace_decl(xmlNodePtr nodep, xmlNsPtr original, zval *return_value, dom_object *parent_intern);
void php_dom_get_content_into_zval(const xmlNode *nodep, zval *target, bool default_is_null);
zend_string *dom_node_concatenated_name_helper(size_t name_len, const char *name, size_t prefix_len, const char *prefix);
zend_string *dom_node_get_node_name_attribute_or_element(const xmlNode *nodep, bool uppercase);
bool php_dom_is_node_connected(const xmlNode *node);
bool php_dom_adopt_node(xmlNodePtr nodep, dom_object *dom_object_new_document, xmlDocPtr new_document);
xmlNsPtr dom_get_ns_resolve_prefix_conflict(xmlNodePtr tree, const char *uri);
void php_dom_reconcile_attribute_namespace_after_insertion(xmlAttrPtr attrp);
int dom_validate_and_extract(zend_string *namespace, zend_string *qname, xmlChar **localName, xmlChar **prefix);

void php_dom_document_constructor(INTERNAL_FUNCTION_PARAMETERS);

dom_object *php_dom_instantiate_object_helper(zval *return_value, zend_class_entry *ce, xmlNodePtr obj, dom_object *parent);

typedef enum {
	DOM_LOAD_STRING = 0,
	DOM_LOAD_FILE = 1,
} dom_load_mode;

xmlDocPtr dom_document_parser(zval *id, dom_load_mode mode, const char *source, size_t source_len, size_t options, xmlCharEncodingHandlerPtr encoding);

/* parentnode */
void dom_parent_node_prepend(dom_object *context, zval *nodes, uint32_t nodesc);
void dom_parent_node_append(dom_object *context, zval *nodes, uint32_t nodesc);
void dom_parent_node_after(dom_object *context, zval *nodes, uint32_t nodesc);
void dom_parent_node_before(dom_object *context, zval *nodes, uint32_t nodesc);
void dom_parent_node_replace_children(dom_object *context, zval *nodes, uint32_t nodesc);
void dom_child_node_remove(dom_object *context);
void dom_child_replace_with(dom_object *context, zval *nodes, uint32_t nodesc);

void dom_remove_all_children(xmlNodePtr nodep);

/* nodemap and nodelist APIs */
xmlNodePtr php_dom_named_node_map_get_named_item(dom_nnodemap_object *objmap, const char *named, bool may_transform);
void php_dom_named_node_map_get_named_item_into_zval(dom_nnodemap_object *objmap, const char *named, zval *return_value);
xmlNodePtr php_dom_named_node_map_get_item(dom_nnodemap_object *objmap, zend_long index);
void php_dom_named_node_map_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value);
void php_dom_nodelist_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value);
int php_dom_get_namednodemap_length(dom_object *obj);
int php_dom_get_nodelist_length(dom_object *obj);

xmlNodePtr dom_clone_node(xmlNodePtr node, xmlDocPtr doc, const dom_object *intern, bool recursive);

#define DOM_GET_INTERN(__id, __intern) { \
	__intern = Z_DOMOBJ_P(__id); \
	if (UNEXPECTED(__intern->ptr == NULL)) { \
		zend_throw_error(NULL, "Couldn't fetch %s", ZSTR_VAL(__intern->std.ce->name));\
		RETURN_THROWS();\
  	} \
}

#define DOM_GET_THIS_INTERN(__intern) DOM_GET_INTERN(ZEND_THIS, __intern)

#define DOM_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	DOM_GET_INTERN(__id, __intern); \
	__ptr = (__prtype)((php_libxml_node_ptr *)__intern->ptr)->node; \
}

#define DOM_NODELIST 0
#define DOM_NAMEDNODEMAP 1

static zend_always_inline bool php_dom_is_cache_tag_stale_from_doc_ptr(const php_libxml_cache_tag *cache_tag, const php_libxml_ref_obj *doc_ptr)
{
	ZEND_ASSERT(cache_tag != NULL);
	ZEND_ASSERT(doc_ptr != NULL);
	/* See overflow comment in php_libxml_invalidate_node_list_cache(). */
#if SIZEOF_SIZE_T == 8
	return cache_tag->modification_nr != doc_ptr->cache_tag.modification_nr;
#else
	return cache_tag->modification_nr != doc_ptr->cache_tag.modification_nr || UNEXPECTED(doc_ptr->cache_tag.modification_nr == SIZE_MAX);
#endif
}

static zend_always_inline bool php_dom_is_cache_tag_stale_from_node(const php_libxml_cache_tag *cache_tag, const xmlNodePtr node)
{
	ZEND_ASSERT(node != NULL);
	php_libxml_node_ptr *_private = node->_private;
	if (!_private) {
		return true;
	}
	php_libxml_node_object *object_private = _private->_private;
	if (!object_private || !object_private->document) {
		return true;
	}
	return php_dom_is_cache_tag_stale_from_doc_ptr(cache_tag, object_private->document);
}

static zend_always_inline void php_dom_mark_cache_tag_up_to_date_from_node(php_libxml_cache_tag *cache_tag, const xmlNodePtr node)
{
	ZEND_ASSERT(cache_tag != NULL);
	php_libxml_node_ptr *_private = node->_private;
	if (_private) {
		php_libxml_node_object *object_private = _private->_private;
		if (object_private->document) {
			cache_tag->modification_nr = object_private->document->cache_tag.modification_nr;
		}
	}
}

static zend_always_inline bool php_dom_follow_spec_doc_ref(const php_libxml_ref_obj *document)
{
	// TODO: remove this function?
	ZEND_ASSERT(document != NULL);
	return document->is_modern_api_class;
}

static zend_always_inline bool php_dom_follow_spec_intern(const dom_object *intern)
{
	ZEND_ASSERT(intern != NULL);
	return intern->document && php_dom_follow_spec_doc_ref(intern->document);
}

static zend_always_inline bool php_dom_follow_spec_node(const xmlNode *node)
{
	// TODO: remove this function?
	ZEND_ASSERT(node != NULL);
	php_libxml_node_ptr *_private = node->_private;
	if (_private) {
		php_libxml_node_object *object_private = _private->_private;
		if (object_private->document) {
			return php_dom_follow_spec_doc_ref(object_private->document);
		}
	}
	return false;
}

PHP_MINIT_FUNCTION(dom);
PHP_MSHUTDOWN_FUNCTION(dom);
PHP_MINFO_FUNCTION(dom);

#endif /* PHP_DOM_H */
