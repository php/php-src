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
#include "php_dom.h"
#include "namespace_compat.h"
#include "internal_helpers.h"
#include "php_dom_arginfo.h"
#include "dom_properties.h"
#include "zend_interfaces.h"
#include "lexbor/lexbor/core/types.h"
#include "lexbor/lexbor/core/lexbor.h"

#include "ext/standard/info.h"

/* {{{ class entries */
PHP_DOM_EXPORT zend_class_entry *dom_node_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_node_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_domexception_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_parentnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_parentnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_childnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_childnode_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_domimplementation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_domimplementation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_documentfragment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_documentfragment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_document_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_html_document_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_xml_document_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_nodelist_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_nodelist_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_namednodemap_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_namednodemap_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_dtd_namednodemap_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_html_collection_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_characterdata_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_characterdata_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_attr_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_attr_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_element_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_element_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_text_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_text_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_comment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_comment_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_cdatasection_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_cdatasection_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_documenttype_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_documenttype_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_notation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_notation_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_entity_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_entity_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_entityreference_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_entityreference_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_processinginstruction_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_processinginstruction_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_abstract_base_document_class_entry;
#ifdef LIBXML_XPATH_ENABLED
PHP_DOM_EXPORT zend_class_entry *dom_xpath_class_entry;
PHP_DOM_EXPORT zend_class_entry *dom_modern_xpath_class_entry;
#endif
PHP_DOM_EXPORT zend_class_entry *dom_namespace_node_class_entry;
/* }}} */

static zend_object_handlers dom_object_handlers;
static zend_object_handlers dom_nnodemap_object_handlers;
static zend_object_handlers dom_nodelist_object_handlers;
static zend_object_handlers dom_modern_nnodemap_object_handlers;
static zend_object_handlers dom_modern_nodelist_object_handlers;
static zend_object_handlers dom_object_namespace_node_handlers;
static zend_object_handlers dom_modern_domimplementation_object_handlers;
#ifdef LIBXML_XPATH_ENABLED
zend_object_handlers dom_xpath_object_handlers;
#endif

static HashTable classes;
/* {{{ prop handler tables */
static HashTable dom_document_prop_handlers;
static HashTable dom_xml_document_prop_handlers;
static HashTable dom_abstract_base_document_prop_handlers;
static HashTable dom_documentfragment_prop_handlers;
static HashTable dom_modern_documentfragment_prop_handlers;
static HashTable dom_node_prop_handlers;
static HashTable dom_modern_node_prop_handlers;
static HashTable dom_nodelist_prop_handlers;
static HashTable dom_namednodemap_prop_handlers;
static HashTable dom_characterdata_prop_handlers;
static HashTable dom_modern_characterdata_prop_handlers;
static HashTable dom_attr_prop_handlers;
static HashTable dom_modern_attr_prop_handlers;
static HashTable dom_element_prop_handlers;
static HashTable dom_modern_element_prop_handlers;
static HashTable dom_modern_element_prop_handlers;
static HashTable dom_text_prop_handlers;
static HashTable dom_modern_text_prop_handlers;
static HashTable dom_documenttype_prop_handlers;
static HashTable dom_modern_documenttype_prop_handlers;
static HashTable dom_notation_prop_handlers;
static HashTable dom_modern_notation_prop_handlers;
static HashTable dom_entity_prop_handlers;
static HashTable dom_modern_entity_prop_handlers;
static HashTable dom_processinginstruction_prop_handlers;
static HashTable dom_modern_processinginstruction_prop_handlers;
static HashTable dom_namespace_node_prop_handlers;
#ifdef LIBXML_XPATH_ENABLED
static HashTable dom_xpath_prop_handlers;
#endif
/* }}} */

static zend_object *dom_objects_namespace_node_new(zend_class_entry *class_type);
static void dom_object_namespace_node_free_storage(zend_object *object);
static xmlNodePtr php_dom_create_fake_namespace_decl_node_ptr(xmlNodePtr nodep, xmlNsPtr original);

typedef zend_result (*dom_read_t)(dom_object *obj, zval *retval);
typedef zend_result (*dom_write_t)(dom_object *obj, zval *newval);

typedef struct _dom_prop_handler {
	dom_read_t read_func;
	dom_write_t write_func;
} dom_prop_handler;

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

bool dom_node_children_valid(xmlNodePtr node) {
	switch (node->type) {
		case XML_DOCUMENT_TYPE_NODE:
		case XML_DTD_NODE:
		case XML_PI_NODE:
		case XML_COMMENT_NODE:
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_NOTATION_NODE:
			return false;
		default:
			return true;
	}
}

static const libxml_doc_props default_doc_props = {
	.formatoutput = false,
	.validateonparse = false,
	.resolveexternals = false,
	.preservewhitespace = true,
	.substituteentities = false,
	.stricterror = true,
	.recover = false,
	.classmap = NULL,
};

/* {{{ dom_get_doc_props() */
dom_doc_propsptr dom_get_doc_props(php_libxml_ref_obj *document)
{
	dom_doc_propsptr doc_props;

	if (document && document->doc_props) {
		return document->doc_props;
	} else {
		doc_props = emalloc(sizeof(libxml_doc_props));
		memcpy(doc_props, &default_doc_props, sizeof(libxml_doc_props));
		if (document) {
			document->doc_props = doc_props;
		}
		return doc_props;
	}
}
/* }}} */

libxml_doc_props const* dom_get_doc_props_read_only(const php_libxml_ref_obj *document)
{
	if (document && document->doc_props) {
		return document->doc_props;
	} else {
		return &default_doc_props;
	}
}

static void dom_copy_document_ref(php_libxml_ref_obj *source_doc, php_libxml_ref_obj *dest_doc)
{
	dom_doc_propsptr dest;

	if (source_doc && dest_doc) {

		libxml_doc_props const* source = dom_get_doc_props_read_only(source_doc);
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
			zend_hash_init(dest->classmap, 0, NULL, NULL, false);
			zend_hash_copy(dest->classmap, source->classmap, NULL);
		}

		dest_doc->class_type = source_doc->class_type;
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
			zend_hash_init(doc_props->classmap, 0, NULL, NULL, false);
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
	if (document) {
		libxml_doc_props const* doc_props = dom_get_doc_props_read_only(document);
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
bool dom_get_strict_error(php_libxml_ref_obj *document) {
	return dom_get_doc_props_read_only(document)->stricterror;
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

static void dom_register_prop_handler(HashTable *prop_handler, const char *name, size_t name_len, const dom_prop_handler *hnd)
{
	zend_string *str = zend_string_init_interned(name, name_len, true);
	zend_hash_add_new_ptr(prop_handler, str, (void *) hnd);
	zend_string_release_ex(str, true);
}

static void dom_overwrite_prop_handler(HashTable *prop_handler, const char *name, size_t name_len, const dom_prop_handler *hnd)
{
	zend_hash_str_update_ptr(prop_handler, name, name_len, (void *) hnd);
}

#define DOM_REGISTER_PROP_HANDLER(prop_handler, name, prop_read_func, prop_write_func) do { \
		static const dom_prop_handler hnd = {.read_func = prop_read_func, .write_func = prop_write_func}; \
		dom_register_prop_handler(prop_handler, "" name, sizeof("" name) - 1, &hnd); \
	} while (0)

#define DOM_OVERWRITE_PROP_HANDLER(prop_handler, name, prop_read_func, prop_write_func) do { \
		static const dom_prop_handler hnd = {.read_func = prop_read_func, .write_func = prop_write_func}; \
		dom_overwrite_prop_handler(prop_handler, "" name, sizeof("" name) - 1, &hnd); \
	} while (0)

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
			zend_throw_error(NULL, "Cannot modify readonly property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
			return &EG(error_zval);
		}

		zend_property_info *prop = zend_get_property_info(object->ce, name, /* silent */ true);
		ZEND_ASSERT(prop && ZEND_TYPE_IS_SET(prop->type));
		zval tmp;
		ZVAL_COPY(&tmp, value);
		if (!zend_verify_property_type(prop, &tmp, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)))) {
			zval_ptr_dtor(&tmp);
			return &EG(error_zval);
		}
		hnd->write_func(obj, &tmp);
		zval_ptr_dtor(&tmp);

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

	object_str = ZSTR_INIT_LITERAL("(object value omitted)", false);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(prop_handlers, string_key, entry) {
		zval value;

		ZEND_ASSERT(string_key != NULL);

		if (entry->read_func(obj, &value) == FAILURE) {
			continue;
		}

		if (Z_TYPE(value) == IS_OBJECT) {
			zval_ptr_dtor(&value);
			ZVAL_NEW_STR(&value, object_str);
			zend_string_addref(object_str);
		}

		zend_hash_update(debug_info, string_key, &value);
	} ZEND_HASH_FOREACH_END();

	zend_string_release_ex(object_str, false);

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
static void dom_import_simplexml_common(INTERNAL_FUNCTION_PARAMETERS, php_libxml_class_type new_class)
{
	zval *node;
	xmlNodePtr nodep = NULL;
	php_libxml_node_object *nodeobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &node) == FAILURE) {
		RETURN_THROWS();
	}

	nodeobj = (php_libxml_node_object *) ((char *) Z_OBJ_P(node) - Z_OBJ_HT_P(node)->offset);
	nodep = php_libxml_import_node(node);

	if (nodep && nodeobj && nodeobj->document && (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE)) {
		php_libxml_class_type old_class_type = nodeobj->document->class_type;
		if (old_class_type != PHP_LIBXML_CLASS_UNSET && old_class_type != new_class) {
			if (new_class == PHP_LIBXML_CLASS_MODERN) {
				zend_argument_type_error(1, "must not be already imported as a DOMNode");
			} else {
				zend_argument_type_error(1, "must not be already imported as a DOM\\Node");
			}			
			RETURN_THROWS();
		}

		/* Lock the node class type to prevent creating multiple representations of the same node. */
		nodeobj->document->class_type = new_class;

		if (old_class_type != PHP_LIBXML_CLASS_MODERN && new_class == PHP_LIBXML_CLASS_MODERN && nodep->doc != NULL) {
			dom_document_convert_to_modern(nodeobj->document, nodep->doc);
		}

		DOM_RET_OBJ((xmlNodePtr) nodep, (dom_object *)nodeobj);
	} else {
		zend_argument_type_error(1, "is not a valid node type");
		RETURN_THROWS();
	}
}

PHP_FUNCTION(dom_import_simplexml)
{
	dom_import_simplexml_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_LIBXML_CLASS_LEGACY);
}

PHP_FUNCTION(DOM_import_simplexml)
{
	dom_import_simplexml_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_LIBXML_CLASS_MODERN);
}
/* }}} */

static dom_object* dom_objects_set_class(zend_class_entry *class_type);

void php_dom_update_document_after_clone(dom_object *original, xmlNodePtr original_node, dom_object *clone, xmlNodePtr cloned_node)
{
	dom_copy_document_ref(original->document, clone->document);
	/* Workaround libxml2 bug, see https://gitlab.gnome.org/GNOME/libxml2/-/commit/07920b4381873187c02df53fa9b5d44aff3a7041 */
#if LIBXML_VERSION < 20911
	if (original_node->type == XML_HTML_DOCUMENT_NODE) {
		cloned_node->type = XML_HTML_DOCUMENT_NODE;
	}
#endif
}

static void dom_update_refcount_after_clone(dom_object *original, xmlNodePtr original_node, dom_object *clone, xmlNodePtr cloned_node)
{
	/* If we cloned a document then we must create new doc proxy */
	if (cloned_node->doc == original_node->doc) {
		clone->document = original->document;
	}
	php_libxml_increment_doc_ref((php_libxml_node_object *)clone, cloned_node->doc);
	php_libxml_increment_node_ptr((php_libxml_node_object *)clone, cloned_node, (void *)clone);
	if (original->document != clone->document) {
		php_dom_update_document_after_clone(original, original_node, clone, cloned_node);
	}
}

static zend_object *dom_objects_store_clone_obj(zend_object *zobject) /* {{{ */
{
	dom_object *intern = php_dom_obj_from_obj(zobject);
	dom_object *clone = dom_objects_set_class(intern->std.ce);

	if (instanceof_function(intern->std.ce, dom_node_class_entry) || instanceof_function(intern->std.ce, dom_modern_node_class_entry)) {
		xmlNodePtr node = (xmlNodePtr)dom_object_get_node(intern);
		if (node != NULL) {
			php_dom_libxml_ns_mapper *ns_mapper = NULL;
			if (php_dom_follow_spec_intern(intern)) {
				if (node->type == XML_DOCUMENT_NODE || node->type == XML_HTML_DOCUMENT_NODE) {
					ns_mapper = php_dom_libxml_ns_mapper_create();
				} else {
					ns_mapper = php_dom_get_ns_mapper(intern);
				}
			}

			xmlNodePtr cloned_node = dom_clone_node(ns_mapper, node, node->doc, true);
			if (cloned_node != NULL) {
				dom_update_refcount_after_clone(intern, node, clone, cloned_node);
			}
			clone->document->private_data = php_dom_libxml_ns_mapper_header(ns_mapper);
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
static zval *dom_modern_nodemap_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
static int dom_modern_nodemap_has_dimension(zend_object *object, zval *member, int check_empty);
static zval *dom_modern_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
static int dom_modern_nodelist_has_dimension(zend_object *object, zval *member, int check_empty);
static zend_object *dom_objects_store_clone_obj(zend_object *zobject);

#ifdef LIBXML_XPATH_ENABLED
void dom_xpath_objects_free_storage(zend_object *object);
HashTable *dom_xpath_get_gc(zend_object *object, zval **table, int *n);
#endif

static void *dom_malloc(size_t size) {
	return emalloc(size);
}

static void *dom_realloc(void *dst, size_t size) {
	return erealloc(dst, size);
}

static void *dom_calloc(size_t num, size_t size) {
	return ecalloc(num, size);
}

static void dom_free(void *ptr) {
	efree(ptr);
}

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

	memcpy(&dom_modern_domimplementation_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	/* The IDL has the [SameObject] constraint, which is incompatible with cloning because it imposes that there is only
	 * one instance per parent object. */
	dom_modern_domimplementation_object_handlers.clone_obj = NULL;

	memcpy(&dom_nnodemap_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_nnodemap_object_handlers.free_obj = dom_nnodemap_objects_free_storage;
	dom_nnodemap_object_handlers.read_dimension = dom_nodemap_read_dimension;
	dom_nnodemap_object_handlers.has_dimension = dom_nodemap_has_dimension;

	memcpy(&dom_nodelist_object_handlers, &dom_nnodemap_object_handlers, sizeof(zend_object_handlers));
	dom_nodelist_object_handlers.read_dimension = dom_nodelist_read_dimension;
	dom_nodelist_object_handlers.has_dimension = dom_nodelist_has_dimension;

	memcpy(&dom_modern_nnodemap_object_handlers, &dom_nnodemap_object_handlers, sizeof(zend_object_handlers));
	dom_modern_nnodemap_object_handlers.read_dimension = dom_modern_nodemap_read_dimension;
	dom_modern_nnodemap_object_handlers.has_dimension = dom_modern_nodemap_has_dimension;

	memcpy(&dom_modern_nodelist_object_handlers, &dom_nodelist_object_handlers, sizeof(zend_object_handlers));
	dom_modern_nodelist_object_handlers.read_dimension = dom_modern_nodelist_read_dimension;
	dom_modern_nodelist_object_handlers.has_dimension = dom_modern_nodelist_has_dimension;

	memcpy(&dom_object_namespace_node_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_object_namespace_node_handlers.offset = XtOffsetOf(dom_object_namespace_node, dom.std);
	dom_object_namespace_node_handlers.free_obj = dom_object_namespace_node_free_storage;
	dom_object_namespace_node_handlers.clone_obj = dom_object_namespace_node_clone_obj;

	zend_hash_init(&classes, 0, NULL, NULL, true);

	dom_domexception_class_entry = register_class_DOMException(zend_ce_exception);

	dom_parentnode_class_entry = register_class_DOMParentNode();
	dom_modern_parentnode_class_entry = register_class_DOM_ParentNode();
	dom_childnode_class_entry = register_class_DOMChildNode();
	dom_modern_childnode_class_entry = register_class_DOM_ChildNode();

	dom_domimplementation_class_entry = register_class_DOMImplementation();
	dom_domimplementation_class_entry->create_object = dom_objects_new;
	dom_domimplementation_class_entry->default_object_handlers = &dom_object_handlers;

	dom_modern_domimplementation_class_entry = register_class_DOM_Implementation();
	dom_modern_domimplementation_class_entry->create_object = dom_objects_new;
	dom_modern_domimplementation_class_entry->default_object_handlers = &dom_modern_domimplementation_object_handlers;

	dom_node_class_entry = register_class_DOMNode();
	dom_node_class_entry->create_object = dom_objects_new;
	dom_node_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_node_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "nodeName", dom_node_node_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "nodeValue", dom_node_node_value_read, dom_node_node_value_write);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "nodeType", dom_node_node_type_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "parentNode", dom_node_parent_node_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "parentElement", dom_node_parent_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "childNodes", dom_node_child_nodes_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "firstChild", dom_node_first_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "lastChild", dom_node_last_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "previousSibling", dom_node_previous_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "nextSibling", dom_node_next_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "attributes", dom_node_attributes_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "isConnected", dom_node_is_connected_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "ownerDocument", dom_node_owner_document_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "prefix", dom_node_prefix_read, dom_node_prefix_write);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "localName", dom_node_local_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "baseURI", dom_node_base_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_node_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_new_ptr(&classes, dom_node_class_entry->name, &dom_node_prop_handlers);

	dom_modern_node_class_entry = register_class_DOM_Node();
	dom_modern_node_class_entry->create_object = dom_objects_new;
	dom_modern_node_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_node_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "nodeType", dom_node_node_type_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "nodeName", dom_node_node_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "baseURI", dom_node_base_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "isConnected", dom_node_is_connected_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "ownerDocument", dom_node_owner_document_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "parentNode", dom_node_parent_node_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "parentElement", dom_node_parent_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "childNodes", dom_node_child_nodes_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "firstChild", dom_node_first_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "lastChild", dom_node_last_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "previousSibling", dom_node_previous_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "nextSibling", dom_node_next_sibling_read, NULL);
	/* We will set-up the setter for the derived classes afterwards on a class-by-class basis. */
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "nodeValue", dom_node_node_value_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_node_prop_handlers, "textContent", dom_node_text_content_read, NULL);
	zend_hash_add_new_ptr(&classes, dom_modern_node_class_entry->name, &dom_modern_node_prop_handlers);

	dom_namespace_node_class_entry = register_class_DOMNameSpaceNode();
	dom_namespace_node_class_entry->create_object = dom_objects_namespace_node_new;
	dom_namespace_node_class_entry->default_object_handlers = &dom_object_namespace_node_handlers;

	zend_hash_init(&dom_namespace_node_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "nodeName", dom_node_node_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "nodeValue", dom_node_node_value_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "nodeType", dom_node_node_type_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "prefix", dom_node_prefix_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "localName", dom_node_local_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "isConnected", dom_node_is_connected_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "ownerDocument", dom_node_owner_document_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "parentNode", dom_node_parent_node_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_namespace_node_prop_handlers, "parentElement", dom_node_parent_element_read, NULL);
	zend_hash_add_new_ptr(&classes, dom_namespace_node_class_entry->name, &dom_namespace_node_prop_handlers);

	dom_documentfragment_class_entry = register_class_DOMDocumentFragment(dom_node_class_entry, dom_parentnode_class_entry);
	dom_documentfragment_class_entry->create_object = dom_objects_new;
	dom_documentfragment_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_init(&dom_documentfragment_prop_handlers, 0, NULL, NULL, true);

	DOM_REGISTER_PROP_HANDLER(&dom_documentfragment_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documentfragment_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documentfragment_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);
	zend_hash_merge(&dom_documentfragment_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_documentfragment_class_entry->name, &dom_documentfragment_prop_handlers);

	dom_modern_documentfragment_class_entry = register_class_DOM_DocumentFragment(dom_modern_node_class_entry, dom_modern_parentnode_class_entry);
	dom_modern_documentfragment_class_entry->create_object = dom_objects_new;
	dom_modern_documentfragment_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_init(&dom_modern_documentfragment_prop_handlers, 0, NULL, NULL, true);

	DOM_REGISTER_PROP_HANDLER(&dom_modern_documentfragment_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documentfragment_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documentfragment_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);
	zend_hash_merge(&dom_modern_documentfragment_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_documentfragment_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_new_ptr(&classes, dom_modern_documentfragment_class_entry->name, &dom_modern_documentfragment_prop_handlers);

	dom_abstract_base_document_class_entry = register_class_DOM_Document(dom_modern_node_class_entry, dom_modern_parentnode_class_entry);
	dom_abstract_base_document_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_init(&dom_abstract_base_document_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "implementation", dom_modern_document_implementation_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "URL", dom_document_document_uri_read, dom_document_document_uri_write);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "documentURI", dom_document_document_uri_read, dom_document_document_uri_write);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "characterSet", dom_document_encoding_read, dom_html_document_encoding_write);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "charset", dom_document_encoding_read, dom_html_document_encoding_write);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "inputEncoding", dom_document_encoding_read, dom_html_document_encoding_write);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "doctype", dom_document_doctype_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "documentElement", dom_document_document_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "body", dom_html_document_body_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_abstract_base_document_prop_handlers, "head", dom_html_document_head_read, NULL);
	zend_hash_merge(&dom_abstract_base_document_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	/* No need to register in &classes because this is an abstract class handler. */

	dom_document_class_entry = register_class_DOMDocument(dom_node_class_entry, dom_parentnode_class_entry);
	dom_document_class_entry->create_object = dom_objects_new;
	dom_document_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_init(&dom_document_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "doctype", dom_document_doctype_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "implementation", dom_document_implementation_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "documentElement", dom_document_document_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "actualEncoding", dom_document_encoding_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "encoding", dom_document_encoding_read, dom_document_encoding_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "xmlEncoding", dom_document_encoding_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "standalone", dom_document_standalone_read, dom_document_standalone_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "xmlStandalone", dom_document_standalone_read, dom_document_standalone_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "version", dom_document_version_read, dom_document_version_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "xmlVersion", dom_document_version_read, dom_document_version_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "strictErrorChecking", dom_document_strict_error_checking_read, dom_document_strict_error_checking_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "documentURI", dom_document_document_uri_read, dom_document_document_uri_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "config", dom_document_config_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "formatOutput", dom_document_format_output_read, dom_document_format_output_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "validateOnParse", dom_document_validate_on_parse_read, dom_document_validate_on_parse_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "resolveExternals", dom_document_resolve_externals_read, dom_document_resolve_externals_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "preserveWhiteSpace", dom_document_preserve_whitespace_read, dom_document_preserve_whitespace_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "recover", dom_document_recover_read, dom_document_recover_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "substituteEntities", dom_document_substitue_entities_read, dom_document_substitue_entities_write);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_document_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);

	zend_hash_merge(&dom_document_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_document_class_entry->name, &dom_document_prop_handlers);

	dom_html_document_class_entry = register_class_DOM_HTMLDocument(dom_abstract_base_document_class_entry);
	dom_html_document_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_html_document_class_entry->name, &dom_abstract_base_document_prop_handlers);

	dom_xml_document_class_entry = register_class_DOM_XMLDocument(dom_abstract_base_document_class_entry);
	dom_xml_document_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_init(&dom_xml_document_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_xml_document_prop_handlers, "xmlEncoding", dom_document_encoding_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_xml_document_prop_handlers, "xmlStandalone", dom_document_standalone_read, dom_document_standalone_write);
	DOM_REGISTER_PROP_HANDLER(&dom_xml_document_prop_handlers, "xmlVersion", dom_document_version_read, dom_document_version_write);
	DOM_REGISTER_PROP_HANDLER(&dom_xml_document_prop_handlers, "formatOutput", dom_document_format_output_read, dom_document_format_output_write);

	zend_hash_merge(&dom_xml_document_prop_handlers, &dom_abstract_base_document_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_xml_document_class_entry->name, &dom_xml_document_prop_handlers);

	dom_nodelist_class_entry = register_class_DOMNodeList(zend_ce_aggregate, zend_ce_countable);
	dom_nodelist_class_entry->create_object = dom_nnodemap_objects_new;
	dom_nodelist_class_entry->default_object_handlers = &dom_nodelist_object_handlers;
	dom_nodelist_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_init(&dom_nodelist_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_nodelist_prop_handlers, "length", dom_nodelist_length_read, NULL);
	zend_hash_add_new_ptr(&classes, dom_nodelist_class_entry->name, &dom_nodelist_prop_handlers);

	dom_modern_nodelist_class_entry = register_class_DOM_NodeList(zend_ce_aggregate, zend_ce_countable);
	dom_modern_nodelist_class_entry->create_object = dom_nnodemap_objects_new;
	dom_modern_nodelist_class_entry->default_object_handlers = &dom_modern_nodelist_object_handlers;
	dom_modern_nodelist_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_add_new_ptr(&classes, dom_modern_nodelist_class_entry->name, &dom_nodelist_prop_handlers);

	dom_namednodemap_class_entry = register_class_DOMNamedNodeMap(zend_ce_aggregate, zend_ce_countable);
	dom_namednodemap_class_entry->create_object = dom_nnodemap_objects_new;
	dom_namednodemap_class_entry->default_object_handlers = &dom_nnodemap_object_handlers;
	dom_namednodemap_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_init(&dom_namednodemap_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_namednodemap_prop_handlers, "length", dom_namednodemap_length_read, NULL);
	zend_hash_add_new_ptr(&classes, dom_namednodemap_class_entry->name, &dom_namednodemap_prop_handlers);

	dom_modern_namednodemap_class_entry = register_class_DOM_NamedNodeMap(zend_ce_aggregate, zend_ce_countable);
	dom_modern_namednodemap_class_entry->create_object = dom_nnodemap_objects_new;
	dom_modern_namednodemap_class_entry->default_object_handlers = &dom_modern_nnodemap_object_handlers;
	dom_modern_namednodemap_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_add_new_ptr(&classes, dom_modern_namednodemap_class_entry->name, &dom_namednodemap_prop_handlers);

	dom_modern_dtd_namednodemap_class_entry = register_class_DOM_DTDNamedNodeMap(zend_ce_aggregate, zend_ce_countable);
	dom_modern_dtd_namednodemap_class_entry->create_object = dom_nnodemap_objects_new;
	dom_modern_dtd_namednodemap_class_entry->default_object_handlers = &dom_modern_nnodemap_object_handlers;
	dom_modern_dtd_namednodemap_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_add_new_ptr(&classes, dom_modern_dtd_namednodemap_class_entry->name, &dom_namednodemap_prop_handlers);

	dom_html_collection_class_entry = register_class_DOM_HTMLCollection(zend_ce_aggregate, zend_ce_countable);
	dom_html_collection_class_entry->create_object = dom_nnodemap_objects_new;
	dom_html_collection_class_entry->default_object_handlers = &dom_modern_nodelist_object_handlers;
	dom_html_collection_class_entry->get_iterator = php_dom_get_iterator;

	zend_hash_add_new_ptr(&classes, dom_html_collection_class_entry->name, &dom_nodelist_prop_handlers);

	dom_characterdata_class_entry = register_class_DOMCharacterData(dom_node_class_entry, dom_childnode_class_entry);
	dom_characterdata_class_entry->create_object = dom_objects_new;
	dom_characterdata_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_characterdata_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_characterdata_prop_handlers, "data", dom_characterdata_data_read, dom_characterdata_data_write);
	DOM_REGISTER_PROP_HANDLER(&dom_characterdata_prop_handlers, "length", dom_characterdata_length_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_characterdata_prop_handlers, "previousElementSibling", dom_node_previous_element_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_characterdata_prop_handlers, "nextElementSibling", dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_characterdata_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_characterdata_class_entry->name, &dom_characterdata_prop_handlers);

	dom_modern_characterdata_class_entry = register_class_DOM_CharacterData(dom_modern_node_class_entry, dom_modern_childnode_class_entry);
	dom_modern_characterdata_class_entry->create_object = dom_objects_new;
	dom_modern_characterdata_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_characterdata_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "data", dom_characterdata_data_read, dom_characterdata_data_write);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "length", dom_characterdata_length_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "previousElementSibling", dom_node_previous_element_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "nextElementSibling", dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_modern_characterdata_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "nodeValue", dom_node_node_value_read, dom_node_node_value_write);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_characterdata_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_new_ptr(&classes, dom_modern_characterdata_class_entry->name, &dom_modern_characterdata_prop_handlers);

	dom_attr_class_entry = register_class_DOMAttr(dom_node_class_entry);
	dom_attr_class_entry->create_object = dom_objects_new;
	dom_attr_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_attr_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_attr_prop_handlers, "name", dom_attr_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_attr_prop_handlers, "specified", dom_attr_specified_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_attr_prop_handlers, "value", dom_attr_value_read, dom_attr_value_write);
	DOM_REGISTER_PROP_HANDLER(&dom_attr_prop_handlers, "ownerElement", dom_attr_owner_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_attr_prop_handlers, "schemaTypeInfo", dom_attr_schema_type_info_read, NULL);
	zend_hash_merge(&dom_attr_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_attr_class_entry->name, &dom_attr_prop_handlers);

	dom_modern_attr_class_entry = register_class_DOM_Attr(dom_modern_node_class_entry);
	dom_modern_attr_class_entry->create_object = dom_objects_new;
	dom_modern_attr_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_attr_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "prefix", dom_modern_node_prefix_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "localName", dom_node_local_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "name", dom_attr_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "value", dom_attr_value_read, dom_attr_value_write);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "ownerElement", dom_attr_owner_element_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_attr_prop_handlers, "specified", dom_attr_specified_read, NULL);
	zend_hash_merge(&dom_modern_attr_prop_handlers, &dom_node_prop_handlers, NULL, false);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_attr_prop_handlers, "nodeValue", dom_node_node_value_read, dom_node_node_value_write);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_attr_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_new_ptr(&classes, dom_modern_attr_class_entry->name, &dom_modern_attr_prop_handlers);

	dom_element_class_entry = register_class_DOMElement(dom_node_class_entry, dom_parentnode_class_entry, dom_childnode_class_entry);
	dom_element_class_entry->create_object = dom_objects_new;
	dom_element_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_element_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "tagName", dom_element_tag_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "className", dom_element_class_name_read, dom_element_class_name_write);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "id", dom_element_id_read, dom_element_id_write);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "schemaTypeInfo", dom_element_schema_type_info_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "previousElementSibling", dom_node_previous_element_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_element_prop_handlers, "nextElementSibling", dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_element_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_element_class_entry->name, &dom_element_prop_handlers);

	dom_modern_element_class_entry = register_class_DOM_Element(dom_modern_node_class_entry, dom_modern_parentnode_class_entry, dom_modern_childnode_class_entry);
	dom_modern_element_class_entry->create_object = dom_objects_new;
	dom_modern_element_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_element_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "prefix", dom_modern_node_prefix_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "localName", dom_node_local_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "tagName", dom_element_tag_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "id", dom_element_id_read, dom_element_id_write);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "className", dom_element_class_name_read, dom_element_class_name_write);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "attributes", dom_node_attributes_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "firstElementChild", dom_parent_node_first_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "lastElementChild", dom_parent_node_last_element_child_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "childElementCount", dom_parent_node_child_element_count, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "previousElementSibling", dom_node_previous_element_sibling_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_element_prop_handlers, "nextElementSibling", dom_node_next_element_sibling_read, NULL);
	zend_hash_merge(&dom_modern_element_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	DOM_OVERWRITE_PROP_HANDLER(&dom_modern_element_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_new_ptr(&classes, dom_modern_element_class_entry->name, &dom_modern_element_prop_handlers);

	dom_text_class_entry = register_class_DOMText(dom_characterdata_class_entry);
	dom_text_class_entry->create_object = dom_objects_new;
	dom_text_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_text_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_text_prop_handlers, "wholeText", dom_text_whole_text_read, NULL);
	zend_hash_merge(&dom_text_prop_handlers, &dom_characterdata_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_text_class_entry->name, &dom_text_prop_handlers);

	dom_modern_text_class_entry = register_class_DOM_Text(dom_modern_characterdata_class_entry);
	dom_modern_text_class_entry->create_object = dom_objects_new;
	dom_modern_text_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_text_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_text_prop_handlers, "wholeText", dom_text_whole_text_read, NULL);
	zend_hash_merge(&dom_modern_text_prop_handlers, &dom_modern_characterdata_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_modern_text_class_entry->name, &dom_modern_text_prop_handlers);

	dom_comment_class_entry = register_class_DOMComment(dom_characterdata_class_entry);
	dom_comment_class_entry->create_object = dom_objects_new;
	dom_comment_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_comment_class_entry->name, &dom_characterdata_prop_handlers);

	dom_modern_comment_class_entry = register_class_DOM_Comment(dom_modern_characterdata_class_entry);
	dom_modern_comment_class_entry->create_object = dom_objects_new;
	dom_modern_comment_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_modern_comment_class_entry->name, &dom_modern_characterdata_prop_handlers);

	dom_cdatasection_class_entry = register_class_DOMCdataSection(dom_text_class_entry);
	dom_cdatasection_class_entry->create_object = dom_objects_new;
	dom_cdatasection_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_cdatasection_class_entry->name, &dom_text_prop_handlers);

	dom_modern_cdatasection_class_entry = register_class_DOM_CDATASection(dom_modern_text_class_entry);
	dom_modern_cdatasection_class_entry->create_object = dom_objects_new;
	dom_modern_cdatasection_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_modern_cdatasection_class_entry->name, &dom_modern_text_prop_handlers);

	dom_documenttype_class_entry = register_class_DOMDocumentType(dom_node_class_entry);
	dom_documenttype_class_entry->create_object = dom_objects_new;
	dom_documenttype_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_documenttype_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "name", dom_documenttype_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "entities", dom_documenttype_entities_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "notations", dom_documenttype_notations_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "publicId", dom_documenttype_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "systemId", dom_documenttype_system_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_documenttype_prop_handlers, "internalSubset", dom_documenttype_internal_subset_read, NULL);
	zend_hash_merge(&dom_documenttype_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_documenttype_class_entry->name, &dom_documenttype_prop_handlers);

	dom_modern_documenttype_class_entry = register_class_DOM_DocumentType(dom_modern_node_class_entry, dom_modern_childnode_class_entry);
	dom_modern_documenttype_class_entry->create_object = dom_objects_new;
	dom_modern_documenttype_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_documenttype_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "name", dom_documenttype_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "entities", dom_documenttype_entities_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "notations", dom_documenttype_notations_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "publicId", dom_documenttype_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "systemId", dom_documenttype_system_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_documenttype_prop_handlers, "internalSubset", dom_documenttype_internal_subset_read, NULL);
	zend_hash_merge(&dom_modern_documenttype_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_modern_documenttype_class_entry->name, &dom_modern_documenttype_prop_handlers);

	dom_notation_class_entry = register_class_DOMNotation(dom_node_class_entry);
	dom_notation_class_entry->create_object = dom_objects_new;
	dom_notation_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_notation_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_notation_prop_handlers, "publicId", dom_notation_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_notation_prop_handlers, "systemId", dom_notation_system_id_read, NULL);
	zend_hash_merge(&dom_notation_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_notation_class_entry->name, &dom_notation_prop_handlers);

	dom_modern_notation_class_entry = register_class_DOM_Notation(dom_modern_node_class_entry);
	dom_modern_notation_class_entry->create_object = dom_objects_new;
	dom_modern_notation_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_notation_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_notation_prop_handlers, "publicId", dom_notation_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_notation_prop_handlers, "systemId", dom_notation_system_id_read, NULL);
	zend_hash_merge(&dom_modern_notation_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_modern_notation_class_entry->name, &dom_modern_node_prop_handlers);

	dom_entity_class_entry = register_class_DOMEntity(dom_node_class_entry);
	dom_entity_class_entry->create_object = dom_objects_new;
	dom_entity_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_entity_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "publicId", dom_entity_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "systemId", dom_entity_system_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "notationName", dom_entity_notation_name_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "actualEncoding", dom_entity_actual_encoding_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "encoding", dom_entity_encoding_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_entity_prop_handlers, "version", dom_entity_version_read, NULL);
	zend_hash_merge(&dom_entity_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_entity_class_entry->name, &dom_entity_prop_handlers);

	dom_modern_entity_class_entry = register_class_DOM_Entity(dom_modern_node_class_entry);
	dom_modern_entity_class_entry->create_object = dom_objects_new;
	dom_modern_entity_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_entity_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_entity_prop_handlers, "publicId", dom_entity_public_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_entity_prop_handlers, "systemId", dom_entity_system_id_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_entity_prop_handlers, "notationName", dom_entity_notation_name_read, NULL);
	zend_hash_merge(&dom_modern_entity_prop_handlers, &dom_modern_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_modern_entity_class_entry->name, &dom_modern_entity_prop_handlers);

	dom_entityreference_class_entry = register_class_DOMEntityReference(dom_node_class_entry);
	dom_entityreference_class_entry->create_object = dom_objects_new;
	dom_entityreference_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_entityreference_class_entry->name, &dom_node_prop_handlers);

	dom_modern_entityreference_class_entry = register_class_DOM_EntityReference(dom_modern_node_class_entry);
	dom_modern_entityreference_class_entry->create_object = dom_objects_new;
	dom_modern_entityreference_class_entry->default_object_handlers = &dom_object_handlers;
	zend_hash_add_new_ptr(&classes, dom_modern_entityreference_class_entry->name, &dom_modern_node_prop_handlers);

	dom_processinginstruction_class_entry = register_class_DOMProcessingInstruction(dom_node_class_entry);
	dom_processinginstruction_class_entry->create_object = dom_objects_new;
	dom_processinginstruction_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_processinginstruction_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_processinginstruction_prop_handlers, "target", dom_processinginstruction_target_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_processinginstruction_prop_handlers, "data", dom_processinginstruction_data_read, dom_processinginstruction_data_write);
	zend_hash_merge(&dom_processinginstruction_prop_handlers, &dom_node_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_processinginstruction_class_entry->name, &dom_processinginstruction_prop_handlers);

	dom_modern_processinginstruction_class_entry = register_class_DOM_ProcessingInstruction(dom_modern_characterdata_class_entry);
	dom_modern_processinginstruction_class_entry->create_object = dom_objects_new;
	dom_modern_processinginstruction_class_entry->default_object_handlers = &dom_object_handlers;

	zend_hash_init(&dom_modern_processinginstruction_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_processinginstruction_prop_handlers, "target", dom_processinginstruction_target_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_modern_processinginstruction_prop_handlers, "data", dom_processinginstruction_data_read, dom_processinginstruction_data_write);
	zend_hash_merge(&dom_modern_processinginstruction_prop_handlers, &dom_modern_characterdata_prop_handlers, NULL, false);
	zend_hash_add_new_ptr(&classes, dom_modern_processinginstruction_class_entry->name, &dom_modern_processinginstruction_prop_handlers);

#ifdef LIBXML_XPATH_ENABLED
	memcpy(&dom_xpath_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_xpath_object_handlers.offset = XtOffsetOf(dom_xpath_object, dom) + XtOffsetOf(dom_object, std);
	dom_xpath_object_handlers.free_obj = dom_xpath_objects_free_storage;
	dom_xpath_object_handlers.get_gc = dom_xpath_get_gc;
	dom_xpath_object_handlers.clone_obj = NULL;

	dom_xpath_class_entry = register_class_DOMXPath();
	dom_xpath_class_entry->create_object = dom_xpath_objects_new;
	dom_xpath_class_entry->default_object_handlers = &dom_xpath_object_handlers;

	zend_hash_init(&dom_xpath_prop_handlers, 0, NULL, NULL, true);
	DOM_REGISTER_PROP_HANDLER(&dom_xpath_prop_handlers, "document", dom_xpath_document_read, NULL);
	DOM_REGISTER_PROP_HANDLER(&dom_xpath_prop_handlers, "registerNodeNamespaces", dom_xpath_register_node_ns_read, dom_xpath_register_node_ns_write);
	zend_hash_add_new_ptr(&classes, dom_xpath_class_entry->name, &dom_xpath_prop_handlers);

	dom_modern_xpath_class_entry = register_class_DOM_XPath();
	dom_modern_xpath_class_entry->create_object = dom_xpath_objects_new;
	dom_modern_xpath_class_entry->default_object_handlers = &dom_xpath_object_handlers;

	zend_hash_add_new_ptr(&classes, dom_modern_xpath_class_entry->name, &dom_xpath_prop_handlers);
#endif

	register_php_dom_symbols(module_number);

	php_libxml_register_export(dom_node_class_entry, php_dom_export_node);
	php_libxml_register_export(dom_modern_node_class_entry, php_dom_export_node);

	lexbor_memory_setup(dom_malloc, dom_realloc, dom_calloc, dom_free);

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
	zend_hash_destroy(&dom_abstract_base_document_prop_handlers);
	zend_hash_destroy(&dom_xml_document_prop_handlers);
	zend_hash_destroy(&dom_documentfragment_prop_handlers);
	zend_hash_destroy(&dom_modern_documentfragment_prop_handlers);
	zend_hash_destroy(&dom_node_prop_handlers);
	zend_hash_destroy(&dom_modern_node_prop_handlers);
	zend_hash_destroy(&dom_namespace_node_prop_handlers);
	zend_hash_destroy(&dom_nodelist_prop_handlers);
	zend_hash_destroy(&dom_namednodemap_prop_handlers);
	zend_hash_destroy(&dom_characterdata_prop_handlers);
	zend_hash_destroy(&dom_modern_characterdata_prop_handlers);
	zend_hash_destroy(&dom_attr_prop_handlers);
	zend_hash_destroy(&dom_modern_attr_prop_handlers);
	zend_hash_destroy(&dom_element_prop_handlers);
	zend_hash_destroy(&dom_modern_element_prop_handlers);
	zend_hash_destroy(&dom_text_prop_handlers);
	zend_hash_destroy(&dom_modern_text_prop_handlers);
	zend_hash_destroy(&dom_documenttype_prop_handlers);
	zend_hash_destroy(&dom_modern_documenttype_prop_handlers);
	zend_hash_destroy(&dom_notation_prop_handlers);
	zend_hash_destroy(&dom_modern_notation_prop_handlers);
	zend_hash_destroy(&dom_entity_prop_handlers);
	zend_hash_destroy(&dom_modern_entity_prop_handlers);
	zend_hash_destroy(&dom_processinginstruction_prop_handlers);
	zend_hash_destroy(&dom_modern_processinginstruction_prop_handlers);
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

/* {{{ dom_objects_free_storage */
void dom_objects_free_storage(zend_object *object)
{
	dom_object *intern = php_dom_obj_from_obj(object);

	zend_object_std_dtor(&intern->std);

	if (intern->ptr != NULL && ((php_libxml_node_ptr *)intern->ptr)->node != NULL) {
		if (((xmlNodePtr) ((php_libxml_node_ptr *)intern->ptr)->node)->type != XML_DOCUMENT_NODE && ((xmlNodePtr) ((php_libxml_node_ptr *)intern->ptr)->node)->type != XML_HTML_DOCUMENT_NODE) {
			php_libxml_node_decrement_resource((php_libxml_node_object *) intern);
		} else {
			php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
			php_libxml_decrement_doc_ref((php_libxml_node_object *) intern);
		}
		intern->ptr = NULL;
	}
}
/* }}} */

void dom_namednode_iter(dom_object *basenode, int ntype, dom_object *intern, xmlHashTablePtr ht, const char *local, size_t local_len, const char *ns, size_t ns_len) /* {{{ */
{
	dom_nnodemap_object *mapptr = (dom_nnodemap_object *) intern->ptr;

	ZEND_ASSERT(basenode != NULL);

	ZVAL_OBJ_COPY(&mapptr->baseobj_zv, &basenode->std);

	xmlDocPtr doc = basenode->document ? basenode->document->ptr : NULL;

	mapptr->baseobj = basenode;
	mapptr->nodetype = ntype;
	mapptr->ht = ht;

	const xmlChar* tmp;

	if (local) {
		int len = local_len > INT_MAX ? -1 : (int) local_len;
		if (doc != NULL && (tmp = xmlDictExists(doc->dict, (const xmlChar *)local, len)) != NULL) {
			mapptr->local = BAD_CAST tmp;
		} else {
			mapptr->local = xmlCharStrndup(local, len);
			mapptr->free_local = true;
		}
		mapptr->local_lower = BAD_CAST estrdup(local);
		if (len < 0) {
			zend_str_tolower((char *) mapptr->local_lower, strlen((const char *) mapptr->local_lower));
		} else {
			zend_str_tolower((char *) mapptr->local_lower, len);
		}
	}

	if (ns) {
		int len = ns_len > INT_MAX ? -1 : (int) ns_len;
		if (doc != NULL && (tmp = xmlDictExists(doc->dict, (const xmlChar *)ns, len)) != NULL) {
			mapptr->ns = BAD_CAST tmp;
		} else {
			mapptr->ns = xmlCharStrndup(ns, len);
			mapptr->free_ns = true;
		}
	}
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
	return &intern->std;
}
/* }}} */

static zend_object *dom_objects_namespace_node_new(zend_class_entry *class_type)
{
	dom_object_namespace_node *intern = zend_object_alloc(sizeof(dom_object_namespace_node), class_type);
	dom_objects_set_class_ex(class_type, &intern->dom);
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

	php_dom_xpath_callbacks_ctor(&intern->xpath_callbacks);
	intern->register_node_ns = 1;

	intern->dom.prop_handler = &dom_xpath_prop_handlers;

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
		if (objmap->cached_obj && GC_DELREF(&objmap->cached_obj->std) == 0) {
			zend_objects_store_del(&objmap->cached_obj->std);
		}
		if (objmap->free_local) {
			xmlFree(objmap->local);
		}
		if (objmap->free_ns) {
			xmlFree(objmap->ns);
		}
		if (objmap->local_lower) {
			efree(objmap->local_lower);
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

zend_object *dom_nnodemap_objects_new(zend_class_entry *class_type)
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
	objmap->local_lower = NULL;
	objmap->free_local = false;
	objmap->ns = NULL;
	objmap->free_ns = false;
	objmap->cache_tag.modification_nr = 0;
	objmap->cached_length = -1;
	objmap->cached_obj = NULL;
	objmap->cached_obj_index = 0;

	return &intern->std;
}

void php_dom_create_iterator(zval *return_value, dom_iterator_type iterator_type, bool modern) /* {{{ */
{
	zend_class_entry *ce;

	if (iterator_type == DOM_NAMEDNODEMAP) {
		ce = dom_get_namednodemap_ce(modern);
	} else if (iterator_type == DOM_HTMLCOLLECTION) {
		/* This only exists in modern DOM. */
		ZEND_ASSERT(modern);
		ce = dom_html_collection_class_entry;
	} else if (iterator_type == DOM_DTD_NAMEDNODEMAP) {
		ce = dom_get_dtd_namednodemap_ce(modern);
	} else {
		ZEND_ASSERT(iterator_type == DOM_NODELIST);
		ce = dom_get_nodelist_ce(modern);
	}

	object_init_ex(return_value, ce);
}
/* }}} */

/* {{{ php_dom_create_object */
PHP_DOM_EXPORT bool php_dom_create_object(xmlNodePtr obj, zval *return_value, dom_object *domobj)
{
	dom_object *intern = php_dom_object_get_data(obj);
	if (intern) {
		ZVAL_OBJ_COPY(return_value, &intern->std);
		return true;
	}

	bool modern = domobj && php_dom_follow_spec_intern(domobj);

	zend_class_entry *ce;
	switch (obj->type) {
		case XML_DOCUMENT_NODE:
		{
			ce = dom_get_xml_document_ce(modern);
			break;
		}
		case XML_HTML_DOCUMENT_NODE:
		{
			ce = dom_get_html_document_ce(modern);
			break;
		}
		case XML_DTD_NODE:
		case XML_DOCUMENT_TYPE_NODE:
		{
			ce = dom_get_documenttype_ce(modern);
			break;
		}
		case XML_ELEMENT_NODE:
		{
			ce = dom_get_element_ce(modern);
			break;
		}
		case XML_ATTRIBUTE_NODE:
		{
			ce = dom_get_attr_ce(modern);
			break;
		}
		case XML_TEXT_NODE:
		{
			ce = dom_get_text_ce(modern);
			break;
		}
		case XML_COMMENT_NODE:
		{
			ce = dom_get_comment_ce(modern);
			break;
		}
		case XML_PI_NODE:
		{
			ce = dom_get_processinginstruction_ce(modern);
			break;
		}
		case XML_ENTITY_REF_NODE:
		{
			ce = dom_get_entityreference_ce(modern);
			break;
		}
		case XML_ENTITY_DECL:
		case XML_ELEMENT_DECL:
		{
			ce = dom_get_entity_ce(modern);
			break;
		}
		case XML_CDATA_SECTION_NODE:
		{
			ce = dom_get_cdatasection_ce(modern);
			break;
		}
		case XML_DOCUMENT_FRAG_NODE:
		{
			ce = dom_get_documentfragment_ce(modern);
			break;
		}
		case XML_NOTATION_NODE:
		{
			ce = dom_get_notation_ce(modern);
			break;
		}
		case XML_NAMESPACE_DECL:
		{
			/* This has no modern equivalent */
			ce = dom_namespace_node_class_entry;
			break;
		}
		default:
			/* TODO you can actually hit this with fixed attributes in the DTD for example... */
			zend_throw_error(NULL, "Unsupported node type: %d", obj->type);
			ZVAL_NULL(return_value);
			return false;
	}

	if (domobj && domobj->document) {
		ce = dom_get_doc_classmap(domobj->document, ce);
	}
	php_dom_instantiate_object_helper(return_value, ce, obj, domobj);
	return false;
}
/* }}} end php_domobject_new */

dom_object *php_dom_instantiate_object_helper(zval *return_value, zend_class_entry *ce, xmlNodePtr obj, dom_object *parent)
{
	object_init_ex(return_value, ce);

	dom_object *intern = Z_DOMOBJ_P(return_value);
	if (obj->doc != NULL) {
		if (parent != NULL) {
			intern->document = parent->document;
		}
		php_libxml_increment_doc_ref((php_libxml_node_object *)intern, obj->doc);
	}

	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, obj, (void *)intern);

	return intern;
}

void php_dom_create_implementation(zval *retval, bool modern) {
	object_init_ex(retval, dom_get_domimplementation_ce(modern));
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

bool dom_match_qualified_name_according_to_spec(const xmlChar *qname, const xmlNode *nodep)
{
	const xmlChar *node_local = nodep->name;

	/* The qualified name must be matched, which means either:
	 *  - The local parts are equal and there is no ns prefix for this element (i.e. the fqn is in the local name).
	 *  - There is a prefix, the prefixes are equal and the local parts are equal. */
	if (nodep->ns != NULL && nodep->ns->prefix != NULL) {
		const char *prefix = (const char *) nodep->ns->prefix;
		/* 1. match prefix up to |prefix| characters.
		 *    This won't overflow as it'll stop at the '\0' if the lengths don't match. */
		size_t prefix_len = strlen(prefix);
		if (strncmp((const char *) qname, prefix, prefix_len) != 0) {
			return false;
		}
		/* 2. match ':' */
		if (qname[prefix_len] != ':') {
			return false;
		}
		/* 3. match local name */
		return xmlStrEqual(qname + prefix_len + 1, node_local);
	} else {
		return xmlStrEqual(node_local, qname);
	}
}

static bool dom_match_qualified_name_for_tag_name_equality(const xmlChar *local, const xmlChar *local_lower, const xmlNode *nodep, bool match_qname)
{
	if (!match_qname) {
		return xmlStrEqual(nodep->name, local);
	}

	const xmlChar *local_to_use = nodep->doc->type == XML_HTML_DOCUMENT_NODE && php_dom_ns_is_fast(nodep, php_dom_ns_is_html_magic_token) ? local_lower : local;
	return dom_match_qualified_name_according_to_spec(local_to_use, nodep);
}

xmlNode *dom_get_elements_by_tag_name_ns_raw(xmlNodePtr basep, xmlNodePtr nodep, xmlChar *ns, xmlChar *local, xmlChar *local_lower, int *cur, int index) /* {{{ */
{
	/* Can happen with detached document */
	if (UNEXPECTED(nodep == NULL)) {
		return NULL;
	}

	xmlNodePtr ret = NULL;
	bool local_match_any = local[0] == '*' && local[1] == '\0';

	/* Note: The spec says that ns == '' must be transformed to ns == NULL. In other words, they are equivalent.
	 *       PHP however does not do this and internally uses the empty string everywhere when the user provides ns == NULL.
	 *       This is because for PHP ns == NULL has another meaning: "match every namespace" instead of "match the empty namespace". */
	bool ns_match_any = ns == NULL || (ns[0] == '*' && ns[1] == '\0');

	bool match_qname = ns == NULL && php_dom_follow_spec_node(basep);

	while (*cur <= index) {
		if (nodep->type == XML_ELEMENT_NODE) {
			if (local_match_any || dom_match_qualified_name_for_tag_name_equality(local, local_lower, nodep, match_qname)) {
				if (ns_match_any || (ns[0] == '\0' && nodep->ns == NULL) || (nodep->ns != NULL && xmlStrEqual(nodep->ns->href, ns))) {
					if (*cur == index) {
						ret = nodep;
						break;
					}
					(*cur)++;
				}
			}

			if (nodep->children) {
				nodep = nodep->children;
				continue;
			}
		}

		nodep = php_dom_next_in_tree_order(nodep, basep);
		if (!nodep) {
			return NULL;
		}
	}
	return ret;
}
/* }}} end dom_element_get_elements_by_tag_name_ns_raw */

static inline bool is_empty_node(xmlNodePtr nodep)
{
	return nodep->content == NULL || *nodep->content == '\0';
}

static inline void free_node(xmlNodePtr node)
{
	if (node->_private == NULL) {
		xmlFreeNode(node);
	}
}

static void dom_merge_adjacent_exclusive_text_nodes(xmlNodePtr node)
{
	xmlNodePtr nextp = node->next;
	while (nextp != NULL && nextp->type == XML_TEXT_NODE) {
		xmlNodePtr newnextp = nextp->next;
		xmlChar *strContent = nextp->content;
		if (strContent != NULL) {
			xmlNodeAddContent(node, strContent);
		}
		xmlUnlinkNode(nextp);
		free_node(nextp);
		nextp = newnextp;
	}
}

/* {{{ void php_dom_normalize_legacy(xmlNodePtr nodep) */
void php_dom_normalize_legacy(xmlNodePtr nodep)
{
	xmlNodePtr child = nodep->children;
	while(child != NULL) {
		switch (child->type) {
			case XML_TEXT_NODE:
				dom_merge_adjacent_exclusive_text_nodes(child);
				if (is_empty_node(child)) {
					xmlNodePtr nextp = child->next;
					xmlUnlinkNode(child);
					free_node(child);
					child = nextp;
					continue;
				}
				break;
			case XML_ELEMENT_NODE:
				php_dom_normalize_legacy(child);
				xmlAttrPtr attr = child->properties;
				while (attr != NULL) {
					php_dom_normalize_legacy((xmlNodePtr) attr);
					attr = attr->next;
				}
				break;
			default:
				break;
		}
		child = child->next;
	}
}
/* }}} end php_dom_normalize_legacy */

/* https://dom.spec.whatwg.org/#dom-node-normalize */
void php_dom_normalize_modern(xmlNodePtr this)
{
	/* for each descendant exclusive Text node node of this: */
	xmlNodePtr node = this->children;
	while (node != NULL) {
		if (node->type == XML_TEXT_NODE) {
			/* 1. Let length be node’s length.
			 *    We'll deviate a bit here: we'll just check if it's empty or not as we don't want to compute the length. */
			bool is_empty = is_empty_node(node);

			/* 2. If length is zero, then remove node and continue with the next exclusive Text node, if any. */
			if (is_empty) {
				xmlNodePtr next = node->next;
				xmlUnlinkNode(node);
				free_node(node);
				node = next;
				continue;
			}

			/* 3. Let data be the concatenation of the data of node’s contiguous exclusive Text nodes (excluding itself), in tree order.
			 * 4. Replace data with node node, offset length, count 0, and data data.
			 * 7. Remove node’s contiguous exclusive Text nodes (excluding itself), in tree order. 
			 *    => In other words: Concat every contiguous text node into node and delete the merged nodes. */
			dom_merge_adjacent_exclusive_text_nodes(node);

			/* Steps 5-6 deal with mutation records, we don't do that here. */
		} else if (node->type == XML_ELEMENT_NODE) {
			php_dom_normalize_modern(node);
		}
		node = node->next;
	}
}

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
					/* Note: we can't get here if the ns is already on the oldNs list.
					 * This is because in that case the definition won't be on the node, and
					 * therefore won't be in the nodep->nsDef list. */
					php_libxml_set_old_ns(doc, curns);
					curns = prevns;
				}
			}
			prevns = curns;
			curns = nsdftptr;
		}
	}
}

void dom_reconcile_ns(xmlDocPtr doc, xmlNodePtr nodep) /* {{{ */
{
	ZEND_ASSERT(nodep->type != XML_ATTRIBUTE_NODE);

	/* Although the node type will be checked by the libxml2 API,
	 * we still want to do the internal reconciliation conditionally. */
	if (nodep->type == XML_ELEMENT_NODE) {
		dom_reconcile_ns_internal(doc, nodep, nodep->parent);
		xmlReconciliateNs(doc, nodep);
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
		xmlReconciliateNs(doc, nodep);
		if (nodep == last) {
			break;
		}
		nodep = nodep->next;
	}
}

/* https://dom.spec.whatwg.org/#validate-and-extract */
int dom_validate_and_extract(const zend_string *namespace, const zend_string *qname, xmlChar **localName, xmlChar **prefix)
{
	/* 1. If namespace is the empty string, then set it to null.
	 *    However, we're going to cheat and do the opposite to make
	 *    implementation of the below steps with existing zend_ helpers easier. */
	if (namespace == NULL) {
		namespace = zend_empty_string;
	}

	/* 2. Validate qualifiedName. */
	if (xmlValidateQName(BAD_CAST ZSTR_VAL(qname), /* allow spaces */ 0) != 0) {
		return INVALID_CHARACTER_ERR;
	}

	/* Steps 3-5 */
	*localName = xmlSplitQName2(BAD_CAST ZSTR_VAL(qname), prefix);

	/* 6. If prefix is non-null and namespace is null, then throw a "NamespaceError" DOMException.
	 *    Note that null namespace means empty string here becaue of step 1. */
	if (*prefix != NULL && ZSTR_VAL(namespace)[0] == '\0') {
		return NAMESPACE_ERR;
	}

	/* 7. If prefix is "xml" and namespace is not the XML namespace, then throw a "NamespaceError" DOMException. */
	if (UNEXPECTED(!zend_string_equals_literal(namespace, "http://www.w3.org/XML/1998/namespace") && xmlStrEqual(*prefix, BAD_CAST "xml"))) {
		return NAMESPACE_ERR;
	}

	/* 8. If either qualifiedName or prefix is "xmlns" and namespace is not the XMLNS namespace, then throw a "NamespaceError" DOMException. */
	if (UNEXPECTED((zend_string_equals_literal(qname, "xmlns") || xmlStrEqual(*prefix, BAD_CAST "xmlns")) && !zend_string_equals_literal(namespace, "http://www.w3.org/2000/xmlns/"))) {
		return NAMESPACE_ERR;
	}

	/* 9. If namespace is the XMLNS namespace and neither qualifiedName nor prefix is "xmlns", then throw a "NamespaceError" DOMException. */
	if (UNEXPECTED(zend_string_equals_literal(namespace, "http://www.w3.org/2000/xmlns/") && !zend_string_equals_literal(qname, "xmlns") && !xmlStrEqual(*prefix, BAD_CAST "xmlns"))) {
		return NAMESPACE_ERR;
	}

	if (*localName == NULL) {
		*localName = xmlStrdup(BAD_CAST ZSTR_VAL(qname));
	}

	return 0;
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

	*localname = (char *)xmlSplitQName2(BAD_CAST qname, (xmlChar **) prefix);
	if (*localname == NULL) {
		*localname = (char *)xmlStrdup(BAD_CAST qname);
		if (*prefix == NULL && uri_len == 0) {
			return 0;
		}
	}

	/* 1 */
	if (xmlValidateQName(BAD_CAST qname, 0) != 0) {
		return NAMESPACE_ERR;
	}

	/* 2 */
	if (*prefix != NULL && uri_len == 0) {
		return NAMESPACE_ERR;
	}

	return 0;
}
/* }}} */

/* Creates a new namespace declaration with a random prefix with the given uri on the tree.
 * This is used to resolve a namespace prefix conflict in cases where spec does not want a
 * namespace error in case of conflicts, but demands a resolution. */
xmlNsPtr dom_get_ns_resolve_prefix_conflict(xmlNodePtr tree, const char *uri)
{
	ZEND_ASSERT(tree != NULL);
	xmlDocPtr doc = tree->doc;

	if (UNEXPECTED(doc == NULL)) {
		return NULL;
	}

	/* Code adapted from libxml2 (2.10.4) */
	char prefix[50];
	int counter = 1;
	snprintf(prefix, sizeof(prefix), "default");
	xmlNsPtr nsptr = xmlSearchNs(doc, tree, (const xmlChar *) prefix);
	while (nsptr != NULL) {
		if (counter > 1000) {
			return NULL;
		}
		snprintf(prefix, sizeof(prefix), "default%d", counter++);
		nsptr = xmlSearchNs(doc, tree, (const xmlChar *) prefix);
	}

	/* Search yielded no conflict */
	return xmlNewNs(tree, (const xmlChar *) uri, (const xmlChar *) prefix);
}

/*
http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-DocCrElNS

NAMESPACE_ERR: Raised if

3. the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace" [XML Namespaces]
4. the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from  "http://www.w3.org/2000/xmlns/"
5. the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the	qualifiedName nor its prefix is "xmlns".
*/

xmlNsPtr dom_get_ns_unchecked(xmlNodePtr nodep, char *uri, char *prefix)
{
	xmlNsPtr nsptr = xmlNewNs(nodep, BAD_CAST uri, BAD_CAST prefix);
	if (UNEXPECTED(nsptr == NULL)) {
		/* Either memory allocation failure, or it's because of a prefix conflict.
		 * We'll assume a conflict and try again. If it was a memory allocation failure we'll just fail again, whatever.
		 * This isn't needed for every caller (such as createElementNS & DOMElement::__construct), but isn't harmful and simplifies the mental model "when do I use which function?".
		 * This branch will also be taken unlikely anyway as in those cases it'll be for allocation failure. */
		return dom_get_ns_resolve_prefix_conflict(nodep, uri);
	}

	return nsptr;
}

/* {{{ xmlNsPtr dom_get_ns(xmlNodePtr nodep, char *uri, int *errorcode, char *prefix) */
xmlNsPtr dom_get_ns(xmlNodePtr nodep, char *uri, int *errorcode, char *prefix) {
	xmlNsPtr nsptr;

	if (! ((prefix && !strcmp (prefix, "xml") && strcmp(uri, (char *)XML_XML_NAMESPACE)) ||
		   (prefix && !strcmp (prefix, "xmlns") && strcmp(uri, DOM_XMLNS_NS_URI)) ||
		   (prefix && !strcmp(uri, DOM_XMLNS_NS_URI) && strcmp (prefix, "xmlns")))) {
		nsptr = dom_get_ns_unchecked(nodep, uri, prefix);
		if (UNEXPECTED(nsptr == NULL)) {
			goto err;
		}
	} else {
		goto err;
	}

	*errorcode = 0;
	return nsptr;
err:
	*errorcode = NAMESPACE_ERR;
	return NULL;
}
/* }}} end dom_get_ns */

/* {{{ xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName) */
xmlNsPtr dom_get_nsdecl(xmlNode *node, xmlChar *localName) {
	xmlNsPtr cur;
	xmlNs *ret = NULL;
	if (node == NULL)
		return NULL;

	if (localName == NULL || localName[0] == '\0') {
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
		attrp = xmlNewDocNode(nodep->doc, NULL, BAD_CAST original->prefix, original->href);
	} else {
		attrp = xmlNewDocNode(nodep->doc, NULL, BAD_CAST "xmlns", original->href);
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

static zval *dom_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot access %s without offset", ZSTR_VAL(object->ce->name));
		return NULL;
	}

	ZVAL_DEREF(offset);

	zend_long lval;
	if (dom_nodemap_or_nodelist_process_offset_as_named(offset, &lval)) {
		/* does not support named lookup */
		ZVAL_NULL(rv);
		return rv;
	}

	php_dom_nodelist_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, lval, rv);
	return rv;
}

static int dom_nodelist_has_dimension(zend_object *object, zval *member, int check_empty)
{
	ZVAL_DEREF(member);

	/* If it exists, it cannot be empty because nodes aren't empty. */
	ZEND_IGNORE_VALUE(check_empty);

	zend_long offset;
	if (dom_nodemap_or_nodelist_process_offset_as_named(member, &offset)) {
		/* does not support named lookup */
		return 0;
	}

	return offset >= 0 && offset < php_dom_get_nodelist_length(php_dom_obj_from_obj(object));
}

static zend_long dom_modern_nodelist_get_index(zval *offset, bool *failed)
{
	zend_ulong lval;
	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) == IS_LONG) {
		*failed = false;
		return Z_LVAL_P(offset);
	} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
		*failed = false;
		return zend_dval_to_lval_safe(Z_DVAL_P(offset));
	} else if (Z_TYPE_P(offset) == IS_STRING && ZEND_HANDLE_NUMERIC(Z_STR_P(offset), lval)) {
		*failed = false;
		return (zend_long) lval;
	} else {
		*failed = true;
		return 0;
	}
}

static zval *dom_modern_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot append to %s", ZSTR_VAL(object->ce->name));
		return NULL;
	}

	bool failed;
	zend_long lval = dom_modern_nodelist_get_index(offset, &failed);
	if (UNEXPECTED(failed)) {
		zend_illegal_container_offset(object->ce->name, offset, type);
		return NULL;
	}

	php_dom_nodelist_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, lval, rv);
	return rv;
}

static int dom_modern_nodelist_has_dimension(zend_object *object, zval *member, int check_empty)
{
	/* If it exists, it cannot be empty because nodes aren't empty. */
	ZEND_IGNORE_VALUE(check_empty);

	bool failed;
	zend_long lval = dom_modern_nodelist_get_index(member, &failed);
	if (UNEXPECTED(failed)) {
		zend_illegal_container_offset(object->ce->name, member, BP_VAR_IS);
		return 0;
	}

	return lval >= 0 && lval < php_dom_get_nodelist_length(php_dom_obj_from_obj(object));
}

void dom_remove_all_children(xmlNodePtr nodep)
{
	if (nodep->children) {
		node_list_unlink(nodep->children);
		php_libxml_node_free_list((xmlNodePtr) nodep->children);
		nodep->children = NULL;
		nodep->last = NULL;
	}
}

void php_dom_get_content_into_zval(const xmlNode *nodep, zval *retval, bool default_is_null)
{
	ZEND_ASSERT(nodep != NULL);

	if (nodep->type == XML_TEXT_NODE
		|| nodep->type == XML_CDATA_SECTION_NODE
		|| nodep->type == XML_PI_NODE
		|| nodep->type == XML_COMMENT_NODE) {
		char *str = (char * ) nodep->content;
		if (str != NULL) {
			ZVAL_STRING(retval, str);
		} else {
			goto failure;
		}
		return;
	}

	char *str = (char *) xmlNodeGetContent(nodep);

	if (str != NULL) {
		ZVAL_STRING(retval, str);
		xmlFree(str);
		return;
	}

failure:
	if (default_is_null) {
		ZVAL_NULL(retval);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}
}

static zval *dom_nodemap_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot access %s without offset", ZSTR_VAL(object->ce->name));
		return NULL;
	}

	ZVAL_DEREF(offset);

	zend_long lval;
	if (dom_nodemap_or_nodelist_process_offset_as_named(offset, &lval)) {
		/* exceptional case, switch to named lookup */
		php_dom_named_node_map_get_named_item_into_zval(php_dom_obj_from_obj(object)->ptr, Z_STR_P(offset), rv);
		return rv;
	}

	/* see PHP_METHOD(DOMNamedNodeMap, item) */
	if (UNEXPECTED(lval < 0 || ZEND_LONG_INT_OVFL(lval))) {
		zend_value_error("must be between 0 and %d", INT_MAX);
		return NULL;
	}

	php_dom_named_node_map_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, lval, rv);
	return rv;
}

static int dom_nodemap_has_dimension(zend_object *object, zval *member, int check_empty)
{
	ZVAL_DEREF(member);

	/* If it exists, it cannot be empty because nodes aren't empty. */
	ZEND_IGNORE_VALUE(check_empty);

	zend_long offset;
	if (dom_nodemap_or_nodelist_process_offset_as_named(member, &offset)) {
		/* exceptional case, switch to named lookup */
		return php_dom_named_node_map_get_named_item(php_dom_obj_from_obj(object)->ptr, Z_STR_P(member), false) != NULL;
	}

	return offset >= 0 && offset < php_dom_get_namednodemap_length(php_dom_obj_from_obj(object));
}

static zval *dom_modern_nodemap_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot append to %s", ZSTR_VAL(object->ce->name));
		return NULL;
	}

	dom_nnodemap_object *map = php_dom_obj_from_obj(object)->ptr;

	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) == IS_STRING) {
		zend_ulong lval;
		if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), lval)) {
			php_dom_named_node_map_get_item_into_zval(map, (zend_long) lval, rv);
		} else {
			php_dom_named_node_map_get_named_item_into_zval(map, Z_STR_P(offset), rv);
		}
	} else if (Z_TYPE_P(offset) == IS_LONG) {
		php_dom_named_node_map_get_item_into_zval(map, Z_LVAL_P(offset), rv);
	} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
		php_dom_named_node_map_get_item_into_zval(map, zend_dval_to_lval_safe(Z_DVAL_P(offset)), rv);
	} else {
		zend_illegal_container_offset(object->ce->name, offset, type);
		return NULL;
	}

	return rv;
}

static int dom_modern_nodemap_has_dimension(zend_object *object, zval *member, int check_empty)
{
	/* If it exists, it cannot be empty because nodes aren't empty. */
	ZEND_IGNORE_VALUE(check_empty);

	dom_object *obj = php_dom_obj_from_obj(object);
	dom_nnodemap_object *map = obj->ptr;

	ZVAL_DEREF(member);
	if (Z_TYPE_P(member) == IS_STRING) {
		zend_ulong lval;
		if (ZEND_HANDLE_NUMERIC(Z_STR_P(member), lval)) {
			return (zend_long) lval >= 0 && (zend_long) lval < php_dom_get_namednodemap_length(obj);
		} else {
			return php_dom_named_node_map_get_named_item(map, Z_STR_P(member), false) != NULL;
		}
	} else if (Z_TYPE_P(member) == IS_LONG) {
		zend_long offset = Z_LVAL_P(member);
		return offset >= 0 && offset < php_dom_get_namednodemap_length(obj);
	} else if (Z_TYPE_P(member) == IS_DOUBLE) {
		zend_long offset = zend_dval_to_lval_safe(Z_DVAL_P(member));
		return offset >= 0 && offset < php_dom_get_namednodemap_length(obj);
	} else {
		zend_illegal_container_offset(object->ce->name, member, BP_VAR_IS);
		return 0;
	}
}

static xmlNodePtr dom_clone_container_helper(php_dom_libxml_ns_mapper *ns_mapper, xmlNodePtr src_node, xmlDocPtr dst_doc)
{
	xmlNodePtr clone = xmlDocCopyNode(src_node, dst_doc, 0);
	if (EXPECTED(clone != NULL)) {
		/* Set namespace to the original, reconciliation will fix this up. */
		clone->ns = src_node->ns;

		if (src_node->type == XML_ELEMENT_NODE) {
			xmlAttrPtr last_added_attr = NULL;

			if (src_node->nsDef != NULL) {
				xmlNsPtr current_ns = src_node->nsDef;
				do {
					php_dom_ns_compat_mark_attribute(ns_mapper, clone, current_ns);
				} while ((current_ns = current_ns->next) != NULL);

				last_added_attr = clone->properties;
				while (last_added_attr->next != NULL) {
					last_added_attr = last_added_attr->next;
				}
			}

			/* Attribute cloning logic. */
			for (xmlAttrPtr attr = src_node->properties; attr != NULL; attr = attr->next) {
				xmlAttrPtr new_attr = (xmlAttrPtr) xmlDocCopyNode((xmlNodePtr) attr, dst_doc, 0);
				if (UNEXPECTED(new_attr == NULL)) {
					xmlFreeNode(clone);
					return NULL;
				}
				if (last_added_attr == NULL) {
					clone->properties = new_attr;
				} else {
					new_attr->prev = last_added_attr;
					last_added_attr->next = new_attr;
				}
				new_attr->parent = clone;
				last_added_attr = new_attr;

				/* Set namespace to the original, reconciliation will fix this up. */
				new_attr->ns = attr->ns;
			}
		}
	}
	return clone;
}

static xmlNodePtr dom_clone_helper(php_dom_libxml_ns_mapper *ns_mapper, xmlNodePtr src_node, xmlDocPtr dst_doc, bool recursive)
{
	xmlNodePtr outer_clone = dom_clone_container_helper(ns_mapper, src_node, dst_doc);

	if (!recursive || (src_node->type != XML_ELEMENT_NODE && src_node->type != XML_DOCUMENT_FRAG_NODE && src_node->type != XML_DOCUMENT_NODE && src_node->type != XML_HTML_DOCUMENT_NODE)) {
		return outer_clone;
	}

	/* Handle dtd separately, because it is linked twice and requires a special copy function. */
	if (src_node->type == XML_DOCUMENT_NODE || src_node->type == XML_HTML_DOCUMENT_NODE) {
		dst_doc = (xmlDocPtr) outer_clone;

		xmlDtdPtr original_subset = ((xmlDocPtr) src_node)->intSubset;
		if (original_subset != NULL) {
			dst_doc->intSubset = xmlCopyDtd(((xmlDocPtr) src_node)->intSubset);
			if (UNEXPECTED(dst_doc->intSubset == NULL)) {
				xmlFreeNode(outer_clone);
				return NULL;
			}
			dst_doc->intSubset->parent = dst_doc;
			xmlSetTreeDoc((xmlNodePtr) dst_doc->intSubset, dst_doc);
			dst_doc->children = dst_doc->last = (xmlNodePtr) dst_doc->intSubset;
		}
	}

	xmlNodePtr cloned_parent = outer_clone;
	xmlNodePtr base = src_node;
	src_node = src_node->children;
	while (src_node != NULL) {
		ZEND_ASSERT(src_node != base);

		xmlNodePtr cloned;
		if (src_node->type == XML_ELEMENT_NODE) {
			cloned = dom_clone_container_helper(ns_mapper, src_node, dst_doc);
		} else if (src_node->type == XML_DTD_NODE) {
			/* Already handled. */
			cloned = NULL;
		} else {
			cloned = xmlDocCopyNode(src_node, dst_doc, 1);
		}

		if (EXPECTED(cloned != NULL)) {
			if (cloned_parent->children == NULL) {
				cloned_parent->children = cloned;
			} else {
				cloned->prev = cloned_parent->last;
				cloned_parent->last->next = cloned;
			}
			cloned->parent = cloned_parent;
			cloned_parent->last = cloned;
		}

		if (src_node->type == XML_ELEMENT_NODE && src_node->children) {
			cloned_parent = cloned;
			src_node = src_node->children;
		} else if (src_node->next) {
			src_node = src_node->next;
		} else {
			/* Go upwards, until we find a parent node with a next sibling, or until we hit the base. */
			do {
				src_node = src_node->parent;
				if (src_node == base) {
					return outer_clone;
				}
				cloned_parent = cloned_parent->parent;
			} while (src_node->next == NULL);
			src_node = src_node->next;
		}
	}

	return outer_clone;
}

xmlNodePtr dom_clone_node(php_dom_libxml_ns_mapper *ns_mapper, xmlNodePtr node, xmlDocPtr doc, bool recursive)
{
	if (node->type == XML_DTD_NODE) {
		/* The behaviour w.r.t. the internal subset is implementation-defined according to DOM 3.
		 * This follows what e.g. Java and C# do: copy it regardless of the recursiveness.
		 * Makes sense as the subset is not exactly a child in the normal sense. */
		xmlDtdPtr dtd = xmlCopyDtd((xmlDtdPtr) node);
		xmlSetTreeDoc((xmlNodePtr) dtd, doc);
		return (xmlNodePtr) dtd;
	}

	if (ns_mapper != NULL) {
		xmlNodePtr clone = dom_clone_helper(ns_mapper, node, doc, recursive);
		if (EXPECTED(clone != NULL)) {
			if (clone->type == XML_DOCUMENT_NODE || clone->type == XML_HTML_DOCUMENT_NODE || clone->type == XML_DOCUMENT_FRAG_NODE) {
				for (xmlNodePtr child = clone->children; child != NULL; child = child->next) {
					php_dom_libxml_reconcile_modern(ns_mapper, child);
				}
			} else {
				php_dom_libxml_reconcile_modern(ns_mapper, clone);
			}
		}
		return clone;
	} else {
		/* See http://www.xmlsoft.org/html/libxml-tree.html#xmlDocCopyNode for meaning of values */
		int extended_recursive = recursive;
		if (!recursive && node->type == XML_ELEMENT_NODE) {
			extended_recursive = 2;
		}
		return xmlDocCopyNode(node, doc, extended_recursive);
	}
}

bool php_dom_has_child_of_type(xmlNodePtr node, xmlElementType type)
{
	xmlNodePtr child = node->children;

	while (child != NULL) {
		if (child->type == type) {
			return true;
		}

		child = child->next;
	}

	return false;
}

bool php_dom_has_sibling_following_node(xmlNodePtr node, xmlElementType type)
{
	xmlNodePtr next = node->next;

	while (next != NULL) {
		if (next->type == type) {
			return true;
		}

		next = next->next;
	}

	return false;
}

bool php_dom_has_sibling_preceding_node(xmlNodePtr node, xmlElementType type)
{
	xmlNodePtr prev = node->prev;

	while (prev != NULL) {
		if (prev->type == type) {
			return true;
		}

		prev = prev->prev;
	}

	return false;
}

xmlAttrPtr php_dom_get_attribute_node(xmlNodePtr elem, const xmlChar *name, size_t name_len)
{
	xmlChar *name_processed = BAD_CAST name;
	if (php_dom_ns_is_html_and_document_is_html(elem)) {
		char *lowercase_copy = zend_str_tolower_dup_ex((char *) name, name_len);
		if (lowercase_copy != NULL) {
			name_processed = BAD_CAST lowercase_copy;
		}
	}

	xmlAttrPtr ret = NULL;
	for (xmlAttrPtr attr = elem->properties; attr != NULL; attr = attr->next) {
		if (dom_match_qualified_name_according_to_spec(name_processed, (xmlNodePtr) attr)) {
			ret = attr;
			break;
		}
	}

	if (name_processed != name) {
		efree(name_processed);
	}

	return ret;
}

/* Workaround for a libxml2 bug on Windows: https://gitlab.gnome.org/GNOME/libxml2/-/issues/611. */
xmlChar *php_dom_libxml_fix_file_path(xmlChar *path)
{
	if (strncmp((char *) path, "file:/", sizeof("file:/") - 1) == 0) {
		if (path[6] != '/' && path[6] != '\0' && path[7] != '/' && path[7] != '\0') {
			/* The path is file:/xx... where xx != "//", which is broken */
			xmlChar *new_path = xmlStrdup(BAD_CAST "file:///");
			if (UNEXPECTED(new_path == NULL)) {
				return path;
			}
			new_path = xmlStrcat(new_path, path + 6);
			xmlFree(path);
			return new_path;
		}
	}
	return path;
}

xmlDocPtr php_dom_create_html_doc(void)
{
#ifdef LIBXML_HTML_ENABLED
	xmlDocPtr lxml_doc = htmlNewDocNoDtD(NULL, NULL);
	if (EXPECTED(lxml_doc)) {
		lxml_doc->dict = xmlDictCreate();
	}
#else
	/* If HTML support is not enabled, then htmlNewDocNoDtD() is not available.
	 * This code mimics the behaviour. */
	xmlDocPtr lxml_doc = xmlNewDoc((const xmlChar *) "1.0");
	if (EXPECTED(lxml_doc)) {
		lxml_doc->type = XML_HTML_DOCUMENT_NODE;
		lxml_doc->dict = xmlDictCreate();
	}
#endif
	return lxml_doc;
}

#endif /* HAVE_DOM */
