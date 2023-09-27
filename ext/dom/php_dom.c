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
   |          Rob Richards <rrichards@php.net>                            |
   |          Marcus Borger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "ext/random/php_random.h"
#include "php_dom.h"
#include "php_dom_arginfo.h"
#include "dom_properties.h"
#include "zend_interfaces.h"

#include "ext/standard/info.h"
#define PHP_XPATH 1
#define PHP_XPTR 2

/* {{{ class entries */
PHP_DOM_EXPORT zend_class_entry *dom_node_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_domexception_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_parentnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_childnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_domimplementation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_documentfragment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_document_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_nodelist_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_namednodemap_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_characterdata_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_attr_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_element_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_text_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_comment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_cdatasection_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_documenttype_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_notation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_entity_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_entityreference_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_processinginstruction_class_entry;
#ifdef LIBXML_XPATH_ENABLED
PHP_DOM_EXPORT zend_class_entry *dom_xpath_class_entry;
#endif
PHP_DOM_EXPORT zend_class_entry *dom_namespace_node_class_entry;
/* }}} */

zend_object_handlers dom_object_handlers;
zend_object_handlers dom_nnodemap_object_handlers;
zend_object_handlers dom_nodelist_object_handlers;
zend_object_handlers dom_object_namespace_node_handlers;
#ifdef LIBXML_XPATH_ENABLED
zend_object_handlers dom_xpath_object_handlers;
#endif

static HashTable classes;
/* {{{ prop handler tables */
static HashTable dom_document_prop_handlers;
static HashTable dom_documentfragment_prop_handlers;
static HashTable dom_node_prop_handlers;
static HashTable dom_nodelist_prop_handlers;
static HashTable dom_namednodemap_prop_handlers;
static HashTable dom_characterdata_prop_handlers;
static HashTable dom_attr_prop_handlers;
static HashTable dom_element_prop_handlers;
static HashTable dom_text_prop_handlers;
static HashTable dom_documenttype_prop_handlers;
static HashTable dom_notation_prop_handlers;
static HashTable dom_entity_prop_handlers;
static HashTable dom_processinginstruction_prop_handlers;
static HashTable dom_namespace_node_prop_handlers;
#ifdef LIBXML_XPATH_ENABLED
static HashTable dom_xpath_prop_handlers;
#endif
/* }}} */

static zend_object *dom_objects_namespace_node_new(zend_class_entry *class_type);
static void dom_object_namespace_node_free_storage(zend_object *object);
static xmlNodePtr php_dom_create_fake_namespace_decl_node_ptr(xmlNodePtr nodep, xmlNsPtr original);
static zend_object *dom_nodelist_objects_new(zend_class_entry *class_type);

typedef int (*dom_read_t)(dom_object *obj, zval *retval);
typedef int (*dom_write_t)(dom_object *obj, zval *newval);

typedef struct _dom_prop_handler {
	dom_read_t read_func;
	dom_write_t write_func;
} dom_prop_handler;

static zend_object_handlers* dom_get_obj_handlers(void) {
	return &dom_object_handlers;
}

/* {{{ int dom_node_is_read_only(xmlNodePtr node) */
int dom_node_is_read_only(xmlNodePtr node) {
	switch (node->type) {
		case XML_ENTITY_REF_NODE:
		case XML_ENTITY_NODE:
		case XML_DOCUMENT_TYPE_NODE:
		case XML_NOTATION_NODE:
		case XML_DTD_NODE:
		case XML_ELEMENT_DECL:
		case XML_ATTRIBUTE_DECL:
		case XML_ENTITY_DECL:
		case XML_NAMESPACE_DECL:
			return SUCCESS;
			break;
		default:
			if (node->doc == NULL) {
				return SUCCESS;
			} else {
				return FAILURE;
			}
	}
}
/* }}} end dom_node_is_read_only */

/* {{{ int dom_node_children_valid(xmlNodePtr node) */
int dom_node_children_valid(xmlNodePtr node) {
	switch (node->type) {
		case XML_DOCUMENT_TYPE_NODE:
		case XML_DTD_NODE:
		case XML_PI_NODE:
		case XML_COMMENT_NODE:
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_NOTATION_NODE:
			return FAILURE;
			break;
		default:
			return SUCCESS;
	}
}
/* }}} end dom_node_children_valid */

/* {{{ dom_get_doc_props() */
dom_doc_propsptr dom_get_doc_props(php_libxml_ref_obj *document)
{
	dom_doc_propsptr doc_props;

	if (document && document->doc_props) {
		return document->doc_props;
	} else {
		doc_props = emalloc(sizeof(libxml_doc_props));
		doc_props->formatoutput = 0;
		doc_props->validateonparse = 0;
		doc_props->resolveexternals = 0;
		doc_props->preservewhitespace = 1;
		doc_props->substituteentities = 0;
		doc_props->stricterror = 1;
		doc_props->recover = 0;
		doc_props->classmap = NULL;
		if (document) {
			document->doc_props = doc_props;
		}
		return doc_props;
	}
}

static void dom_copy_doc_props(php_libxml_ref_obj *source_doc, php_libxml_ref_obj *dest_doc)
{
	dom_doc_propsptr source, dest;

	if (source_doc && dest_doc) {

		source = dom_get_doc_props(source_doc);
		dest = dom_get_doc_props(dest_doc);

		dest->formatoutput = source->formatoutput;
		dest->validateonparse = source->validateonparse;
		dest->resolveexternals = source->resolveexternals;
		dest->preservewhitespace = source->preservewhitespace;
		dest->substituteentities = source->substituteentities;
		dest->stricterror = source->stricterror;
		dest->recover = source->recover;
		if (source->classmap) {
			ALLOC_HASHTABLE(dest->classmap);
			zend_hash_init(dest->classmap, 0, NULL, NULL, 0);
			zend_hash_copy(dest->classmap, source->classmap, NULL);
		}

	}
}

void dom_set_doc_classmap(php_libxml_ref_obj *document, zend_class_entry *basece, zend_class_entry *ce)
{
	dom_doc_propsptr doc_props;

	if (document) {
		doc_props = dom_get_doc_props(document);
		if (doc_props->classmap == NULL) {
			if (ce == NULL) {
				return;
			}
			ALLOC_HASHTABLE(doc_props->classmap);
			zend_hash_init(doc_props->classmap, 0, NULL, NULL, 0);
		}
		if (ce) {
			zend_hash_update_ptr(doc_props->classmap, basece->name, ce);
		} else {
			zend_hash_del(doc_props->classmap, basece->name);
		}
	}
}

zend_class_entry *dom_get_doc_classmap(php_libxml_ref_obj *document, zend_class_entry *basece)
{
	dom_doc_propsptr doc_props;

	if (document) {
		doc_props = dom_get_doc_props(document);
		if (doc_props->classmap) {
			zend_class_entry *ce = zend_hash_find_ptr(doc_props->classmap, basece->name);
			if (ce) {
				return ce;
			}
		}
	}

	return basece;
}
/* }}} */

/* {{{ dom_get_strict_error() */
int dom_get_strict_error(php_libxml_ref_obj *document) {
	int stricterror;
	dom_doc_propsptr doc_props;

	doc_props = dom_get_doc_props(document);
	stricterror = doc_props->stricterror;
	if (document == NULL) {
		efree(doc_props);
	}

	return stricterror;
}
/* }}} */

/* {{{ xmlNodePtr dom_object_get_node(dom_object *obj) */
PHP_DOM_EXPORT xmlNodePtr dom_object_get_node(dom_object *obj)
{
	if (obj && obj->ptr != NULL) {
		return ((php_libxml_node_ptr *)obj->ptr)->node;
	} else {
		return NULL;
	}
}
/* }}} end dom_object_get_node */

/* {{{ dom_object *php_dom_object_get_data(xmlNodePtr obj) */
PHP_DOM_EXPORT dom_object *php_dom_object_get_data(xmlNodePtr obj)
{
	if (obj && obj->_private != NULL) {
		return (dom_object *) ((php_libxml_node_ptr *) obj->_private)->_private;
	} else {
		return NULL;
	}
}
/* }}} end php_dom_object_get_data */

static void dom_register_prop_handler(HashTable *prop_handler, char *name, size_t name_len, dom_read_t read_func, dom_write_t write_func)
{
	dom_prop_handler hnd;
	zend_string *str;

	hnd.read_func = read_func;
	hnd.write_func = write_func;
	str = zend_string_init_interned(name, name_len, 1);
	zend_hash_add_mem(prop_handler, str, &hnd, sizeof(dom_prop_handler));
	zend_string_release_ex(str, 1);
}

static zval *dom_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	dom_object *obj = php_dom_obj_from_obj(object);

	if (!obj->prop_handler || !zend_hash_exists(obj->prop_handler, name)) {
		return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
	}

	return NULL;
}

/* {{{ dom_read_property */
zval *dom_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	dom_object *obj = php_dom_obj_from_obj(object);
	zval *retval;
	dom_prop_handler *hnd = NULL;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	} else if (instanceof_function(obj->std.ce, dom_node_class_entry)) {
		zend_throw_error(NULL, "Couldn't fetch %s. Node no longer exists", ZSTR_VAL(obj->std.ce->name));
		retval = &EG(uninitialized_zval);
		return retval;
	}

	if (hnd) {
		int ret = hnd->read_func(obj, rv);
		if (ret == SUCCESS) {
			retval = rv;
		} else {
			retval = &EG(uninitialized_zval);
		}
	} else {
		retval = zend_std_read_property(object, name, type, cache_slot, rv);
	}

	return retval;
}
/* }}} */

zval *dom_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	dom_object *obj = php_dom_obj_from_obj(object);
	dom_prop_handler *hnd = NULL;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd) {
		if (!hnd->write_func) {
			zend_throw_error(NULL, "Cannot write read-only property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
			return &EG(error_zval);
		}

		zend_property_info *prop = zend_get_property_info(object->ce, name, /* silent */ true);
		if (prop && ZEND_TYPE_IS_SET(prop->type)) {
			zval tmp;
			ZVAL_COPY(&tmp, value);
			if (!zend_verify_property_type(prop, &tmp, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)))) {
				zval_ptr_dtor(&tmp);
				return &EG(error_zval);
			}
			hnd->write_func(obj, &tmp);
			zval_ptr_dtor(&tmp);
		} else {
			hnd->write_func(obj, value);
		}

		return value;
	}

	return zend_std_write_property(object, name, value, cache_slot);
}

/* {{{ dom_property_exists */
static int dom_property_exists(zend_object *object, zend_string *name, int check_empty, void **cache_slot)
{
	dom_object *obj = php_dom_obj_from_obj(object);
	dom_prop_handler *hnd = NULL;
	int retval = 0;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}
	if (hnd) {
		zval tmp;

		if (check_empty == 2) {
			retval = 1;
		} else if (hnd->read_func(obj, &tmp) == SUCCESS) {
			if (check_empty == 1) {
				retval = zend_is_true(&tmp);
			} else if (check_empty == 0) {
				retval = (Z_TYPE(tmp) != IS_NULL);
			}
			zval_ptr_dtor(&tmp);
		}
	} else {
		retval = zend_std_has_property(object, name, check_empty, cache_slot);
	}

	return retval;
}
/* }}} */

static HashTable* dom_get_debug_info_helper(zend_object *object, int *is_temp) /* {{{ */
{
	dom_object			*obj = php_dom_obj_from_obj(object);
	HashTable			*debug_info,
						*prop_handlers = obj->prop_handler,
						*std_props;
	zend_string			*string_key;
	dom_prop_handler	*entry;
	zend_string         *object_str;

	*is_temp = 1;

	std_props = zend_std_get_properties(object);
	debug_info = zend_array_dup(std_props);

	if (!prop_handlers) {
		return debug_info;
	}

	object_str = zend_string_init("(object value omitted)", sizeof("(object value omitted)")-1, 0);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(prop_handlers, string_key, entry) {
		zval value;

		if (entry->read_func(obj, &value) == FAILURE || !string_key) {
			continue;
		}

		if (Z_TYPE(value) == IS_OBJECT) {
			zval_ptr_dtor(&value);
			ZVAL_NEW_STR(&value, object_str);
			zend_string_addref(object_str);
		}

		zend_hash_update(debug_info, string_key, &value);
	} ZEND_HASH_FOREACH_END();

	zend_string_release_ex(object_str, 0);

	return debug_info;
}
/* }}} */

static HashTable* dom_get_debug_info(zend_object *object, int *is_temp) /* {{{ */
{
	return dom_get_debug_info_helper(object, is_temp);
}
/* }}} */

void *php_dom_export_node(zval *object) /* {{{ */
{
	php_libxml_node_object *intern;
	xmlNodePtr nodep = NULL;

	intern = (php_libxml_node_object *) Z_DOMOBJ_P(object);
	if (intern->node) {
		nodep = intern->node->node;
	}

	return nodep;
}
/* }}} */

/* {{{ Get a simplexml_element object from dom to allow for processing */
PHP_FUNCTION(dom_import_simplexml)
{
	zval *node;
	xmlNodePtr nodep = NULL;
	php_libxml_node_object *nodeobj;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &node) == FAILURE) {
		RETURN_THROWS();
	}

	nodeobj = (php_libxml_node_object *) ((char *) Z_OBJ_P(node) - Z_OBJ_HT_P(node)->offset);
	nodep = php_libxml_import_node(node);

	if (nodep && nodeobj && (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE)) {
		DOM_RET_OBJ((xmlNodePtr) nodep, &ret, (dom_object *)nodeobj);
	} else {
		zend_argument_value_error(1, "is not a valid node type");
		RETURN_THROWS();
	}
}
/* }}} */

static dom_object* dom_objects_set_class(zend_class_entry *class_type);

static void dom_update_refcount_after_clone(dom_object *original, xmlNodePtr original_node, dom_object *clone, xmlNodePtr cloned_node)
{
	/* If we cloned a document then we must create new doc proxy */
	if (cloned_node->doc == original_node->doc) {
		clone->document = original->document;
	}
	php_libxml_increment_doc_ref((php_libxml_node_object *)clone, cloned_node->doc);
	php_libxml_increment_node_ptr((php_libxml_node_object *)clone, cloned_node, (void *)clone);
	if (original->document != clone->document) {
		dom_copy_doc_props(original->document, clone->document);
	}
}

static zend_object *dom_objects_store_clone_obj(zend_object *zobject) /* {{{ */
{
	dom_object *intern = php_dom_obj_from_obj(zobject);
	dom_object *clone = dom_objects_set_class(intern->std.ce);

	clone->std.handlers = dom_get_obj_handlers();

	if (instanceof_function(intern->std.ce, dom_node_class_entry)) {
		xmlNodePtr node = (xmlNodePtr)dom_object_get_node(intern);
		if (node != NULL) {
			xmlNodePtr cloned_node = xmlDocCopyNode(node, node->doc, 1);
			if (cloned_node != NULL) {
				dom_update_refcount_after_clone(intern, node, clone, cloned_node);
			}

		}
	}

	zend_objects_clone_members(&clone->std, &intern->std);

	return &clone->std;
}
/* }}} */

static zend_object *dom_object_namespace_node_clone_obj(zend_object *zobject)
{
	dom_object_namespace_node *intern = php_dom_namespace_node_obj_from_obj(zobject);
	zend_object *clone = dom_objects_namespace_node_new(intern->dom.std.ce);
	dom_object_namespace_node *clone_intern = php_dom_namespace_node_obj_from_obj(clone);

	xmlNodePtr original_node = dom_object_get_node(&intern->dom);
	ZEND_ASSERT(original_node->type == XML_NAMESPACE_DECL);
	xmlNodePtr cloned_node = php_dom_create_fake_namespace_decl_node_ptr(original_node->parent, original_node->ns);

	if (intern->parent_intern) {
		clone_intern->parent_intern = intern->parent_intern;
		GC_ADDREF(&clone_intern->parent_intern->std);
	}
	dom_update_refcount_after_clone(&intern->dom, original_node, &clone_intern->dom, cloned_node);

	zend_objects_clone_members(clone, &intern->dom.std);
	return clone;
}

static void dom_copy_prop_handler(zval *zv) /* {{{ */
{
	dom_prop_handler *hnd = Z_PTR_P(zv);
	Z_PTR_P(zv) = malloc(sizeof(dom_prop_handler));
	memcpy(Z_PTR_P(zv), hnd, sizeof(dom_prop_handler));
}
/* }}} */

static void dom_dtor_prop_handler(zval *zv) /* {{{ */
{
	free(Z_PTR_P(zv));
}

static const zend_module_dep dom_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_CONFLICTS("domxml")
	ZEND_MOD_END
};

zend_module_entry dom_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER_EX, NULL,
	dom_deps,
	"dom",
	ext_functions,
	PHP_MINIT(dom),
	PHP_MSHUTDOWN(dom),
	NULL,
	NULL,
	PHP_MINFO(dom),
	DOM_API_VERSION, /* Extension versionnumber */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DOM
ZEND_GET_MODULE(dom)
#endif

void dom_objects_free_storage(zend_object *object);
void dom_nnodemap_objects_free_storage(zend_object *object);
static zval *dom_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
static int dom_nodelist_has_dimension(zend_object *object, zval *member, int check_empty);
static zval *dom_nodemap_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
static int dom_nodemap_has_dimension(zend_object *object, zval *member, int check_empty);
static zend_object *dom_objects_store_clone_obj(zend_object *zobject);
#ifdef LIBXML_XPATH_ENABLED
void dom_xpath_objects_free_storage(zend_object *object);
#endif

/* {{{ PHP_MINIT_FUNCTION(dom) */
PHP_MINIT_FUNCTION(dom)
{
	memcpy(&dom_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	dom_object_handlers.offset = XtOffsetOf(dom_object, std);
	dom_object_handlers.free_obj = dom_objects_free_storage;
	dom_object_handlers.read_property = dom_read_property;
	dom_object_handlers.write_property = dom_write_property;
	dom_object_handlers.get_property_ptr_ptr = dom_get_property_ptr_ptr;
	dom_object_handlers.clone_obj = dom_objects_store_clone_obj;
	dom_object_handlers.has_property = dom_property_exists;
	dom_object_handlers.get_debug_info = dom_get_debug_info;

	memcpy(&dom_nnodemap_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_nnodemap_object_handlers.free_obj = dom_nnodemap_objects_free_storage;
	dom_nnodemap_object_handlers.read_dimension = dom_nodemap_read_dimension;
	dom_nnodemap_object_handlers.has_dimension = dom_nodemap_has_dimension;

	memcpy(&dom_nodelist_object_handlers, &dom_nnodemap_object_handlers, sizeof(zend_object_handlers));
	dom_nodelist_object_handlers.read_dimension = dom_nodelist_read_dimension;
	dom_nodelist_object_handlers.has_dimension = dom_nodelist_has_dimension;

	memcpy(&dom_object_namespace_node_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_object_namespace_node_handlers.offset = XtOffsetOf(dom_object_namespace_node, dom.std);
	dom_object_namespace_node_handlers.free_obj = dom_object_namespace_node_free_storage;
	dom_object_namespace_node_handlers.clone_obj = dom_object_namespace_node_clone_obj;

	zend_hash_init(&classes, 0, NULL, NULL, 1);

	dom_domexception_class_entry = register_class_DOMException(zend_ce_exception);

	dom_parentnode_class_entry = register_class_DOMParentNode();

	dom_childnode_class_entry = register_class_DOMChildNode();

	dom_domimplementation_class_entry = register_class_DOMImplementation();
	dom_domimplementation_class_entry->create_object = dom_objects_new;

	dom_node_class_entry = register_class_DOMNode();
	dom_node_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_node_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeName", sizeof("nodeName")-1, dom_node_node_name_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeValue", sizeof("nodeValue")-1, dom_node_node_value_read, dom_node_node_value_write);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeType", sizeof("nodeType")-1, dom_node_node_type_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "parentNode", sizeof("parentNode")-1, dom_node_parent_node_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "childNodes", sizeof("childNodes")-1, dom_node_child_nodes_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "firstChild", sizeof("firstChild")-1, dom_node_first_child_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "lastChild", sizeof("lastChild")-1, dom_node_last_child_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "previousSibling", sizeof("previousSibling")-1, dom_node_previous_sibling_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "nextSibling", sizeof("nextSibling")-1, dom_node_next_sibling_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "attributes", sizeof("attributes")-1, dom_node_attributes_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "ownerDocument", sizeof("ownerDocument")-1, dom_node_owner_document_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "namespaceURI", sizeof("namespaceURI")-1, dom_node_namespace_uri_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "prefix", sizeof("prefix")-1, dom_node_prefix_read, dom_node_prefix_write);
	dom_register_prop_handler(&dom_node_prop_handlers, "localName", sizeof("localName")-1, dom_node_local_name_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "baseURI", sizeof("baseURI")-1, dom_node_base_uri_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "textContent", sizeof("textContent")-1, dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_ptr(&classes, dom_node_class_entry->name, &dom_node_prop_handlers);

	dom_namespace_node_class_entry = register_class_DOMNameSpaceNode();
	dom_namespace_node_class_entry->create_object = dom_objects_namespace_node_new;

	zend_hash_init(&dom_namespace_node_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeName", sizeof("nodeName")-1, dom_node_node_name_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeValue", sizeof("nodeValue")-1, dom_node_node_value_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeType", sizeof("nodeType")-1, dom_node_node_type_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "prefix", sizeof("prefix")-1, dom_node_prefix_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "localName", sizeof("localName")-1, dom_node_local_name_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "namespaceURI", sizeof("namespaceURI")-1, dom_node_namespace_uri_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "ownerDocument", sizeof("ownerDocument")-1, dom_node_owner_document_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "parentNode", sizeof("parentNode")-1, dom_node_parent_node_read, NULL);
	zend_hash_add_ptr(&classes, dom_namespace_node_class_entry->name, &dom_namespace_node_prop_handlers);

	dom_documentfragment_class_entry = register_class_DOMDocumentFragment(dom_node_class_entry, dom_parentnode_class_entry);
	dom_documentfragment_class_entry->create_object = dom_objects_new;
	zend_hash_init(&dom_documentfragment_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);

	dom_register_prop_handler(&dom_documentfragment_prop_handlers, "firstElementChild", sizeof("firstElementChild")-1, dom_parent_node_first_element_child_read, NULL);
	dom_register_prop_handler(&dom_documentfragment_prop_handlers, "lastElementChild", sizeof("lastElementChild")-1, dom_parent_node_last_element_child_read, NULL);
	dom_register_prop_handler(&dom_documentfragment_prop_handlers, "childElementCount", sizeof("childElementCount")-1, dom_parent_node_child_element_count, NULL);

	zend_hash_merge(&dom_documentfragment_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_documentfragment_class_entry->name, &dom_documentfragment_prop_handlers);

	dom_document_class_entry = register_class_DOMDocument(dom_node_class_entry, dom_parentnode_class_entry);
	dom_document_class_entry->create_object = dom_objects_new;
	zend_hash_init(&dom_document_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_document_prop_handlers, "doctype", sizeof("doctype")-1, dom_document_doctype_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "implementation", sizeof("implementation")-1, dom_document_implementation_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "documentElement", sizeof("documentElement")-1, dom_document_document_element_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "actualEncoding", sizeof("actualEncoding")-1, dom_document_encoding_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "encoding", sizeof("encoding")-1, dom_document_encoding_read, dom_document_encoding_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlEncoding", sizeof("xmlEncoding")-1, dom_document_encoding_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "standalone", sizeof("standalone")-1, dom_document_standalone_read, dom_document_standalone_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlStandalone", sizeof("xmlStandalone")-1, dom_document_standalone_read, dom_document_standalone_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "version", sizeof("version")-1, dom_document_version_read, dom_document_version_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlVersion", sizeof("xmlVersion")-1, dom_document_version_read, dom_document_version_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "strictErrorChecking", sizeof("strictErrorChecking")-1, dom_document_strict_error_checking_read, dom_document_strict_error_checking_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "documentURI", sizeof("documentURI")-1, dom_document_document_uri_read, dom_document_document_uri_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "config", sizeof("config")-1, dom_document_config_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "formatOutput", sizeof("formatOutput")-1, dom_document_format_output_read, dom_document_format_output_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "validateOnParse", sizeof("validateOnParse")-1, dom_document_validate_on_parse_read, dom_document_validate_on_parse_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "resolveExternals", sizeof("resolveExternals")-1, dom_document_resolve_externals_read, dom_document_resolve_externals_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "preserveWhiteSpace", sizeof("preserveWhitespace")-1, dom_document_preserve_whitespace_read, dom_document_preserve_whitespace_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "recover", sizeof("recover")-1, dom_document_recover_read, dom_document_recover_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "substituteEntities", sizeof("substituteEntities")-1, dom_document_substitue_entities_read, dom_document_substitue_entities_write);

	dom_register_prop_handler(&dom_document_prop_handlers, "firstElementChild", sizeof("firstElementChild")-1, dom_parent_node_first_element_child_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "lastElementChild", sizeof("lastElementChild")-1, dom_parent_node_last_element_child_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "childElementCount", sizeof("childElementCount")-1, dom_parent_node_child_element_count, NULL);

	zend_hash_merge(&dom_document_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_document_class_entry->name, &dom_document_prop_handlers);

	dom_nodelist_class_entry = register_class_DOMNodeList(zend_ce_aggregate, zend_ce_countable);
	dom_nodelist_class_entry->create_object = dom_nodelist_objects_new;
	dom_nodelist_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_init(&dom_nodelist_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_nodelist_prop_handlers, "length", sizeof("length")-1, dom_nodelist_length_read, NULL);
	zend_hash_add_ptr(&classes, dom_nodelist_class_entry->name, &dom_nodelist_prop_handlers);

	dom_namednodemap_class_entry = register_class_DOMNamedNodeMap(zend_ce_aggregate, zend_ce_countable);
	dom_namednodemap_class_entry->create_object = dom_nnodemap_objects_new;
	dom_namednodemap_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_init(&dom_namednodemap_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_namednodemap_prop_handlers, "length", sizeof("length")-1, dom_namednodemap_length_read, NULL);
	zend_hash_add_ptr(&classes, dom_namednodemap_class_entry->name, &dom_namednodemap_prop_handlers);

	dom_characterdata_class_entry = register_class_DOMCharacterData(dom_node_class_entry, dom_childnode_class_entry);
	dom_characterdata_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_characterdata_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "data", sizeof("data")-1, dom_characterdata_data_read, dom_characterdata_data_write);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "length", sizeof("length")-1, dom_characterdata_length_read, NULL);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "previousElementSibling", sizeof("previousElementSibling")-1, dom_node_previous_element_sibling_read, NULL);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "nextElementSibling", sizeof("nextElementSibling")-1, dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_characterdata_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_characterdata_class_entry->name, &dom_characterdata_prop_handlers);

	dom_attr_class_entry = register_class_DOMAttr(dom_node_class_entry);
	dom_attr_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_attr_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_attr_prop_handlers, "name", sizeof("name")-1, dom_attr_name_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "specified", sizeof("specified")-1, dom_attr_specified_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "value", sizeof("value")-1, dom_attr_value_read, dom_attr_value_write);
	dom_register_prop_handler(&dom_attr_prop_handlers, "ownerElement", sizeof("ownerElement")-1, dom_attr_owner_element_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "schemaTypeInfo", sizeof("schemaTypeInfo")-1, dom_attr_schema_type_info_read, NULL);
	zend_hash_merge(&dom_attr_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_attr_class_entry->name, &dom_attr_prop_handlers);

	dom_element_class_entry = register_class_DOMElement(dom_node_class_entry, dom_parentnode_class_entry, dom_childnode_class_entry);
	dom_element_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_element_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_element_prop_handlers, "tagName", sizeof("tagName")-1, dom_element_tag_name_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "schemaTypeInfo", sizeof("schemaTypeInfo")-1, dom_element_schema_type_info_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "firstElementChild", sizeof("firstElementChild")-1, dom_parent_node_first_element_child_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "lastElementChild", sizeof("lastElementChild")-1, dom_parent_node_last_element_child_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "childElementCount", sizeof("childElementCount")-1, dom_parent_node_child_element_count, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "previousElementSibling", sizeof("previousElementSibling")-1, dom_node_previous_element_sibling_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "nextElementSibling", sizeof("nextElementSibling")-1, dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_element_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_element_class_entry->name, &dom_element_prop_handlers);

	dom_text_class_entry = register_class_DOMText(dom_characterdata_class_entry);
	dom_text_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_text_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_text_prop_handlers, "wholeText", sizeof("wholeText")-1, dom_text_whole_text_read, NULL);
	zend_hash_merge(&dom_text_prop_handlers, &dom_characterdata_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_text_class_entry->name, &dom_text_prop_handlers);

	dom_comment_class_entry = register_class_DOMComment(dom_characterdata_class_entry);
	dom_comment_class_entry->create_object = dom_objects_new;
	zend_hash_add_ptr(&classes, dom_comment_class_entry->name, &dom_characterdata_prop_handlers);

	dom_cdatasection_class_entry = register_class_DOMCdataSection(dom_text_class_entry);
	dom_cdatasection_class_entry->create_object = dom_objects_new;
	zend_hash_add_ptr(&classes, dom_cdatasection_class_entry->name, &dom_text_prop_handlers);

	dom_documenttype_class_entry = register_class_DOMDocumentType(dom_node_class_entry);
	dom_documenttype_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_documenttype_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "name", sizeof("name")-1, dom_documenttype_name_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "entities", sizeof("entities")-1, dom_documenttype_entities_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "notations", sizeof("notations")-1, dom_documenttype_notations_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "publicId", sizeof("publicId")-1, dom_documenttype_public_id_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "systemId", sizeof("systemId")-1, dom_documenttype_system_id_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "internalSubset", sizeof("internalSubset")-1, dom_documenttype_internal_subset_read, NULL);
	zend_hash_merge(&dom_documenttype_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_documenttype_class_entry->name, &dom_documenttype_prop_handlers);

	dom_notation_class_entry = register_class_DOMNotation(dom_node_class_entry);
	dom_notation_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_notation_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_notation_prop_handlers, "publicId", sizeof("publicId")-1, dom_notation_public_id_read, NULL);
	dom_register_prop_handler(&dom_notation_prop_handlers, "systemId", sizeof("systemId")-1, dom_notation_system_id_read, NULL);
	zend_hash_merge(&dom_notation_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_notation_class_entry->name, &dom_notation_prop_handlers);

	dom_entity_class_entry = register_class_DOMEntity(dom_node_class_entry);
	dom_entity_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_entity_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_entity_prop_handlers, "publicId", sizeof("publicId")-1, dom_entity_public_id_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "systemId", sizeof("systemId")-1, dom_entity_system_id_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "notationName", sizeof("notationName")-1, dom_entity_notation_name_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "actualEncoding", sizeof("actualEncoding")-1, dom_entity_actual_encoding_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "encoding", sizeof("encoding")-1, dom_entity_encoding_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "version", sizeof("version")-1, dom_entity_version_read, NULL);
	zend_hash_merge(&dom_entity_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_entity_class_entry->name, &dom_entity_prop_handlers);

	dom_entityreference_class_entry = register_class_DOMEntityReference(dom_node_class_entry);
	dom_entityreference_class_entry->create_object = dom_objects_new;
	zend_hash_add_ptr(&classes, dom_entityreference_class_entry->name, &dom_node_prop_handlers);

	dom_processinginstruction_class_entry = register_class_DOMProcessingInstruction(dom_node_class_entry);
	dom_processinginstruction_class_entry->create_object = dom_objects_new;

	zend_hash_init(&dom_processinginstruction_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_processinginstruction_prop_handlers, "target", sizeof("target")-1, dom_processinginstruction_target_read, NULL);
	dom_register_prop_handler(&dom_processinginstruction_prop_handlers, "data", sizeof("data")-1, dom_processinginstruction_data_read, dom_processinginstruction_data_write);
	zend_hash_merge(&dom_processinginstruction_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, dom_processinginstruction_class_entry->name, &dom_processinginstruction_prop_handlers);

#ifdef LIBXML_XPATH_ENABLED
	memcpy(&dom_xpath_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_xpath_object_handlers.offset = XtOffsetOf(dom_xpath_object, dom) + XtOffsetOf(dom_object, std);
	dom_xpath_object_handlers.free_obj = dom_xpath_objects_free_storage;

	dom_xpath_class_entry = register_class_DOMXPath();
	dom_xpath_class_entry->create_object = dom_xpath_objects_new;

	zend_hash_init(&dom_xpath_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_xpath_prop_handlers, "document", sizeof("document")-1, dom_xpath_document_read, NULL);
	dom_register_prop_handler(&dom_xpath_prop_handlers, "registerNodeNamespaces", sizeof("registerNodeNamespaces")-1, dom_xpath_register_node_ns_read, dom_xpath_register_node_ns_write);
	zend_hash_add_ptr(&classes, dom_xpath_class_entry->name, &dom_xpath_prop_handlers);
#endif

	register_php_dom_symbols(module_number);

	php_libxml_register_export(dom_node_class_entry, php_dom_export_node);

	return SUCCESS;
}
/* }}} */

/* {{{ */
PHP_MINFO_FUNCTION(dom)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "DOM/XML", "enabled");
	php_info_print_table_row(2, "DOM/XML API Version", DOM_API_VERSION);
	php_info_print_table_row(2, "libxml Version", LIBXML_DOTTED_VERSION);
#ifdef LIBXML_HTML_ENABLED
	php_info_print_table_row(2, "HTML Support", "enabled");
#endif
#ifdef LIBXML_XPATH_ENABLED
	php_info_print_table_row(2, "XPath Support", "enabled");
#endif
#ifdef LIBXML_XPTR_ENABLED
	php_info_print_table_row(2, "XPointer Support", "enabled");
#endif
#ifdef LIBXML_SCHEMAS_ENABLED
	php_info_print_table_row(2, "Schema Support", "enabled");
	php_info_print_table_row(2, "RelaxNG Support", "enabled");
#endif
	php_info_print_table_end();
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(dom) /* {{{ */
{
	zend_hash_destroy(&dom_document_prop_handlers);
	zend_hash_destroy(&dom_documentfragment_prop_handlers);
	zend_hash_destroy(&dom_node_prop_handlers);
	zend_hash_destroy(&dom_namespace_node_prop_handlers);
	zend_hash_destroy(&dom_nodelist_prop_handlers);
	zend_hash_destroy(&dom_namednodemap_prop_handlers);
	zend_hash_destroy(&dom_characterdata_prop_handlers);
	zend_hash_destroy(&dom_attr_prop_handlers);
	zend_hash_destroy(&dom_element_prop_handlers);
	zend_hash_destroy(&dom_text_prop_handlers);
	zend_hash_destroy(&dom_documenttype_prop_handlers);
	zend_hash_destroy(&dom_notation_prop_handlers);
	zend_hash_destroy(&dom_entity_prop_handlers);
	zend_hash_destroy(&dom_processinginstruction_prop_handlers);
#ifdef LIBXML_XPATH_ENABLED
	zend_hash_destroy(&dom_xpath_prop_handlers);
#endif
	zend_hash_destroy(&classes);

/*	If you want do find memleaks in this module, compile libxml2 with --with-mem-debug and
	uncomment the following line, this will tell you the amount of not freed memory
	and the total used memory into apaches error_log  */
/*  xmlMemoryDump();*/

	return SUCCESS;
}
/* }}} */

/* {{{ node_list_unlink */
void node_list_unlink(xmlNodePtr node)
{
	dom_object *wrapper;

	while (node != NULL) {

		wrapper = php_dom_object_get_data(node);

		if (wrapper != NULL ) {
			xmlUnlinkNode(node);
		} else {
			if (node->type == XML_ENTITY_REF_NODE)
				break;
			node_list_unlink(node->children);

			switch (node->type) {
				case XML_ATTRIBUTE_DECL:
				case XML_DTD_NODE:
				case XML_DOCUMENT_TYPE_NODE:
				case XML_ENTITY_DECL:
				case XML_ATTRIBUTE_NODE:
				case XML_TEXT_NODE:
					break;
				default:
					node_list_unlink((xmlNodePtr) node->properties);
			}

		}

		node = node->next;
	}
}
/* }}} end node_list_unlink */

#ifdef LIBXML_XPATH_ENABLED
/* {{{ dom_xpath_objects_free_storage */
void dom_xpath_objects_free_storage(zend_object *object)
{
	dom_xpath_object *intern = php_xpath_obj_from_obj(object);

	zend_object_std_dtor(&intern->dom.std);

	if (intern->dom.ptr != NULL) {
		xmlXPathFreeContext((xmlXPathContextPtr) intern->dom.ptr);
		php_libxml_decrement_doc_ref((php_libxml_node_object *) &intern->dom);
	}

	if (intern->registered_phpfunctions) {
		zend_hash_destroy(intern->registered_phpfunctions);
		FREE_HASHTABLE(intern->registered_phpfunctions);
	}

	if (intern->node_list) {
		zend_hash_destroy(intern->node_list);
		FREE_HASHTABLE(intern->node_list);
	}
}
/* }}} */
#endif

/* {{{ dom_objects_free_storage */
void dom_objects_free_storage(zend_object *object)
{
	dom_object *intern = php_dom_obj_from_obj(object);
#if defined(__GNUC__) && __GNUC__ >= 3
	int retcount __attribute__((unused)); /* keep compiler quiet */
#else
	int retcount;
#endif

	zend_object_std_dtor(&intern->std);

	if (intern->ptr != NULL && ((php_libxml_node_ptr *)intern->ptr)->node != NULL) {
		if (((xmlNodePtr) ((php_libxml_node_ptr *)intern->ptr)->node)->type != XML_DOCUMENT_NODE && ((xmlNodePtr) ((php_libxml_node_ptr *)intern->ptr)->node)->type != XML_HTML_DOCUMENT_NODE) {
			php_libxml_node_decrement_resource((php_libxml_node_object *) intern);
		} else {
			php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
			retcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);
		}
		intern->ptr = NULL;
	}
}
/* }}} */

void dom_namednode_iter(dom_object *basenode, int ntype, dom_object *intern, xmlHashTablePtr ht, xmlChar *local, xmlChar *ns) /* {{{ */
{
	dom_nnodemap_object *mapptr = (dom_nnodemap_object *) intern->ptr;

	ZEND_ASSERT(basenode != NULL);

	ZVAL_OBJ_COPY(&mapptr->baseobj_zv, &basenode->std);

	mapptr->baseobj = basenode;
	mapptr->nodetype = ntype;
	mapptr->ht = ht;
	mapptr->local = local;
	mapptr->ns = ns;
}
/* }}} */

static void dom_objects_set_class_ex(zend_class_entry *class_type, dom_object *intern)
{
	zend_class_entry *base_class = class_type;
	while ((base_class->type != ZEND_INTERNAL_CLASS || base_class->info.internal.module->module_number != dom_module_entry.module_number) && base_class->parent != NULL) {
		base_class = base_class->parent;
	}

	intern->prop_handler = zend_hash_find_ptr(&classes, base_class->name);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
}

static dom_object* dom_objects_set_class(zend_class_entry *class_type)
{
	dom_object *intern = zend_object_alloc(sizeof(dom_object), class_type);
	dom_objects_set_class_ex(class_type, intern);
	return intern;
}

/* {{{ dom_objects_new */
zend_object *dom_objects_new(zend_class_entry *class_type)
{
	dom_object *intern = dom_objects_set_class(class_type);
	intern->std.handlers = dom_get_obj_handlers();
	return &intern->std;
}
/* }}} */

static zend_object *dom_objects_namespace_node_new(zend_class_entry *class_type)
{
	dom_object_namespace_node *intern = zend_object_alloc(sizeof(dom_object_namespace_node), class_type);
	dom_objects_set_class_ex(class_type, &intern->dom);
	intern->dom.std.handlers = &dom_object_namespace_node_handlers;
	return &intern->dom.std;
}

static void dom_object_namespace_node_free_storage(zend_object *object)
{
	dom_object_namespace_node *intern = php_dom_namespace_node_obj_from_obj(object);
	if (intern->parent_intern != NULL) {
		zval tmp;
		ZVAL_OBJ(&tmp, &intern->parent_intern->std);
		zval_ptr_dtor(&tmp);
	}
	dom_objects_free_storage(object);
}

#ifdef LIBXML_XPATH_ENABLED
/* {{{ zend_object dom_xpath_objects_new(zend_class_entry *class_type) */
zend_object *dom_xpath_objects_new(zend_class_entry *class_type)
{
	dom_xpath_object *intern = zend_object_alloc(sizeof(dom_xpath_object), class_type);

	intern->registered_phpfunctions = zend_new_array(0);
	intern->register_node_ns = 1;

	intern->dom.prop_handler = &dom_xpath_prop_handlers;
	intern->dom.std.handlers = &dom_xpath_object_handlers;

	zend_object_std_init(&intern->dom.std, class_type);
	object_properties_init(&intern->dom.std, class_type);

	return &intern->dom.std;
}
/* }}} */
#endif

void dom_nnodemap_objects_free_storage(zend_object *object) /* {{{ */
{
	dom_object *intern = php_dom_obj_from_obj(object);
	dom_nnodemap_object *objmap = (dom_nnodemap_object *)intern->ptr;

	if (objmap) {
		if (objmap->local) {
			xmlFree(objmap->local);
		}
		if (objmap->ns) {
			xmlFree(objmap->ns);
		}
		if (!Z_ISUNDEF(objmap->baseobj_zv)) {
			zval_ptr_dtor(&objmap->baseobj_zv);
		}
		efree(objmap);
		intern->ptr = NULL;
	}

	php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);

	zend_object_std_dtor(&intern->std);
}
/* }}} */

static zend_object *dom_nodemap_or_nodelist_objects_new(zend_class_entry *class_type, const zend_object_handlers *handlers)
{
	dom_object *intern;
	dom_nnodemap_object *objmap;

	intern = dom_objects_set_class(class_type);
	intern->ptr = emalloc(sizeof(dom_nnodemap_object));
	objmap = (dom_nnodemap_object *)intern->ptr;
	ZVAL_UNDEF(&objmap->baseobj_zv);
	objmap->baseobj = NULL;
	objmap->nodetype = 0;
	objmap->ht = NULL;
	objmap->local = NULL;
	objmap->ns = NULL;

	intern->std.handlers = handlers;

	return &intern->std;
}

zend_object *dom_nnodemap_objects_new(zend_class_entry *class_type) /* {{{ */
{
	return dom_nodemap_or_nodelist_objects_new(class_type, &dom_nnodemap_object_handlers);
}
/* }}} */

zend_object *dom_nodelist_objects_new(zend_class_entry *class_type)
{
	return dom_nodemap_or_nodelist_objects_new(class_type, &dom_nodelist_object_handlers);
}

void php_dom_create_iterator(zval *return_value, int ce_type) /* {{{ */
{
	zend_class_entry *ce;

	if (ce_type == DOM_NAMEDNODEMAP) {
		ce = dom_namednodemap_class_entry;
	} else {
		ce = dom_nodelist_class_entry;
	}

	object_init_ex(return_value, ce);
}
/* }}} */

/* {{{ php_dom_create_object */
PHP_DOM_EXPORT bool php_dom_create_object(xmlNodePtr obj, zval *return_value, dom_object *domobj)
{
	zend_class_entry *ce;
	dom_object *intern;

	if (!obj) {
		ZVAL_NULL(return_value);
		return 0;
	}

	if ((intern = (dom_object *) php_dom_object_get_data((void *) obj))) {
		ZVAL_OBJ_COPY(return_value, &intern->std);
		return 1;
	}

	switch (obj->type) {
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		{
			ce = dom_document_class_entry;
			break;
		}
		case XML_DTD_NODE:
		case XML_DOCUMENT_TYPE_NODE:
		{
			ce = dom_documenttype_class_entry;
			break;
		}
		case XML_ELEMENT_NODE:
		{
			ce = dom_element_class_entry;
			break;
		}
		case XML_ATTRIBUTE_NODE:
		{
			ce = dom_attr_class_entry;
			break;
		}
		case XML_TEXT_NODE:
		{
			ce = dom_text_class_entry;
			break;
		}
		case XML_COMMENT_NODE:
		{
			ce = dom_comment_class_entry;
			break;
		}
		case XML_PI_NODE:
		{
			ce = dom_processinginstruction_class_entry;
			break;
		}
		case XML_ENTITY_REF_NODE:
		{
			ce = dom_entityreference_class_entry;
			break;
		}
		case XML_ENTITY_DECL:
		case XML_ELEMENT_DECL:
		{
			ce = dom_entity_class_entry;
			break;
		}
		case XML_CDATA_SECTION_NODE:
		{
			ce = dom_cdatasection_class_entry;
			break;
		}
		case XML_DOCUMENT_FRAG_NODE:
		{
			ce = dom_documentfragment_class_entry;
			break;
		}
		case XML_NOTATION_NODE:
		{
			ce = dom_notation_class_entry;
			break;
		}
		case XML_NAMESPACE_DECL:
		{
			ce = dom_namespace_node_class_entry;
			break;
		}
		default:
			/* TODO Convert to a ZEND assertion? */
			zend_throw_error(NULL, "Unsupported node type: %d", obj->type);
			ZVAL_NULL(return_value);
			return 0;
	}

	if (domobj && domobj->document) {
		ce = dom_get_doc_classmap(domobj->document, ce);
	}
	object_init_ex(return_value, ce);

	intern = Z_DOMOBJ_P(return_value);
	if (obj->doc != NULL) {
		if (domobj != NULL) {
			intern->document = domobj->document;
		}
		php_libxml_increment_doc_ref((php_libxml_node_object *)intern, obj->doc);
	}

	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, obj, (void *)intern);
	return 0;
}
/* }}} end php_domobject_new */

void php_dom_create_implementation(zval *retval) {
	object_init_ex(retval, dom_domimplementation_class_entry);
}

/* {{{ int dom_hierarchy(xmlNodePtr parent, xmlNodePtr child) */
int dom_hierarchy(xmlNodePtr parent, xmlNodePtr child)
{
	xmlNodePtr nodep;

	if (parent == NULL || child == NULL || child->doc != parent->doc) {
		return SUCCESS;
	}

	if (child->type == XML_DOCUMENT_NODE) {
		return FAILURE;
	}

	nodep = parent;

	while (nodep) {
		if (nodep == child) {
			return FAILURE;
		}
		nodep = nodep->parent;
	}

	return SUCCESS;
}
/* }}} end dom_hierarchy */

/* {{{ */
bool dom_has_feature(zend_string *feature, zend_string *version)
{
	if (zend_string_equals_literal(version, "1.0")
		|| zend_string_equals_literal(version, "2.0")
		|| zend_string_equals_literal(version, "")
	) {
		if (zend_string_equals_literal_ci(feature, "XML")
			|| (zend_string_equals_literal_ci(feature, "Core") && zend_string_equals_literal(version, "1.0"))
		) {
			return true;
		}
	}

	return false;
}
/* }}} end dom_has_feature */

xmlNode *dom_get_elements_by_tag_name_ns_raw(xmlNodePtr nodep, char *ns, char *local, int *cur, int index) /* {{{ */
{
	xmlNodePtr ret = NULL;

	/* Note: The spec says that ns == '' must be transformed to ns == NULL. In other words, they are equivalent.
	 *       PHP however does not do this and internally uses the empty string everywhere when the user provides ns == NULL.
	 *       This is because for PHP ns == NULL has another meaning: "match every namespace" instead of "match the empty namespace". */
	bool ns_match_any = ns == NULL || (ns[0] == '*' && ns[1] == '\0');

	while (nodep != NULL && (*cur <= index || index == -1)) {
		if (nodep->type == XML_ELEMENT_NODE) {
			if (xmlStrEqual(nodep->name, (xmlChar *)local) || xmlStrEqual((xmlChar *)"*", (xmlChar *)local)) {
				if (ns_match_any || (!strcmp(ns, "") && nodep->ns == NULL) || (nodep->ns != NULL && xmlStrEqual(nodep->ns->href, (xmlChar *)ns))) {
					if (*cur == index) {
						ret = nodep;
						break;
					}
					(*cur)++;
				}
			}
			ret = dom_get_elements_by_tag_name_ns_raw(nodep->children, ns, local, cur, index);
			if (ret != NULL) {
				break;
			}
		}
		nodep = nodep->next;
	}
	return ret;
}
/* }}} */
/* }}} end dom_element_get_elements_by_tag_name_ns_raw */

static inline bool is_empty_node(xmlNodePtr nodep)
{
	xmlChar	*strContent = xmlNodeGetContent(nodep);
	bool ret = strContent == NULL || *strContent == '\0';
	xmlFree(strContent);
	return ret;
}

/* {{{ void dom_normalize (xmlNodePtr nodep) */
void dom_normalize (xmlNodePtr nodep)
{
	xmlNodePtr child, nextp, newnextp;
	xmlAttrPtr attr;
	xmlChar	*strContent;

	child = nodep->children;
	while(child != NULL) {
		switch (child->type) {
			case XML_TEXT_NODE:
				nextp = child->next;
				while (nextp != NULL) {
					if (nextp->type == XML_TEXT_NODE) {
						newnextp = nextp->next;
						strContent = xmlNodeGetContent(nextp);
						xmlNodeAddContent(child, strContent);
						xmlFree(strContent);
						xmlUnlinkNode(nextp);
						php_libxml_node_free_resource(nextp);
						nextp = newnextp;
					} else {
						break;
					}
				}
				if (is_empty_node(child)) {
					nextp = child->next;
					xmlUnlinkNode(child);
					php_libxml_node_free_resource(child);
					child = nextp;
					continue;
				}
				break;
			case XML_ELEMENT_NODE:
				dom_normalize (child);
				attr = child->properties;
				while (attr != NULL) {
					dom_normalize((xmlNodePtr) attr);
					attr = attr->next;
				}
				break;
			case XML_ATTRIBUTE_NODE:
				dom_normalize (child);
				break;
			default:
				break;
		}
		child = child->next;
	}
}
/* }}} end dom_normalize */


/* {{{ void dom_set_old_ns(xmlDoc *doc, xmlNs *ns) */
void dom_set_old_ns(xmlDoc *doc, xmlNs *ns) {
	xmlNs *cur;

	if (doc == NULL)
		return;

	if (doc->oldNs == NULL) {
		doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
		if (doc->oldNs == NULL) {
			return;
		}
		memset(doc->oldNs, 0, sizeof(xmlNs));
		doc->oldNs->type = XML_LOCAL_NAMESPACE;
		doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE);
		doc->oldNs->prefix = xmlStrdup((const xmlChar *)"xml");
	}

	cur = doc->oldNs;
	while (cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = ns;
}
/* }}} end dom_set_old_ns */

static void dom_reconcile_ns_internal(xmlDocPtr doc, xmlNodePtr nodep, xmlNodePtr search_parent)
{
	xmlNsPtr nsptr, nsdftptr, curns, prevns = NULL;

	/* Following if block primarily used for inserting nodes created via createElementNS */
	if (nodep->nsDef != NULL) {
		curns = nodep->nsDef;
		while (curns) {
			nsdftptr = curns->next;
			if (curns->href != NULL) {
				if((nsptr = xmlSearchNsByHref(doc, search_parent, curns->href)) &&
					(curns->prefix == NULL || xmlStrEqual(nsptr->prefix, curns->prefix))) {
					curns->next = NULL;
					if (prevns == NULL) {
						nodep->nsDef = nsdftptr;
					} else {
						prevns->next = nsdftptr;
					}
					dom_set_old_ns(doc, curns);
					curns = prevns;
				}
			}
			prevns = curns;
			curns = nsdftptr;
		}
	}
}

static void dom_libxml_reconcile_ensure_namespaces_are_declared(xmlNodePtr nodep)
{
	/* Ideally we'd use the DOM-wrapped version, but we cannot: https://github.com/php/php-src/pull/12308. */
#if 0
	/* Put on stack to avoid allocation.
	 * Although libxml2 currently does not use this for the reconciliation, it still
	 * makes sense to do this just in case libxml2's internal change in the future. */
	xmlDOMWrapCtxt dummy_ctxt = {0};
	xmlDOMWrapReconcileNamespaces(&dummy_ctxt, nodep, /* options */ 0);
#else
	xmlReconciliateNs(nodep->doc, nodep);
#endif
}

void dom_reconcile_ns(xmlDocPtr doc, xmlNodePtr nodep) /* {{{ */
{
	/* Although the node type will be checked by the libxml2 API,
	 * we still want to do the internal reconciliation conditionally. */
	if (nodep->type == XML_ELEMENT_NODE) {
		dom_reconcile_ns_internal(doc, nodep, nodep->parent);
		dom_libxml_reconcile_ensure_namespaces_are_declared(nodep);
	}
}
/* }}} */

static void dom_reconcile_ns_list_internal(xmlDocPtr doc, xmlNodePtr nodep, xmlNodePtr last, xmlNodePtr search_parent)
{
	ZEND_ASSERT(nodep != NULL);
	while (true) {
		if (nodep->type == XML_ELEMENT_NODE) {
			dom_reconcile_ns_internal(doc, nodep, search_parent);
			if (nodep->children) {
				dom_reconcile_ns_list_internal(doc, nodep->children, nodep->last /* process the whole children list */, search_parent);
			}
		}
		if (nodep == last) {
			break;
		}
		nodep = nodep->next;
	}
}

void dom_reconcile_ns_list(xmlDocPtr doc, xmlNodePtr nodep, xmlNodePtr last)
{
	dom_reconcile_ns_list_internal(doc, nodep, last, nodep->parent);
	/* The loop is outside of the recursion in the above call because
	 * dom_libxml_reconcile_ensure_namespaces_are_declared() performs its own recursion. */
	while (true) {
		/* The internal libxml2 call will already check the node type, no need for us to do it here. */
		dom_libxml_reconcile_ensure_namespaces_are_declared(nodep);
		if (nodep == last) {
			break;
		}
		nodep = nodep->next;
	}
}

/*
http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-DocCrElNS

NAMESPACE_ERR: Raised if

1. the qualifiedName is a malformed qualified name
2. the qualifiedName has a prefix and the  namespaceURI is null
*/

/* {{{ int dom_check_qname(char *qname, char **localname, char **prefix, int uri_len, int name_len) */
int dom_check_qname(char *qname, char **localname, char **prefix, int uri_len, int name_len) {
	if (name_len == 0) {
		return NAMESPACE_ERR;
	}

	*localname = (char *)xmlSplitQName2((xmlChar *)qname, (xmlChar **) prefix);
	if (*localname == NULL) {
		*localname = (char *)xmlStrdup((xmlChar *)qname);
		if (*prefix == NULL && uri_len == 0) {
			return 0;
		}
	}

	/* 1 */
	if (xmlValidateQName((xmlChar *) qname, 0) != 0) {
		return NAMESPACE_ERR;
	}

	/* 2 */
	if (*prefix != NULL && uri_len == 0) {
		return NAMESPACE_ERR;
	}

	return 0;
}
/* }}} */

/*
http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-DocCrElNS

NAMESPACE_ERR: Raised if

3. the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace" [XML Namespaces]
4. the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from  "http://www.w3.org/2000/xmlns/"
5. the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the	qualifiedName nor its prefix is "xmlns".
*/

/* {{{ xmlNsPtr dom_get_ns(xmlNodePtr nodep, char *uri, int *errorcode, char *prefix) */
xmlNsPtr dom_get_ns(xmlNodePtr nodep, char *uri, int *errorcode, char *prefix) {
	xmlNsPtr nsptr = NULL;

	*errorcode = 0;

	if (! ((prefix && !strcmp (prefix, "xml") && strcmp(uri, (char *)XML_XML_NAMESPACE)) ||
		   (prefix && !strcmp (prefix, "xmlns") && strcmp(uri, (char *)DOM_XMLNS_NAMESPACE)) ||
		   (prefix && !strcmp(uri, (char *)DOM_XMLNS_NAMESPACE) && strcmp (prefix, "xmlns")))) {
		nsptr = xmlNewNs(nodep, (xmlChar *)uri, (xmlChar *)prefix);
	}

	if (nsptr == NULL) {
		*errorcode = NAMESPACE_ERR;
	}

	return nsptr;

}
/* }}} end dom_get_ns */

/* {{{ xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName) */
xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName) {
	xmlNsPtr cur;
	xmlNs *ret = NULL;
	if (node == NULL)
		return NULL;

	if (localName == NULL || xmlStrEqual(localName, (xmlChar *)"")) {
		cur = node->nsDef;
		while (cur != NULL) {
			if (cur->prefix == NULL  && cur->href != NULL) {
				ret = cur;
				break;
			}
			cur = cur->next;
		}
	} else {
		cur = node->nsDef;
		while (cur != NULL) {
			if (cur->prefix != NULL && xmlStrEqual(localName, cur->prefix)) {
				ret = cur;
				break;
			}
			cur = cur->next;
		}
	}
	return ret;
}
/* }}} end dom_get_nsdecl */

static xmlNodePtr php_dom_create_fake_namespace_decl_node_ptr(xmlNodePtr nodep, xmlNsPtr original)
{
	xmlNodePtr attrp;
	xmlNsPtr curns = xmlNewNs(NULL, original->href, NULL);
	if (original->prefix) {
		curns->prefix = xmlStrdup(original->prefix);
		attrp = xmlNewDocNode(nodep->doc, NULL, (xmlChar *) original->prefix, original->href);
	} else {
		attrp = xmlNewDocNode(nodep->doc, NULL, (xmlChar *)"xmlns", original->href);
	}
	attrp->type = XML_NAMESPACE_DECL;
	attrp->parent = nodep;
	attrp->ns = curns;
	return attrp;
}

/* Note: Assumes the additional lifetime was already added in the caller. */
xmlNodePtr php_dom_create_fake_namespace_decl(xmlNodePtr nodep, xmlNsPtr original, zval *return_value, dom_object *parent_intern)
{
	xmlNodePtr attrp = php_dom_create_fake_namespace_decl_node_ptr(nodep, original);
	php_dom_create_object(attrp, return_value, parent_intern);
	/* This object must exist, because we just created an object for it via php_dom_create_object(). */
	php_dom_namespace_node_obj_from_obj(Z_OBJ_P(return_value))->parent_intern = parent_intern;
	return attrp;
}

static bool dom_nodemap_or_nodelist_process_offset_as_named(zval *offset, zend_long *lval)
{
	if (Z_TYPE_P(offset) == IS_STRING) {
		/* See zval_get_long_func() */
		double dval;
		zend_uchar is_numeric_string_type;
		if (0 == (is_numeric_string_type = is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), lval, &dval, true))) {
			return true;
		} else if (is_numeric_string_type == IS_DOUBLE) {
			*lval = zend_dval_to_lval_cap(dval);
		}
	} else {
		*lval = zval_get_long(offset);
	}
	return false;
}

static zval *dom_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv) /* {{{ */
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot access DOMNodeList without offset");
		return NULL;
	}

	zend_long lval;
	if (dom_nodemap_or_nodelist_process_offset_as_named(offset, &lval)) {
		/* does not support named lookup */
		ZVAL_NULL(rv);
		return rv;
	}

	php_dom_nodelist_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, lval, rv);
	return rv;
} /* }}} end dom_nodelist_read_dimension */

static int dom_nodelist_has_dimension(zend_object *object, zval *member, int check_empty)
{
	zend_long offset;
	if (dom_nodemap_or_nodelist_process_offset_as_named(member, &offset)) {
		/* does not support named lookup */
		return 0;
	}

	return offset >= 0 && offset < php_dom_get_nodelist_length(php_dom_obj_from_obj(object));
} /* }}} end dom_nodelist_has_dimension */

static zval *dom_nodemap_read_dimension(zend_object *object, zval *offset, int type, zval *rv) /* {{{ */
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot access DOMNamedNodeMap without offset");
		return NULL;
	}

	zend_long lval;
	if (dom_nodemap_or_nodelist_process_offset_as_named(offset, &lval)) {
		/* exceptional case, switch to named lookup */
		php_dom_named_node_map_get_named_item_into_zval(php_dom_obj_from_obj(object)->ptr, Z_STRVAL_P(offset), rv);
		return rv;
	}

	/* see PHP_METHOD(DOMNamedNodeMap, item) */
	if (UNEXPECTED(lval < 0 || ZEND_LONG_INT_OVFL(lval))) {
		zend_value_error("must be between 0 and %d", INT_MAX);
		return NULL;
	}

	php_dom_named_node_map_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, lval, rv);
	return rv;
} /* }}} end dom_nodemap_read_dimension */

static int dom_nodemap_has_dimension(zend_object *object, zval *member, int check_empty)
{
	zend_long offset;
	if (dom_nodemap_or_nodelist_process_offset_as_named(member, &offset)) {
		/* exceptional case, switch to named lookup */
		return php_dom_named_node_map_get_named_item(php_dom_obj_from_obj(object)->ptr, Z_STRVAL_P(member), false) != NULL;
	}

	return offset >= 0 && offset < php_dom_get_namednodemap_length(php_dom_obj_from_obj(object));
} /* }}} end dom_nodemap_has_dimension */

#endif /* HAVE_DOM */
