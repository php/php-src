/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
   |          Rob Richards <rrichards@php.net>                            |
   |          Marcus Borger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "ext/standard/php_rand.h"
#include "php_dom.h"
#include "dom_properties.h"
#include "zend_interfaces.h"

#include "ext/standard/info.h"
#define PHP_XPATH 1
#define PHP_XPTR 2

/* {{{ class entries */
zend_class_entry *dom_node_class_entry;
zend_class_entry *dom_domexception_class_entry;
zend_class_entry *dom_domstringlist_class_entry;
zend_class_entry *dom_namelist_class_entry;
zend_class_entry *dom_domimplementationlist_class_entry;
zend_class_entry *dom_domimplementationsource_class_entry;
zend_class_entry *dom_domimplementation_class_entry;
zend_class_entry *dom_documentfragment_class_entry;
zend_class_entry *dom_document_class_entry;
zend_class_entry *dom_nodelist_class_entry;
zend_class_entry *dom_namednodemap_class_entry;
zend_class_entry *dom_characterdata_class_entry;
zend_class_entry *dom_attr_class_entry;
zend_class_entry *dom_element_class_entry;
zend_class_entry *dom_text_class_entry;
zend_class_entry *dom_comment_class_entry;
zend_class_entry *dom_typeinfo_class_entry;
zend_class_entry *dom_userdatahandler_class_entry;
zend_class_entry *dom_domerror_class_entry;
zend_class_entry *dom_domerrorhandler_class_entry;
zend_class_entry *dom_domlocator_class_entry;
zend_class_entry *dom_domconfiguration_class_entry;
zend_class_entry *dom_cdatasection_class_entry;
zend_class_entry *dom_documenttype_class_entry;
zend_class_entry *dom_notation_class_entry;
zend_class_entry *dom_entity_class_entry;
zend_class_entry *dom_entityreference_class_entry;
zend_class_entry *dom_processinginstruction_class_entry;
zend_class_entry *dom_string_extend_class_entry;
#if defined(LIBXML_XPATH_ENABLED)
zend_class_entry *dom_xpath_class_entry;
#endif
zend_class_entry *dom_namespace_node_class_entry;
/* }}} */

zend_object_handlers dom_object_handlers;
zend_object_handlers dom_nnodemap_object_handlers;
#if defined(LIBXML_XPATH_ENABLED)
zend_object_handlers dom_xpath_object_handlers;
#endif

static HashTable classes;
/* {{{ prop handler tables */
static HashTable dom_domstringlist_prop_handlers;
static HashTable dom_namelist_prop_handlers;
static HashTable dom_domimplementationlist_prop_handlers;
static HashTable dom_document_prop_handlers;
static HashTable dom_node_prop_handlers;
static HashTable dom_nodelist_prop_handlers;
static HashTable dom_namednodemap_prop_handlers;
static HashTable dom_characterdata_prop_handlers;
static HashTable dom_attr_prop_handlers;
static HashTable dom_element_prop_handlers;
static HashTable dom_text_prop_handlers;
static HashTable dom_typeinfo_prop_handlers;
static HashTable dom_domerror_prop_handlers;
static HashTable dom_domlocator_prop_handlers;
static HashTable dom_documenttype_prop_handlers;
static HashTable dom_notation_prop_handlers;
static HashTable dom_entity_prop_handlers;
static HashTable dom_processinginstruction_prop_handlers;
static HashTable dom_namespace_node_prop_handlers;
#if defined(LIBXML_XPATH_ENABLED)
static HashTable dom_xpath_prop_handlers;
#endif
/* }}} */

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

int dom_set_doc_classmap(php_libxml_ref_obj *document, zend_class_entry *basece, zend_class_entry *ce)
{
	dom_doc_propsptr doc_props;

	if (document) {
		doc_props = dom_get_doc_props(document);
		if (doc_props->classmap == NULL) {
			if (ce == NULL) {
				return SUCCESS;
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
	return SUCCESS;
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

/* {{{ dom_read_na */
static int dom_read_na(dom_object *obj, zval *retval)
{
	php_error_docref(NULL, E_ERROR, "Cannot read property");
	return FAILURE;
}
/* }}} */

/* {{{ dom_write_na */
static int dom_write_na(dom_object *obj, zval *newval)
{
	php_error_docref(NULL, E_ERROR, "Cannot write property");
	return FAILURE;
}
/* }}} */

/* {{{ dom_register_prop_handler */
static void dom_register_prop_handler(HashTable *prop_handler, char *name, dom_read_t read_func, dom_write_t write_func)
{
	dom_prop_handler hnd;

	hnd.read_func = read_func ? read_func : dom_read_na;
	hnd.write_func = write_func ? write_func : dom_write_na;
	zend_hash_str_add_mem(prop_handler, name, strlen(name), &hnd, sizeof(dom_prop_handler));
}
/* }}} */

static zval *dom_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot) /* {{{ */
{
	dom_object *obj = Z_DOMOBJ_P(object);
	zend_string *member_str = zval_get_string(member);
	zval *retval = NULL;

	if (!obj->prop_handler || !zend_hash_exists(obj->prop_handler, member_str)) {
		zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->get_property_ptr_ptr(object, member, type, cache_slot);
	}

	zend_string_release(member_str);
	return retval;
}
/* }}} */

/* {{{ dom_read_property */
zval *dom_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
	dom_object *obj = Z_DOMOBJ_P(object);
	zend_string *member_str = zval_get_string(member);
	zval *retval;
	dom_prop_handler *hnd = NULL;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, member_str);
	} else if (instanceof_function(obj->std.ce, dom_node_class_entry)) {
		php_error(E_WARNING, "Couldn't fetch %s. Node no longer exists", obj->std.ce->name->val);
	}

	if (hnd) {
		int ret = hnd->read_func(obj, rv);
		if (ret == SUCCESS) {
			retval = rv;
		} else {
			retval = &EG(uninitialized_zval);
		}
	} else {
		zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type, cache_slot, rv);
	}

	zend_string_release(member_str);
	return retval;
}
/* }}} */

/* {{{ dom_write_property */
void dom_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	dom_object *obj = Z_DOMOBJ_P(object);
	zend_string *member_str = zval_get_string(member);
	dom_prop_handler *hnd = NULL;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, member_str);
	}
	if (hnd) {
		hnd->write_func(obj, value);
	} else {
		zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		std_hnd->write_property(object, member, value, cache_slot);
	}

	zend_string_release(member_str);
}
/* }}} */

/* {{{ dom_property_exists */
static int dom_property_exists(zval *object, zval *member, int check_empty, void **cache_slot)
{
	dom_object *obj = Z_DOMOBJ_P(object);
	zend_string *member_str = zval_get_string(member);
	dom_prop_handler *hnd = NULL;
	int retval = 0;

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, member_str);
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
			zval_dtor(&tmp);
		}
	} else {
		zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->has_property(object, member, check_empty, cache_slot);
	}

	zend_string_release(member_str);
	return retval;
}
/* }}} */

static HashTable* dom_get_debug_info_helper(zval *object, int *is_temp) /* {{{ */
{
	dom_object			*obj = Z_DOMOBJ_P(object);
	HashTable			*debug_info,
						*prop_handlers = obj->prop_handler,
						*std_props;
	HashPosition		pos;
	dom_prop_handler	*entry;
	zval object_value;

	*is_temp = 1;

	ALLOC_HASHTABLE(debug_info);

	std_props = zend_std_get_properties(object);
	zend_array_dup(debug_info, std_props);

	if (!prop_handlers) {
		return debug_info;
	}

	ZVAL_STRING(&object_value, "(object value omitted)");

	for (zend_hash_internal_pointer_reset_ex(prop_handlers, &pos);
			(entry = zend_hash_get_current_data_ptr_ex(prop_handlers, &pos)) != NULL;
			zend_hash_move_forward_ex(prop_handlers, &pos)) {
		zval value;
		zend_string *string_key;
		zend_ulong num_key;

		if (entry->read_func(obj, &value) == FAILURE) {
			continue;
		}

		if (zend_hash_get_current_key_ex(prop_handlers, &string_key,
			&num_key, &pos) != HASH_KEY_IS_STRING) {
			continue;
		}

		if (Z_TYPE(value) == IS_OBJECT) {
			zval_dtor(&value);
			ZVAL_COPY(&value, &object_value);
		}

		zend_hash_add(debug_info, string_key, &value);
	}

	zval_dtor(&object_value);

	return debug_info;
}
/* }}} */

static HashTable* dom_get_debug_info(zval *object, int *is_temp) /* {{{ */
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

/* {{{ proto somNode dom_import_simplexml(sxeobject node)
   Get a simplexml_element object from dom to allow for processing */
PHP_FUNCTION(dom_import_simplexml)
{
	zval *node;
	xmlNodePtr nodep = NULL;
	php_libxml_node_object *nodeobj;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &node) == FAILURE) {
		return;
	}

	nodeobj = (php_libxml_node_object *) ((char *) Z_OBJ_P(node) - Z_OBJ_HT_P(node)->offset);
	nodep = php_libxml_import_node(node);

	if (nodep && nodeobj && (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE)) {
		DOM_RET_OBJ((xmlNodePtr) nodep, &ret, (dom_object *)nodeobj);
	} else {
		php_error_docref(NULL, E_WARNING, "Invalid Nodetype to import");
		RETURN_NULL();
	}
}
/* }}} */

static dom_object* dom_objects_set_class(zend_class_entry *class_type, zend_bool hash_copy);

static zend_object *dom_objects_store_clone_obj(zval *zobject) /* {{{ */
{
	dom_object *intern = Z_DOMOBJ_P(zobject);
	dom_object *clone = dom_objects_set_class(intern->std.ce, 0);

	clone->std.handlers = dom_get_obj_handlers();
	zend_objects_clone_members(&clone->std, &intern->std);

	if (instanceof_function(intern->std.ce, dom_node_class_entry)) {
		xmlNodePtr node = (xmlNodePtr)dom_object_get_node(intern);
		if (node != NULL) {
			xmlNodePtr cloned_node = xmlDocCopyNode(node, node->doc, 1);
			if (cloned_node != NULL) {
				/* If we cloned a document then we must create new doc proxy */
				if (cloned_node->doc == node->doc) {
					clone->document = intern->document;
				}
				php_libxml_increment_doc_ref((php_libxml_node_object *)clone, cloned_node->doc);
				php_libxml_increment_node_ptr((php_libxml_node_object *)clone, cloned_node, (void *)clone);
				if (intern->document != clone->document) {
					dom_copy_doc_props(intern->document, clone->document);
				}
			}

		}
	}

	return &clone->std;
}
/* }}} */

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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_import_simplexml, 0, 0, 1)
	ZEND_ARG_INFO(0, node)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry dom_functions[] = {
	PHP_FE(dom_import_simplexml, arginfo_dom_import_simplexml)
	PHP_FE_END
};

static const zend_module_dep dom_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_CONFLICTS("domxml")
	ZEND_MOD_END
};

zend_module_entry dom_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER_EX, NULL,
	dom_deps,
	"dom",
	dom_functions,
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
static zend_object *dom_objects_store_clone_obj(zval *zobject);
static void dom_nnodemap_object_dtor(zend_object *object);
#if defined(LIBXML_XPATH_ENABLED)
void dom_xpath_objects_free_storage(zend_object *object);
#endif

/* {{{ PHP_MINIT_FUNCTION(dom) */
PHP_MINIT_FUNCTION(dom)
{
	zend_class_entry ce;

	memcpy(&dom_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	dom_object_handlers.offset = XtOffsetOf(dom_object, std);
	dom_object_handlers.free_obj = dom_objects_free_storage;
	dom_object_handlers.clone_obj = dom_objects_store_clone_obj;
	dom_object_handlers.read_property = dom_read_property;
	dom_object_handlers.write_property = dom_write_property;
	dom_object_handlers.get_property_ptr_ptr = dom_get_property_ptr_ptr;
	dom_object_handlers.clone_obj = dom_objects_store_clone_obj;
	dom_object_handlers.has_property = dom_property_exists;
	dom_object_handlers.get_debug_info = dom_get_debug_info;

	memcpy(&dom_nnodemap_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_nnodemap_object_handlers.free_obj = dom_nnodemap_objects_free_storage;
	dom_nnodemap_object_handlers.dtor_obj = dom_nnodemap_object_dtor;
	dom_nnodemap_object_handlers.read_dimension = dom_nodelist_read_dimension;
	dom_nnodemap_object_handlers.has_dimension = dom_nodelist_has_dimension;

	zend_hash_init(&classes, 0, NULL, NULL, 1);

	INIT_CLASS_ENTRY(ce, "DOMException", php_dom_domexception_class_functions);
	dom_domexception_class_entry = zend_register_internal_class_ex(&ce, zend_exception_get_default());
	dom_domexception_class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_declare_property_long(dom_domexception_class_entry, "code", sizeof("code")-1, 0, ZEND_ACC_PUBLIC);

	REGISTER_DOM_CLASS(ce, "DOMStringList", NULL, php_dom_domstringlist_class_functions, dom_domstringlist_class_entry);

	zend_hash_init(&dom_domstringlist_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_domstringlist_prop_handlers, "length", dom_domstringlist_length_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_domstringlist_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMNameList", NULL, php_dom_namelist_class_functions, dom_namelist_class_entry);

	zend_hash_init(&dom_namelist_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_namelist_prop_handlers, "length", dom_namelist_length_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_namelist_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMImplementationList", NULL, php_dom_domimplementationlist_class_functions, dom_domimplementationlist_class_entry);

	zend_hash_init(&dom_domimplementationlist_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_domimplementationlist_prop_handlers, "length", dom_domimplementationlist_length_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_domimplementationlist_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMImplementationSource", NULL, php_dom_domimplementationsource_class_functions, dom_domimplementationsource_class_entry);
	REGISTER_DOM_CLASS(ce, "DOMImplementation", NULL, php_dom_domimplementation_class_functions, dom_domimplementation_class_entry);

	REGISTER_DOM_CLASS(ce, "DOMNode", NULL, php_dom_node_class_functions, dom_node_class_entry);

	zend_hash_init(&dom_node_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeName", dom_node_node_name_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeValue", dom_node_node_value_read, dom_node_node_value_write);
	dom_register_prop_handler(&dom_node_prop_handlers, "nodeType", dom_node_node_type_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "parentNode", dom_node_parent_node_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "childNodes", dom_node_child_nodes_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "firstChild", dom_node_first_child_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "lastChild", dom_node_last_child_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "previousSibling", dom_node_previous_sibling_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "nextSibling", dom_node_next_sibling_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "attributes", dom_node_attributes_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "ownerDocument", dom_node_owner_document_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "prefix", dom_node_prefix_read, dom_node_prefix_write);
	dom_register_prop_handler(&dom_node_prop_handlers, "localName", dom_node_local_name_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "baseURI", dom_node_base_uri_read, NULL);
	dom_register_prop_handler(&dom_node_prop_handlers, "textContent", dom_node_text_content_read, dom_node_text_content_write);
	zend_hash_add_ptr(&classes, ce.name, &dom_node_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMNameSpaceNode", NULL, NULL, dom_namespace_node_class_entry);

	zend_hash_init(&dom_namespace_node_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeName", dom_node_node_name_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeValue", dom_node_node_value_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "nodeType", dom_node_node_type_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "prefix", dom_node_prefix_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "localName", dom_node_local_name_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "namespaceURI", dom_node_namespace_uri_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "ownerDocument", dom_node_owner_document_read, NULL);
	dom_register_prop_handler(&dom_namespace_node_prop_handlers, "parentNode", dom_node_parent_node_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_namespace_node_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMDocumentFragment", dom_node_class_entry, php_dom_documentfragment_class_functions, dom_documentfragment_class_entry);
	zend_hash_add_ptr(&classes, ce.name, &dom_node_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMDocument", dom_node_class_entry, php_dom_document_class_functions, dom_document_class_entry);
	zend_hash_init(&dom_document_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_document_prop_handlers, "doctype", dom_document_doctype_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "implementation", dom_document_implementation_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "documentElement", dom_document_document_element_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "actualEncoding", dom_document_encoding_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "encoding", dom_document_encoding_read, dom_document_encoding_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlEncoding", dom_document_encoding_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "standalone", dom_document_standalone_read, dom_document_standalone_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlStandalone", dom_document_standalone_read, dom_document_standalone_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "version", dom_document_version_read, dom_document_version_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "xmlVersion", dom_document_version_read, dom_document_version_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "strictErrorChecking", dom_document_strict_error_checking_read, dom_document_strict_error_checking_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "documentURI", dom_document_document_uri_read, dom_document_document_uri_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "config", dom_document_config_read, NULL);
	dom_register_prop_handler(&dom_document_prop_handlers, "formatOutput", dom_document_format_output_read, dom_document_format_output_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "validateOnParse", dom_document_validate_on_parse_read, dom_document_validate_on_parse_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "resolveExternals", dom_document_resolve_externals_read, dom_document_resolve_externals_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "preserveWhiteSpace", dom_document_preserve_whitespace_read, dom_document_preserve_whitespace_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "recover", dom_document_recover_read, dom_document_recover_write);
	dom_register_prop_handler(&dom_document_prop_handlers, "substituteEntities", dom_document_substitue_entities_read, dom_document_substitue_entities_write);

	zend_hash_merge(&dom_document_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_document_prop_handlers);

	INIT_CLASS_ENTRY(ce, "DOMNodeList", php_dom_nodelist_class_functions);
	ce.create_object = dom_nnodemap_objects_new;
	dom_nodelist_class_entry = zend_register_internal_class_ex(&ce, NULL);
	dom_nodelist_class_entry->get_iterator = php_dom_get_iterator;
	zend_class_implements(dom_nodelist_class_entry, 1, zend_ce_traversable);

	zend_hash_init(&dom_nodelist_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_nodelist_prop_handlers, "length", dom_nodelist_length_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_nodelist_prop_handlers);

	INIT_CLASS_ENTRY(ce, "DOMNamedNodeMap", php_dom_namednodemap_class_functions);
	ce.create_object = dom_nnodemap_objects_new;
	dom_namednodemap_class_entry = zend_register_internal_class_ex(&ce, NULL);
	dom_namednodemap_class_entry->get_iterator = php_dom_get_iterator;
	zend_class_implements(dom_namednodemap_class_entry, 1, zend_ce_traversable);

	zend_hash_init(&dom_namednodemap_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_namednodemap_prop_handlers, "length", dom_namednodemap_length_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_namednodemap_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMCharacterData", dom_node_class_entry, php_dom_characterdata_class_functions, dom_characterdata_class_entry);

	zend_hash_init(&dom_characterdata_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "data", dom_characterdata_data_read, dom_characterdata_data_write);
	dom_register_prop_handler(&dom_characterdata_prop_handlers, "length", dom_characterdata_length_read, NULL);
	zend_hash_merge(&dom_characterdata_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_characterdata_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMAttr", dom_node_class_entry, php_dom_attr_class_functions, dom_attr_class_entry);

	zend_hash_init(&dom_attr_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_attr_prop_handlers, "name", dom_attr_name_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "specified", dom_attr_specified_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "value", dom_attr_value_read, dom_attr_value_write);
	dom_register_prop_handler(&dom_attr_prop_handlers, "ownerElement", dom_attr_owner_element_read, NULL);
	dom_register_prop_handler(&dom_attr_prop_handlers, "schemaTypeInfo", dom_attr_schema_type_info_read, NULL);
	zend_hash_merge(&dom_attr_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_attr_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMElement", dom_node_class_entry, php_dom_element_class_functions, dom_element_class_entry);

	zend_hash_init(&dom_element_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_element_prop_handlers, "tagName", dom_element_tag_name_read, NULL);
	dom_register_prop_handler(&dom_element_prop_handlers, "schemaTypeInfo", dom_element_schema_type_info_read, NULL);
	zend_hash_merge(&dom_element_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_element_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMText", dom_characterdata_class_entry, php_dom_text_class_functions, dom_text_class_entry);

	zend_hash_init(&dom_text_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_text_prop_handlers, "wholeText", dom_text_whole_text_read, NULL);
	zend_hash_merge(&dom_text_prop_handlers, &dom_characterdata_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_text_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMComment", dom_characterdata_class_entry, php_dom_comment_class_functions, dom_comment_class_entry);
	zend_hash_add_ptr(&classes, ce.name, &dom_characterdata_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMTypeinfo", NULL, php_dom_typeinfo_class_functions, dom_typeinfo_class_entry);

	zend_hash_init(&dom_typeinfo_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_typeinfo_prop_handlers, "typeName", dom_typeinfo_type_name_read, NULL);
	dom_register_prop_handler(&dom_typeinfo_prop_handlers, "typeNamespace", dom_typeinfo_type_namespace_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_typeinfo_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMUserDataHandler", NULL, php_dom_userdatahandler_class_functions, dom_userdatahandler_class_entry);
	REGISTER_DOM_CLASS(ce, "DOMDomError", NULL, php_dom_domerror_class_functions, dom_domerror_class_entry);

	zend_hash_init(&dom_domerror_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "severity", dom_domerror_severity_read, NULL);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "message", dom_domerror_message_read, NULL);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "type", dom_domerror_type_read, NULL);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "relatedException", dom_domerror_related_exception_read, NULL);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "related_data", dom_domerror_related_data_read, NULL);
	dom_register_prop_handler(&dom_domerror_prop_handlers, "location", dom_domerror_location_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_domerror_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMErrorHandler", NULL, php_dom_domerrorhandler_class_functions, dom_domerrorhandler_class_entry);
	REGISTER_DOM_CLASS(ce, "DOMLocator", NULL, php_dom_domlocator_class_functions, dom_domlocator_class_entry);

	zend_hash_init(&dom_domlocator_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_domlocator_prop_handlers, "lineNumber", dom_domlocator_line_number_read, NULL);
	dom_register_prop_handler(&dom_domlocator_prop_handlers, "columnNumber", dom_domlocator_column_number_read, NULL);
	dom_register_prop_handler(&dom_domlocator_prop_handlers, "offset", dom_domlocator_offset_read, NULL);
	dom_register_prop_handler(&dom_domlocator_prop_handlers, "relatedNode", dom_domlocator_related_node_read, NULL);
	dom_register_prop_handler(&dom_domlocator_prop_handlers, "uri", dom_domlocator_uri_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_domlocator_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMConfiguration", NULL, php_dom_domconfiguration_class_functions, dom_domconfiguration_class_entry);
	REGISTER_DOM_CLASS(ce, "DOMCdataSection", dom_text_class_entry, php_dom_cdatasection_class_functions, dom_cdatasection_class_entry);
	zend_hash_add_ptr(&classes, ce.name, &dom_text_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMDocumentType", dom_node_class_entry, php_dom_documenttype_class_functions, dom_documenttype_class_entry);

	zend_hash_init(&dom_documenttype_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "name", dom_documenttype_name_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "entities", dom_documenttype_entities_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "notations", dom_documenttype_notations_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "publicId", dom_documenttype_public_id_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "systemId", dom_documenttype_system_id_read, NULL);
	dom_register_prop_handler(&dom_documenttype_prop_handlers, "internalSubset", dom_documenttype_internal_subset_read, NULL);
	zend_hash_merge(&dom_documenttype_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_documenttype_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMNotation", dom_node_class_entry, php_dom_notation_class_functions, dom_notation_class_entry);

	zend_hash_init(&dom_notation_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_notation_prop_handlers, "publicId", dom_notation_public_id_read, NULL);
	dom_register_prop_handler(&dom_notation_prop_handlers, "systemId", dom_notation_system_id_read, NULL);
	zend_hash_merge(&dom_notation_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_notation_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMEntity", dom_node_class_entry, php_dom_entity_class_functions, dom_entity_class_entry);

	zend_hash_init(&dom_entity_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_entity_prop_handlers, "publicId", dom_entity_public_id_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "systemId", dom_entity_system_id_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "notationName", dom_entity_notation_name_read, NULL);
	dom_register_prop_handler(&dom_entity_prop_handlers, "actualEncoding", dom_entity_actual_encoding_read, dom_entity_actual_encoding_write);
	dom_register_prop_handler(&dom_entity_prop_handlers, "encoding", dom_entity_encoding_read, dom_entity_encoding_write);
	dom_register_prop_handler(&dom_entity_prop_handlers, "version", dom_entity_version_read, dom_entity_version_write);
	zend_hash_merge(&dom_entity_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_entity_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMEntityReference", dom_node_class_entry, php_dom_entityreference_class_functions, dom_entityreference_class_entry);
	zend_hash_add_ptr(&classes, ce.name, &dom_node_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMProcessingInstruction", dom_node_class_entry, php_dom_processinginstruction_class_functions, dom_processinginstruction_class_entry);

	zend_hash_init(&dom_processinginstruction_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_processinginstruction_prop_handlers, "target", dom_processinginstruction_target_read, NULL);
	dom_register_prop_handler(&dom_processinginstruction_prop_handlers, "data", dom_processinginstruction_data_read, dom_processinginstruction_data_write);
	zend_hash_merge(&dom_processinginstruction_prop_handlers, &dom_node_prop_handlers, dom_copy_prop_handler, 0);
	zend_hash_add_ptr(&classes, ce.name, &dom_processinginstruction_prop_handlers);

	REGISTER_DOM_CLASS(ce, "DOMStringExtend", NULL, php_dom_string_extend_class_functions, dom_string_extend_class_entry);

#if defined(LIBXML_XPATH_ENABLED)
	memcpy(&dom_xpath_object_handlers, &dom_object_handlers, sizeof(zend_object_handlers));
	dom_xpath_object_handlers.offset = XtOffsetOf(dom_xpath_object, dom) + XtOffsetOf(dom_object, std);
	dom_xpath_object_handlers.free_obj = dom_xpath_objects_free_storage;

	INIT_CLASS_ENTRY(ce, "DOMXPath", php_dom_xpath_class_functions);
	ce.create_object = dom_xpath_objects_new;
	dom_xpath_class_entry = zend_register_internal_class_ex(&ce, NULL);

	zend_hash_init(&dom_xpath_prop_handlers, 0, NULL, dom_dtor_prop_handler, 1);
	dom_register_prop_handler(&dom_xpath_prop_handlers, "document", dom_xpath_document_read, NULL);
	zend_hash_add_ptr(&classes, ce.name, &dom_xpath_prop_handlers);
#endif

	REGISTER_LONG_CONSTANT("XML_ELEMENT_NODE",			XML_ELEMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NODE",		XML_ATTRIBUTE_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_TEXT_NODE",				XML_TEXT_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_CDATA_SECTION_NODE",	XML_CDATA_SECTION_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_REF_NODE",		XML_ENTITY_REF_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_NODE",			XML_ENTITY_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_PI_NODE",				XML_PI_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_COMMENT_NODE",			XML_COMMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_NODE",			XML_DOCUMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_TYPE_NODE",	XML_DOCUMENT_TYPE_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_FRAG_NODE",	XML_DOCUMENT_FRAG_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NOTATION_NODE",			XML_NOTATION_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_HTML_DOCUMENT_NODE",	XML_HTML_DOCUMENT_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DTD_NODE",				XML_DTD_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ELEMENT_DECL_NODE", 	XML_ELEMENT_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_DECL_NODE",	XML_ATTRIBUTE_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_DECL_NODE",		XML_ENTITY_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NAMESPACE_DECL_NODE",	XML_NAMESPACE_DECL,			CONST_CS | CONST_PERSISTENT);
#ifdef XML_GLOBAL_NAMESPACE
	REGISTER_LONG_CONSTANT("XML_GLOBAL_NAMESPACE",		XML_GLOBAL_NAMESPACE,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("XML_LOCAL_NAMESPACE",		XML_LOCAL_NAMESPACE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_CDATA",		XML_ATTRIBUTE_CDATA,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ID",			XML_ATTRIBUTE_ID,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREF",		XML_ATTRIBUTE_IDREF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREFS",		XML_ATTRIBUTE_IDREFS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENTITY",		XML_ATTRIBUTE_ENTITIES,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKEN",		XML_ATTRIBUTE_NMTOKEN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKENS",	XML_ATTRIBUTE_NMTOKENS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENUMERATION",	XML_ATTRIBUTE_ENUMERATION,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NOTATION",	XML_ATTRIBUTE_NOTATION,		CONST_CS | CONST_PERSISTENT);

	/* DOMException Codes */
	REGISTER_LONG_CONSTANT("DOM_PHP_ERR",				PHP_ERR,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INDEX_SIZE_ERR",		INDEX_SIZE_ERR,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMSTRING_SIZE_ERR",		DOMSTRING_SIZE_ERR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_HIERARCHY_REQUEST_ERR",	HIERARCHY_REQUEST_ERR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_WRONG_DOCUMENT_ERR",	WRONG_DOCUMENT_ERR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_CHARACTER_ERR",	INVALID_CHARACTER_ERR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NO_DATA_ALLOWED_ERR",	NO_DATA_ALLOWED_ERR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NO_MODIFICATION_ALLOWED_ERR", NO_MODIFICATION_ALLOWED_ERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NOT_FOUND_ERR",			NOT_FOUND_ERR,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NOT_SUPPORTED_ERR",		NOT_SUPPORTED_ERR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INUSE_ATTRIBUTE_ERR",	INUSE_ATTRIBUTE_ERR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_STATE_ERR",		INVALID_STATE_ERR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_SYNTAX_ERR",			SYNTAX_ERR,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_MODIFICATION_ERR",	INVALID_MODIFICATION_ERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NAMESPACE_ERR",			NAMESPACE_ERR,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_ACCESS_ERR",	INVALID_ACCESS_ERR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_VALIDATION_ERR",		VALIDATION_ERR,			CONST_CS | CONST_PERSISTENT);

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
#if defined(LIBXML_HTML_ENABLED)
	php_info_print_table_row(2, "HTML Support", "enabled");
#endif
#if defined(LIBXML_XPATH_ENABLED)
	php_info_print_table_row(2, "XPath Support", "enabled");
#endif
#if defined(LIBXML_XPTR_ENABLED)
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
	zend_hash_destroy(&dom_domstringlist_prop_handlers);
	zend_hash_destroy(&dom_namelist_prop_handlers);
	zend_hash_destroy(&dom_domimplementationlist_prop_handlers);
	zend_hash_destroy(&dom_document_prop_handlers);
	zend_hash_destroy(&dom_node_prop_handlers);
	zend_hash_destroy(&dom_namespace_node_prop_handlers);
	zend_hash_destroy(&dom_nodelist_prop_handlers);
	zend_hash_destroy(&dom_namednodemap_prop_handlers);
	zend_hash_destroy(&dom_characterdata_prop_handlers);
	zend_hash_destroy(&dom_attr_prop_handlers);
	zend_hash_destroy(&dom_element_prop_handlers);
	zend_hash_destroy(&dom_text_prop_handlers);
	zend_hash_destroy(&dom_typeinfo_prop_handlers);
	zend_hash_destroy(&dom_domerror_prop_handlers);
	zend_hash_destroy(&dom_domlocator_prop_handlers);
	zend_hash_destroy(&dom_documenttype_prop_handlers);
	zend_hash_destroy(&dom_notation_prop_handlers);
	zend_hash_destroy(&dom_entity_prop_handlers);
	zend_hash_destroy(&dom_processinginstruction_prop_handlers);
#if defined(LIBXML_XPATH_ENABLED)
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

#if defined(LIBXML_XPATH_ENABLED)
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

	//??? if (basenode)
	ZVAL_OBJ(&mapptr->baseobj_zv, &basenode->std);
	Z_ADDREF(mapptr->baseobj_zv);

	mapptr->baseobj = basenode;
	mapptr->nodetype = ntype;
	mapptr->ht = ht;
	mapptr->local = local;
	mapptr->ns = ns;
}
/* }}} */

static dom_object* dom_objects_set_class(zend_class_entry *class_type, zend_bool hash_copy) /* {{{ */
{
	dom_object *intern = ecalloc(1, sizeof(dom_object) + sizeof(zval) * (class_type->default_properties_count - 1));

	zend_class_entry *base_class = class_type;
	while (base_class->type != ZEND_INTERNAL_CLASS && base_class->parent != NULL) {
		base_class = base_class->parent;
	}

	intern->prop_handler = zend_hash_find_ptr(&classes, base_class->name);

	zend_object_std_init(&intern->std, class_type);
	if (hash_copy) {
		object_properties_init(&intern->std, class_type);
	}

	return intern;
}
/* }}} */

/* {{{ dom_objects_new */
zend_object *dom_objects_new(zend_class_entry *class_type)
{
	dom_object *intern = dom_objects_set_class(class_type, 1);
	intern->std.handlers = dom_get_obj_handlers();
	return &intern->std;
}
/* }}} */

#if defined(LIBXML_XPATH_ENABLED)
/* {{{ zend_object_value dom_xpath_objects_new(zend_class_entry *class_type) */
zend_object *dom_xpath_objects_new(zend_class_entry *class_type)
{
	dom_xpath_object *intern = ecalloc(1, sizeof(dom_xpath_object) + sizeof(zval) * (class_type->default_properties_count - 1));

	ALLOC_HASHTABLE(intern->registered_phpfunctions);
	zend_hash_init(intern->registered_phpfunctions, 0, NULL, ZVAL_PTR_DTOR, 0);

	intern->dom.prop_handler = &dom_xpath_prop_handlers;
	intern->dom.std.handlers = &dom_xpath_object_handlers;

	zend_object_std_init(&intern->dom.std, class_type);
	object_properties_init(&intern->dom.std, class_type);

	return &intern->dom.std;
}
/* }}} */
#endif

static void dom_nnodemap_object_dtor(zend_object *object) /* {{{ */
{
	dom_object *intern;
	dom_nnodemap_object *objmap;

	intern = php_dom_obj_from_obj(object);
	objmap = (dom_nnodemap_object *)intern->ptr;

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
}
/* }}} */

void dom_nnodemap_objects_free_storage(zend_object *object) /* {{{ */
{
	dom_object *intern = php_dom_obj_from_obj(object);

	php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);

	zend_object_std_dtor(&intern->std);
}
/* }}} */

zend_object *dom_nnodemap_objects_new(zend_class_entry *class_type) /* {{{ */
{
	dom_object *intern;
	dom_nnodemap_object *objmap;

	intern = dom_objects_set_class(class_type, 1);
	intern->ptr = emalloc(sizeof(dom_nnodemap_object));
	objmap = (dom_nnodemap_object *)intern->ptr;
	ZVAL_UNDEF(&objmap->baseobj_zv);
	objmap->baseobj = NULL;
	objmap->nodetype = 0;
	objmap->ht = NULL;
	objmap->local = NULL;
	objmap->ns = NULL;

	intern->std.handlers = &dom_nnodemap_object_handlers;

	return &intern->std;
}
/* }}} */

void php_dom_create_interator(zval *return_value, int ce_type) /* {{{ */
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
PHP_DOM_EXPORT zend_bool php_dom_create_object(xmlNodePtr obj, zval *return_value, dom_object *domobj)
{
	zend_class_entry *ce;
	dom_object *intern;

	if (!obj) {
		ZVAL_NULL(return_value);
		return 0;
	}

	if ((intern = (dom_object *) php_dom_object_get_data((void *) obj))) {
		GC_REFCOUNT(&intern->std)++;
		ZVAL_OBJ(return_value, &intern->std);
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
			php_error_docref(NULL, E_WARNING, "Unsupported node type: %d", obj->type);
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

/* {{{ dom_has_feature(char *feature, char *version) */
int dom_has_feature(char *feature, char *version)
{
	int retval = 0;

	if (!(strcmp (version, "1.0") && strcmp (version,"2.0") && strcmp(version, ""))) {
		if ((!strcasecmp(feature, "Core") && !strcmp (version, "1.0")) || !strcasecmp(feature, "XML"))
			retval = 1;
	}

	return retval;
}
/* }}} end dom_has_feature */

xmlNode *dom_get_elements_by_tag_name_ns_raw(xmlNodePtr nodep, char *ns, char *local, int *cur, int index) /* {{{ */
{
	xmlNodePtr ret = NULL;

	while (nodep != NULL && (*cur <= index || index == -1)) {
		if (nodep->type == XML_ELEMENT_NODE) {
			if (xmlStrEqual(nodep->name, (xmlChar *)local) || xmlStrEqual((xmlChar *)"*", (xmlChar *)local)) {
				if (ns == NULL || (nodep->ns != NULL && (xmlStrEqual(nodep->ns->href, (xmlChar *)ns) || xmlStrEqual((xmlChar *)"*", (xmlChar *)ns)))) {
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

zval *dom_nodelist_read_dimension(zval *object, zval *offset, int type, zval *rv) /* {{{ */
{
	zval offset_copy;

	if (!offset) {
		return NULL;
	}

	ZVAL_LONG(&offset_copy, zval_get_long(offset));

	zend_call_method_with_1_params(object, Z_OBJCE_P(object), NULL, "item", rv, &offset_copy);

	return rv;
} /* }}} end dom_nodelist_read_dimension */

int dom_nodelist_has_dimension(zval *object, zval *member, int check_empty)
{
	zend_long offset = zval_get_long(member);
	zval rv;

	if (offset < 0) {
		return 0;
	} else {
		zval *length = zend_read_property(Z_OBJCE_P(object), object, "length", sizeof("length") - 1, 0, &rv);

		return length && offset < Z_LVAL_P(length);
	}
} /* }}} end dom_nodelist_has_dimension */

#endif /* HAVE_DOM */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
