/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann <steinm@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_domxml.h"

#if HAVE_DOMXML
#include "ext/standard/info.h"
#define PHP_XPATH 1
#define PHP_XPTR 2

static int le_domxmldocp;
static int le_domxmldoctypep;
static int le_domxmldtdp;
static int le_domxmlnodep;
static int le_domxmlelementp;
static int le_domxmlattrp;
static int le_domxmlcdatap;
static int le_domxmltextp;
static int le_domxmlpip;
static int le_domxmlcommentp;
static int le_domxmlnotationp;
/*static int le_domxmlentityp;*/
static int le_domxmlentityrefp;
/*static int le_domxmlnsp;*/

#if defined(LIBXML_XPATH_ENABLED)
static int le_xpathctxp;
static int le_xpathobjectp;
#endif

zend_class_entry *domxmldoc_class_entry;
zend_class_entry *domxmldoctype_class_entry;
zend_class_entry *domxmlelement_class_entry;
zend_class_entry *domxmldtd_class_entry;
zend_class_entry *domxmlnode_class_entry;
zend_class_entry *domxmlattr_class_entry;
zend_class_entry *domxmlcdata_class_entry;
zend_class_entry *domxmltext_class_entry;
zend_class_entry *domxmlpi_class_entry;
zend_class_entry *domxmlcomment_class_entry;
zend_class_entry *domxmlnotation_class_entry;
zend_class_entry *domxmlentity_class_entry;
zend_class_entry *domxmlentityref_class_entry;
zend_class_entry *domxmlns_class_entry;
#if defined(LIBXML_XPATH_ENABLED)
zend_class_entry *xpathctx_class_entry;
zend_class_entry *xpathobject_class_entry;
#endif


static int node_attributes(zval **attributes, xmlNode *nodep TSRMLS_DC);
static int node_children(zval **children, xmlNode *nodep TSRMLS_DC);

static zend_function_entry domxml_functions[] = {
	PHP_FE(domxml_version,	NULL)
	PHP_FE(xmldoc,	NULL)
	PHP_FE(xmldocfile,	NULL)
	PHP_FE(xmltree,	NULL)
	PHP_FALIAS(domxml_root,	domxml_doc_document_element, NULL)
	PHP_FE(domxml_add_root,	NULL)
	PHP_FE(domxml_dumpmem,	NULL)
	PHP_FE(domxml_node_attributes,	NULL)
	PHP_FALIAS(domxml_attributes,	domxml_node_attributes, NULL)
	PHP_FE(domxml_elem_get_attribute,	NULL)
	PHP_FALIAS(domxml_get_attribute,	domxml_elem_get_attribute, NULL)
	PHP_FALIAS(domxml_getattr,	domxml_elem_get_attribute, NULL)
	PHP_FE(domxml_elem_set_attribute,	NULL)
	PHP_FALIAS(domxml_set_attribute,	domxml_elem_set_attribute, NULL)
	PHP_FALIAS(domxml_setattr,	domxml_elem_set_attribute, NULL)
	PHP_FE(domxml_node_children,	NULL)
	PHP_FALIAS(domxml_children,	domxml_node_children, NULL)
	PHP_FE(domxml_node_new_child,	NULL)
	PHP_FALIAS(domxml_new_child,	domxml_node_new_child, NULL)
	PHP_FE(domxml_node,	NULL)
	PHP_FE(domxml_node_unlink_node,	NULL)
	PHP_FALIAS(domxml_unlink_node,	domxml_node_unlink_node, NULL)
	PHP_FE(domxml_node_set_content,	NULL)
	PHP_FALIAS(set_content,	domxml_node_set_content,	NULL)
	PHP_FE(domxml_new_xmldoc,	NULL)
	PHP_FALIAS(new_xmldoc, domxml_new_xmldoc,	NULL)
#if defined(LIBXML_XPATH_ENABLED)
	PHP_FE(xpath_new_context, NULL)
	PHP_FE(xpath_eval, NULL)
	PHP_FE(xpath_eval_expression, NULL)
#endif
#if defined(LIBXML_XPTR_ENABLED)
	PHP_FE(xptr_new_context, NULL)
	PHP_FE(xptr_eval, NULL)
#endif
	{NULL, NULL, NULL}
};


static function_entry php_domxmldoc_class_functions[] = {
/*	PHP_FALIAS(domdocument, xmldoc, NULL) */
	{"domdocument", PHP_FN(xmldoc), NULL},
	PHP_FALIAS(doctype, domxml_doc_doctype,	NULL)
	PHP_FALIAS(implementation, domxml_doc_implementation,	NULL)
	PHP_FALIAS(root, domxml_doc_document_element,	NULL) /* not DOM */
	PHP_FALIAS(document_element,	domxml_doc_document_element,	NULL)
	PHP_FALIAS(documentelement,	domxml_doc_document_element,	NULL)
	PHP_FALIAS(createelement,	domxml_doc_create_element,	NULL)
	PHP_FALIAS(create_element,	domxml_doc_create_element,	NULL)
	PHP_FALIAS(createtextnode,	domxml_doc_create_text_node,	NULL)
	PHP_FALIAS(create_text_node,	domxml_doc_create_text_node,	NULL)
	PHP_FALIAS(createcomment,	domxml_doc_create_comment,	NULL)
	PHP_FALIAS(create_comment,	domxml_doc_create_comment,	NULL)
	PHP_FALIAS(createattribute,	domxml_doc_create_attribute,	NULL)
	PHP_FALIAS(create_attribute,	domxml_doc_create_attribute,	NULL)
	PHP_FALIAS(createprocessinginstruction,	domxml_doc_create_processing_instruction,	NULL)
	PHP_FALIAS(create_processing_instruction,	domxml_doc_create_processing_instruction,	NULL)
	PHP_FALIAS(children,	domxml_node_children,	NULL)
	PHP_FALIAS(add_root,	domxml_add_root,	NULL)
	PHP_FALIAS(importednode,	domxml_doc_imported_node,	NULL)
	PHP_FALIAS(imported_node,	domxml_doc_imported_node,	NULL)
	PHP_FALIAS(dtd,	domxml_intdtd,	NULL)
	PHP_FALIAS(dumpmem,	domxml_dumpmem,	NULL)
/*	PHP_FALIAS(createcdatasection,	domxml_create_cdata_section,	NULL)
	PHP_FALIAS(create_cdata_section,	domxml_create_cdata_section,	NULL)
*/
#if defined(LIBXML_XPATH_ENABLED)
	PHP_FALIAS(xpath_init, xpath_init, NULL)
	PHP_FALIAS(xpath_new_context, xpath_new_context, NULL)
	PHP_FALIAS(xptr_new_context, xptr_new_context, NULL)
#endif
	{NULL, NULL, NULL}
};

static function_entry php_domxmldoctype_class_functions[] = {
	PHP_FALIAS(name, domxml_doctype_name, NULL)
/*	PHP_FALIAS(entities, domxml_doctype_entities, NULL)
	PHP_FALIAS(notations, domxml_doctype_notations, NULL)
	PHP_FALIAS(systemid, domxml_doctype_system_id, NULL)
	PHP_FALIAS(system_id, domxml_doctype_system_id, NULL)
	PHP_FALIAS(publicid, domxml_doctype_public_id, NULL)
	PHP_FALIAS(public_id, domxml_doctype_public_id, NULL)
	PHP_FALIAS(internalsubset, domxml_doctype_internal_subset, NULL)
	PHP_FALIAS(internal_subset, domxml_doctype_internal_subset, NULL)
*/	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnode_class_functions[] = {
	PHP_FALIAS(domnode, domxml_node, NULL)
	PHP_FALIAS(firstchild,	domxml_node_first_child,	NULL)
	PHP_FALIAS(first_child,	domxml_node_first_child,	NULL)
	PHP_FALIAS(lastchild,	domxml_node_last_child,	NULL)
	PHP_FALIAS(last_child,	domxml_node_last_child,	NULL)
	PHP_FALIAS(add_child,	domxml_node_add_child,	NULL)
	PHP_FALIAS(children,	domxml_node_children,	NULL)
	PHP_FALIAS(childnodes,	domxml_node_children,	NULL)
	PHP_FALIAS(child_nodes,	domxml_node_children,	NULL)
	PHP_FALIAS(previoussibling,	domxml_node_previous_sibling,	NULL)
	PHP_FALIAS(previous_sibling,	domxml_node_previous_sibling,	NULL)
	PHP_FALIAS(nextsibling,	domxml_node_next_sibling,	NULL)
	PHP_FALIAS(next_sibling,	domxml_node_next_sibling,	NULL)
	PHP_FALIAS(has_child_nodes,	domxml_node_has_child_nodes,	NULL)
	PHP_FALIAS(haschildnodes,	domxml_node_has_child_nodes,	NULL)
	PHP_FALIAS(prefix,	domxml_node_prefix,		NULL)
	PHP_FALIAS(parent,	domxml_node_parent,		NULL)
	PHP_FALIAS(parentnode,	domxml_node_parent,		NULL)
	PHP_FALIAS(parent_node,	domxml_node_parent,		NULL)
	PHP_FALIAS(insert_before,	domxml_node_insert_before,		NULL)
	PHP_FALIAS(insertbefore,	domxml_node_insert_before,		NULL)
	PHP_FALIAS(append_child,	domxml_node_append_child,		NULL)
	PHP_FALIAS(appendchild,	domxml_node_append_child,		NULL)
	PHP_FALIAS(ownerdocument,	domxml_node_owner_document,		NULL)
	PHP_FALIAS(owner_document,	domxml_node_owner_document,		NULL)
	PHP_FALIAS(new_child,	domxml_node_new_child,		NULL)
	PHP_FALIAS(attributes,	domxml_node_attributes,	NULL)
	PHP_FALIAS(node,	domxml_node,	NULL)
	PHP_FALIAS(unlink,	domxml_node_unlink_node,	NULL)
	PHP_FALIAS(set_content,	domxml_node_set_content,	NULL)
	PHP_FALIAS(text_concat,	domxml_node_text_concat,	NULL)
	PHP_FALIAS(set_name,	domxml_node_set_name,	NULL)
	PHP_FALIAS(node_name,	domxml_node_name,	NULL)
	PHP_FALIAS(nodename,	domxml_node_name,	NULL)
	PHP_FALIAS(node_value,	domxml_node_value,	NULL)
	PHP_FALIAS(nodevalue,	domxml_node_value,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlelement_class_functions[] = {
	PHP_FALIAS(domelement, domxml_element, NULL)
	PHP_FALIAS(tagname, domxml_elem_tagname, NULL)
	PHP_FALIAS(getattribute,	domxml_elem_get_attribute,		NULL)
	PHP_FALIAS(get_attribute,	domxml_elem_get_attribute,		NULL)
	PHP_FALIAS(setattribute,	domxml_elem_set_attribute,		NULL)
	PHP_FALIAS(set_attribute,	domxml_elem_set_attribute,		NULL)
	PHP_FALIAS(removeattribute,	domxml_elem_remove_attribute,		NULL)
	PHP_FALIAS(remove_attribute,	domxml_elem_remove_attribute,		NULL)
	PHP_FALIAS(getattributenode,	domxml_elem_get_attribute_node,		NULL)
	PHP_FALIAS(get_attributenode,	domxml_elem_get_attribute_node,		NULL)
	PHP_FALIAS(setattributenode,	domxml_elem_set_attribute_node,		NULL)
	PHP_FALIAS(set_attributenode,	domxml_elem_set_attribute_node,		NULL)
	PHP_FALIAS(getelementbytagname,	domxml_elem_get_element_by_tagname,		NULL)
	PHP_FALIAS(get_element_by_tagname,	domxml_elem_get_element_by_tagname,		NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlcdata_class_functions[] = {
	PHP_FALIAS(length,	domxml_cdata_length,		NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmltext_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlcomment_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnotation_class_functions[] = {
	PHP_FALIAS(publicid,	domxml_notation_public_id,		NULL)
	PHP_FALIAS(public_id,	domxml_notation_public_id,		NULL)
	PHP_FALIAS(systemid,	domxml_notation_system_id,		NULL)
	PHP_FALIAS(system_id,	domxml_notation_system_id,		NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlentityref_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlentity_class_functions[] = {
/*	PHP_FALIAS(publicid,	domxml_entity_public_id,		NULL)
	PHP_FALIAS(public_id,	domxml_entity_public_id,		NULL)
	PHP_FALIAS(systemid,	domxml_entity_system_id,		NULL)
	PHP_FALIAS(system_id,	domxml_entity_system_id,		NULL)
	PHP_FALIAS(notationname,	domxml_entity_notation_name,		NULL)
	PHP_FALIAS(notation_name,	domxml_entity_notation_name,		NULL)
*/	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlpi_class_functions[] = {
	PHP_FALIAS(target,	domxml_pi_target,		NULL)
	PHP_FALIAS(data,	domxml_pi_data,		NULL)
	{NULL, NULL, NULL}
};

#if defined(LIBXML_XPATH_ENABLED)
static zend_function_entry php_xpathctx_class_functions[] = {
	PHP_FALIAS(xpath_eval, xpath_eval, NULL)
	PHP_FALIAS(xpath_eval_expression, xpath_eval_expression, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_xpathobject_class_functions[] = {
	{NULL, NULL, NULL}
};
#endif

static zend_function_entry php_domxmlattr_class_functions[] = {
	PHP_FALIAS(name,	domxml_attr_name,	NULL)
	PHP_FALIAS(value,	domxml_attr_value,	NULL)
	PHP_FALIAS(specified,	domxml_attr_specified,	NULL)
/*	PHP_FALIAS(ownerelement,	domxml_attr_owner_element,	NULL)
	PHP_FALIAS(owner_element,	domxml_attr_owner_element,	NULL)
*/	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlns_class_functions[] = {
	{NULL, NULL, NULL}
};

zend_module_entry domxml_module_entry = {
	"domxml", domxml_functions, PHP_MINIT(domxml), NULL, PHP_RINIT(domxml), NULL, PHP_MINFO(domxml), STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DOMXML
ZEND_GET_MODULE(domxml)
#endif

static void dom_object_set_data(void *obj, zval *wrapper) {
	/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp);
	*/	
	((xmlNodePtr) obj)->_private = wrapper;
}

static zval *dom_object_get_data(void *obj) {
	/*	char tmp[20];
		sprintf(tmp, "%08X", obj);
		fprintf(stderr, "Trying getting %s from object ...", tmp);
		if(((xmlNodePtr) obj)->_private)
		fprintf(stderr, " found\n");
		else
		fprintf(stderr, " not found\n"); */
	return((zval *) (((xmlNodePtr) obj)->_private));
}

static inline void node_wrapper_dtor(xmlNodePtr node)
{
	zval *wrapper;

	if (!node || node->type == XML_DTD_NODE)
		return;

	wrapper = dom_object_get_data(node);
	if (wrapper)
		zval_ptr_dtor(&wrapper);
}

static inline void attr_list_wrapper_dtor(xmlAttrPtr attr)
{
	while (attr != NULL) {
		node_wrapper_dtor((xmlNodePtr) attr);
		attr = attr->next;
	}
}

static inline void node_list_wrapper_dtor(xmlNodePtr node)
{
	while (node != NULL) {
		node_list_wrapper_dtor(node->children);
		attr_list_wrapper_dtor(node->properties);
		node_wrapper_dtor(node);
		node = node->next;
	}
}

static void php_free_xml_doc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlDoc *doc = (xmlDoc *)rsrc->ptr;

	if (doc) {
		node_list_wrapper_dtor(doc->children);
		
		node_wrapper_dtor((xmlNodePtr) doc);
		xmlFreeDoc(doc);
	}
}

static void php_free_xml_node(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlNodePtr node = (xmlNodePtr) rsrc->ptr;
	
	if (node) {
		zval *wrapper = dom_object_get_data(node);
		if (wrapper)
			zval_ptr_dtor(&wrapper);
	}
}

#if defined(LIBXML_XPATH_ENABLED)
static void php_free_xpath_context(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlXPathContextPtr ctx = (xmlXPathContextPtr)rsrc->ptr;
	if(ctx)
		xmlXPathFreeContext(ctx);
}

static void php_free_xpath_object(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlXPathObjectPtr obj = (xmlXPathObjectPtr)rsrc->ptr;

	if(obj)
		xmlXPathFreeObject(obj);
}
#endif

void *php_xpath_get_object(zval *wrapper, int rsrc_type1, int rsrc_type2)
{
	void *obj;
	zval **handle;
	int type;
	TSRMLS_FETCH();
					  
	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error(E_ERROR, "Wrapper is not an object");
	}
	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **)&handle) == FAILURE) {
	  php_error(E_ERROR, "Underlying object missing");
	}
	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || ((type != rsrc_type1) && (type != rsrc_type2))) {
		php_error(E_ERROR, "Underlying object missing or of invalid type");
	} 

	return obj;
}  

static void xpath_object_set_data(void *obj, zval *wrapper)
{
/*	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp); */
	((xmlXPathObjectPtr) obj)->user = wrapper;
}

static zval *xpath_object_get_data(void *obj)
{
/*	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Trying getting %s from hash ...", tmp); 
	if(((xmlXPathObjectPtr) obj)->user)
		fprintf(stderr, " found\n"); 
	else
		fprintf(stderr, " not found\n"); */
	return((zval *) (((xmlXPathObjectPtr) obj)->user));
}

static void php_xpath_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);
	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr,  sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	xpath_object_set_data(obj, wrapper);
}

static zval *php_xpathobject_new(xmlXPathObjectPtr obj, int *found TSRMLS_DC) 
{
	zval *wrapper;

	*found = 0;
	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *)xpath_object_get_data((void *)obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	} 

	MAKE_STD_ZVAL(wrapper);
	object_init_ex(wrapper, xpathobject_class_entry);
	/* rsrc_type = le_xpathobjectp;
	php_xpath_set_object(wrapper, (void *) obj, rsrc_type); */

	php_xpath_set_object(wrapper, (void *) obj, le_xpathobjectp);

	return(wrapper);
}

void *php_xpath_get_context(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;
					  
	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error(E_ERROR, "Wrapper is not an object");
	}
	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **)&handle) == FAILURE) {
	  php_error(E_ERROR, "Underlying object missing");
	}
	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || ((type != rsrc_type1) && (type != rsrc_type2))) {
		php_error(E_ERROR, "Underlying object missing or of invalid type");
	} 

	return obj;
}  

static void xpath_context_set_data(void *obj, zval *wrapper)
{
/*	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp); */
	((xmlXPathContextPtr) obj)->user = (void *) wrapper;
}

static zval *xpath_context_get_data(void *obj)
{
/*	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Trying getting %s from hash ...", tmp); 
	if(((xmlXPathContextPtr) obj)->user)
		fprintf(stderr, " found\n"); 
	else
		fprintf(stderr, " not found\n"); */
	return((zval *) (((xmlXPathContextPtr) obj)->user));
}

static void php_xpath_set_context(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);
	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr,  sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	xpath_context_set_data(obj, wrapper);
}

static zval *php_xpathcontext_new(xmlXPathContextPtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;
	int rsrc_type;

	*found = 0;
	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *)xpath_context_get_data((void *)obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	} 

	MAKE_STD_ZVAL(wrapper);
/*	fprintf(stderr, "Adding new XPath Context\n"); */
	object_init_ex(wrapper, xpathctx_class_entry);
	rsrc_type = le_xpathctxp;
	php_xpath_set_context(wrapper, (void *) obj, rsrc_type);

	return(wrapper);
}

void *php_dom_get_object(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;
				  
	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error(E_ERROR, "Wrapper is not an object");
	}
	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **)&handle) == FAILURE) {
	  php_error(E_ERROR, "Underlying object missing");
	}
	obj = zend_list_find(Z_LVAL_PP(handle), &type);
/* The following test should be replaced with search in all parents */
	if (!obj) { /* || ((type != rsrc_type1) && (type != rsrc_type2))) { */
		php_error(E_ERROR, "Underlying object missing or of invalid type");
	} 

	return obj;
}  

static void php_dom_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);
	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr,  sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	dom_object_set_data(obj, wrapper);
}

static zval *php_domobject_new(xmlNodePtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;
	char *content;
	int rsrc_type;

	*found = 0;
	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *)dom_object_get_data((void *)obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	} 

	MAKE_STD_ZVAL(wrapper);
	switch(obj->type) {
		case XML_ELEMENT_NODE: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmlelement_class_entry);
			rsrc_type = le_domxmlelementp;
			add_property_long(wrapper, "type", nodep->type);
			add_property_stringl(wrapper, "tagname", (char *) nodep->name, strlen(nodep->name), 1);
			break;
		}
		case XML_TEXT_NODE: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmltext_class_entry);
			rsrc_type = le_domxmltextp;
			content = xmlNodeGetContent(nodep);
			if(content)
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
			break;
		}
		case XML_COMMENT_NODE: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmlcomment_class_entry);
			rsrc_type = le_domxmlcommentp;
			content = xmlNodeGetContent(nodep);
			if(content)
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
			break;
		}
		case XML_PI_NODE: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmlpi_class_entry);
			rsrc_type = le_domxmlpip;
			content = xmlNodeGetContent(nodep);
			add_property_stringl(wrapper, "target", (char *) nodep->name, strlen(nodep->name), 1);
			if(content)
				add_property_stringl(wrapper, "data", (char *) content, strlen(content), 1);
			break;
		}
		case XML_ENTITY_REF_NODE: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmlentityref_class_entry);
			rsrc_type = le_domxmlentityrefp;
			content = xmlNodeGetContent(nodep);
			add_property_stringl(wrapper, "name", (char *) nodep->name, strlen(nodep->name), 1);
			if(content)
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
			break;
		}
		case XML_ENTITY_DECL: 
		case XML_ELEMENT_DECL: {
			xmlNodePtr nodep = obj;
			object_init_ex(wrapper, domxmlnode_class_entry);
			rsrc_type = le_domxmlnodep;
			add_property_long(wrapper, "type", nodep->type);
			add_property_stringl(wrapper, "name", (char *) nodep->name, strlen(nodep->name), 1);
			if(obj->type == XML_ENTITY_REF_NODE) {
				content = xmlNodeGetContent(nodep);
				if(content)
					add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
			}
			break;
		}
		case XML_ATTRIBUTE_NODE: {
			xmlAttrPtr attrp = (xmlAttrPtr) obj;
			object_init_ex(wrapper, domxmlattr_class_entry);
			rsrc_type = le_domxmlattrp;
			add_property_stringl(wrapper, "name", (char *) attrp->name, strlen(attrp->name), 1);
			content = xmlNodeGetContent((xmlNodePtr) attrp);
			if(content)
				add_property_stringl(wrapper, "value", (char *) content, strlen(content), 1);
			break;
		}
		case XML_DOCUMENT_NODE: {
			xmlDocPtr docp = (xmlDocPtr) obj;
			object_init_ex(wrapper, domxmldoc_class_entry);
			rsrc_type = le_domxmldocp;
			if(docp->name)
				add_property_stringl(wrapper, "name", (char *) docp->name, strlen(docp->name), 1);
			else
				add_property_stringl(wrapper, "name", "", 0, 1);
			if(docp->URL)
				add_property_stringl(wrapper, "url", (char *) docp->URL, strlen(docp->URL), 1);
			else
				add_property_stringl(wrapper, "url", "", 0, 1);
			add_property_stringl(wrapper, "version", (char *) docp->version, strlen(docp->version), 1);
			if(docp->encoding)
				add_property_stringl(wrapper, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
			add_property_long(wrapper, "standalone", docp->standalone);
			add_property_long(wrapper, "type", docp->type);
			add_property_long(wrapper, "compression", docp->compression);
			add_property_long(wrapper, "charset", docp->charset);
			break;
		}
		case XML_DTD_NODE: {
			xmlDtdPtr dtd = (xmlDtdPtr) obj;
			object_init_ex(wrapper, domxmldtd_class_entry);
			rsrc_type = le_domxmldtdp;
			if(dtd->ExternalID)
				add_property_string(wrapper, "publicId", (char *) dtd->ExternalID, 1);
			if(dtd->SystemID)
				add_property_string(wrapper, "systemId", (char *) dtd->SystemID, 1);
			if(dtd->name)
				add_property_string(wrapper, "name", (char *) dtd->name, 1);
			break;
		}
		default:
			fprintf(stderr, "Unsupported Node type: %d\n", obj->type);
			return NULL;
	}
	php_dom_set_object(wrapper, (void *) obj, rsrc_type);

	return(wrapper);
}

PHP_RINIT_FUNCTION(domxml)
{
	return SUCCESS;
}

PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry ce;

	le_domxmldocp = zend_register_list_destructors_ex(php_free_xml_doc, NULL, "domdocument", module_number);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	*/
	le_domxmlnodep = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domnode", module_number);
	le_domxmlattrp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domattribute", module_number);
	le_domxmlelementp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domelement", module_number);
#if defined(LIBXML_XPATH_ENABLED)
	le_xpathctxp = zend_register_list_destructors_ex(php_free_xpath_context, NULL, "xpathcontext", module_number);
	le_xpathobjectp = zend_register_list_destructors_ex(php_free_xpath_object, NULL, "xpathobject", module_number);
#endif

/*	le_domxmlnsp = register_list_destructors(NULL, NULL); */

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomNode", php_domxmlnode_class_functions, NULL, NULL, NULL);
	domxmlnode_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomDocument", php_domxmldoc_class_functions, NULL, NULL, NULL);
	domxmldoc_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomDocumentType", php_domxmldoctype_class_functions, NULL, NULL, NULL);
	domxmldoctype_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "Dtd", php_domxmldtd_class_functions, NULL, NULL, NULL);
	domxmldtd_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomElement", php_domxmlelement_class_functions, NULL, NULL, NULL);
	domxmlelement_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomAttribute", php_domxmlattr_class_functions, NULL, NULL, NULL);
	domxmlattr_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomCData", php_domxmlcdata_class_functions, NULL, NULL, NULL);
	domxmlcdata_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomText", php_domxmltext_class_functions, NULL, NULL, NULL);
	domxmltext_class_entry = zend_register_internal_class_ex(&ce, domxmlcdata_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomComment", php_domxmlcomment_class_functions, NULL, NULL, NULL);
	domxmlcomment_class_entry = zend_register_internal_class_ex(&ce, domxmlcdata_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomProcessingInstruction", php_domxmlpi_class_functions, NULL, NULL, NULL);
	domxmlpi_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomNotation", php_domxmlnotation_class_functions, NULL, NULL, NULL);
	domxmlnotation_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomEntity", php_domxmlentity_class_functions, NULL, NULL, NULL);
	domxmlentity_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomEntityReference", php_domxmlentityref_class_functions, NULL, NULL, NULL);
	domxmlentityref_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "DomNamespace", php_domxmlns_class_functions, NULL, NULL, NULL);
	domxmlns_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

#if defined(LIBXML_XPATH_ENABLED)
	INIT_OVERLOADED_CLASS_ENTRY(ce, "XPathContext", php_xpathctx_class_functions, NULL, NULL, NULL);
	xpathctx_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "XPathObject", php_xpathobject_class_functions, NULL, NULL, NULL);
	xpathobject_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
#endif

	REGISTER_LONG_CONSTANT("XML_ELEMENT_NODE", XML_ELEMENT_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NODE", XML_ATTRIBUTE_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_TEXT_NODE", XML_TEXT_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_CDATA_SECTION_NODE", XML_CDATA_SECTION_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_REF_NODE", XML_ENTITY_REF_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_NODE", XML_ENTITY_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_PI_NODE", XML_PI_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_COMMENT_NODE", XML_COMMENT_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_NODE", XML_DOCUMENT_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_TYPE_NODE", XML_DOCUMENT_TYPE_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_FRAG_NODE", XML_DOCUMENT_FRAG_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NOTATION_NODE", XML_NOTATION_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_HTML_DOCUMENT_NODE", XML_HTML_DOCUMENT_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DTD_NODE", XML_DTD_NODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ELEMENT_DECL_NODE", XML_ELEMENT_DECL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_DECL_NODE", XML_ATTRIBUTE_DECL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_DECL_NODE", XML_ENTITY_DECL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NAMESPACE_DECL_NODE", XML_NAMESPACE_DECL, CONST_CS | CONST_PERSISTENT);
#ifdef XML_GLOBAL_NAMESPACE
	REGISTER_LONG_CONSTANT("XML_GLOBAL_NAMESPACE", XML_GLOBAL_NAMESPACE, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("XML_LOCAL_NAMESPACE", XML_LOCAL_NAMESPACE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_CDATA", XML_ATTRIBUTE_CDATA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ID", XML_ATTRIBUTE_ID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREF", XML_ATTRIBUTE_IDREF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREFS", XML_ATTRIBUTE_IDREFS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENTITY", XML_ATTRIBUTE_ENTITIES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKEN", XML_ATTRIBUTE_NMTOKEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKENS", XML_ATTRIBUTE_NMTOKENS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENUMERATION", XML_ATTRIBUTE_ENUMERATION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NOTATION", XML_ATTRIBUTE_NOTATION, CONST_CS | CONST_PERSISTENT);

#if defined(LIBXML_XPATH_ENABLED)
	REGISTER_LONG_CONSTANT("XPATH_UNDEFINED", XPATH_UNDEFINED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_NODESET", XPATH_NODESET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_BOOLEAN", XPATH_BOOLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_NUMBER", XPATH_NUMBER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_STRING", XPATH_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_POINT", XPATH_POINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_RANGE", XPATH_RANGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_LOCATIONSET", XPATH_LOCATIONSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_USERS", XPATH_USERS, CONST_CS | CONST_PERSISTENT);
#endif

	return SUCCESS;
}

/* {{{ proto int domxml_test(int id)
   Unity function for testing */
PHP_FUNCTION(domxml_test)
{
	zval *id;
	
	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(id);
	RETURN_LONG(Z_LVAL_P(id));
}
/* }}} */

/* }}} */

PHP_MINFO_FUNCTION(domxml)
{
	/* don't know why that line was commented out in the previous version, so i left it (cmv) */
	php_info_print_table_start();	
	php_info_print_table_row(2, "DOM/XML", "enabled");
	php_info_print_table_row(2, "libxml Version", LIBXML_DOTTED_VERSION );
#if defined(LIBXML_XPATH_ENABLED)
	php_info_print_table_row(2, "XPath Support", "enabled");
#endif
#if defined(LIBXML_XPTR_ENABLED)
	php_info_print_table_row(2, "XPointer Support", "enabled");
#endif
	php_info_print_table_end();
}

/* {{{ Methods of Class DomAttribute */

/* {{{ proto array domxml_attr_name()
   Returns list of attribute names */
PHP_FUNCTION(domxml_attr_name)
{
	zval *id;
	xmlAttrPtr attrp;
	
	id = getThis();
	attrp = php_dom_get_object(id, le_domxmlattrp, 0 TSRMLS_CC);
		
	RETURN_STRING((char *) (attrp->name), 1);
}
/* }}} */

/* {{{ proto array domxml_attr_value()
   Returns list of attribute names */
PHP_FUNCTION(domxml_attr_value)
{
	zval *id;
	xmlAttrPtr attrp;
	
	id = getThis();
	attrp = php_dom_get_object(id, le_domxmlattrp, 0 TSRMLS_CC);
		
	RETURN_STRING((char *) xmlNodeGetContent((xmlNodePtr) attrp), 1);
}
/* }}} */

/* {{{ proto array domxml_attr_specified()
   Returns list of attribute names */
PHP_FUNCTION(domxml_attr_specified)
{
	zval *id;
	xmlAttrPtr attrp;
	
	id = getThis();
	attrp = php_dom_get_object(id, le_domxmlattrp, 0 TSRMLS_CC);
		
	RETURN_TRUE;
}
/* }}} */
/* End of Methods DomAttr }}} */

/* {{{ Methods of Class DomProcessingInstruction */

/* {{{ proto array domxml_pi_target()
   Returns target of pi */
PHP_FUNCTION(domxml_pi_target)
{
	zval *id;
	xmlNodePtr nodep;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlpip, 0 TSRMLS_CC);

	RETURN_STRING((char *)nodep->name, 1);
}
/* }}} */

/* {{{ proto array domxml_pi_data()
   Returns data of pi */
PHP_FUNCTION(domxml_pi_data)
{
	zval *id;
	xmlNodePtr nodep;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlpip, 0 TSRMLS_CC);

	RETURN_STRING(xmlNodeGetContent(nodep), 1);
}
/* }}} */
/* End of Methods of DomProcessingInstruction }}} */

/* {{{ Methods of Class DomCData */

/* {{{ proto array domxml_cdata_length()
   Returns list of attribute names */
PHP_FUNCTION(domxml_cdata_length)
{
	zval *id;
	xmlNodePtr nodep;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlcdatap, 0 TSRMLS_CC);
		
	RETURN_LONG(1);
}
/* }}} */
/* End of Methods DomCDdata }}} */

/* {{{ Methods of Class DomNode */

/* {{{ proto object domxml_node(string name)
   Creates node */
PHP_FUNCTION(domxml_node)
{
	zval *arg, *rv;
	xmlNode *node;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewNode(NULL, Z_STRVAL_P(arg));
	if (!node) {
		RETURN_FALSE;
	}
	rv = php_domobject_new(node, &ret TSRMLS_CC);
	if (!rv) {
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_name()
   Returns name of node */
PHP_FUNCTION(domxml_node_name)
{
	zval *id;
	xmlNode *n;
	const char *str = NULL;
	
	id = getThis();
	n = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	switch (n->type)
	{
	case XML_ELEMENT_NODE:
		 str = n->name;
		 break;
		 
	case XML_TEXT_NODE:
		 str = "#text";
		 break;
		 
    case XML_CDATA_SECTION_NODE:
		 str = "#cdata-section";
		 break;
    case XML_ENTITY_REF_NODE:
		 str = n->name;
		 break;
    case XML_ENTITY_NODE:
		 str = NULL;
		 break;
    case XML_PI_NODE:
		 str = n->name;
		 break;
    case XML_COMMENT_NODE:
		 str = "#comment";
		 break;
    case XML_DOCUMENT_FRAG_NODE:
		 str = "#document-fragment";
		 break;
    default:
		 str = NULL;
		 break;
    }
	RETURN_STRING((char *) str, 1);
}
/* }}} */

/* {{{ proto object domxml_node_value()
   Returns name of value */
PHP_FUNCTION(domxml_node_value)
{
	zval *id;
	xmlNode *n;
	char *str = NULL;
	
	id = getThis();
	n = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	
	if (!n) {
		 RETURN_FALSE;
	}
	switch (n->type)
    {
    case XML_TEXT_NODE:
    case XML_COMMENT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_PI_NODE:
		 str = n->content;
		 break;
    default:
		 str = NULL;
		 break;
    }
	RETURN_STRING(str, 1);
}
/* }}} */

PHP_FUNCTION(domxml_node_type)
{
	zval *id;
	xmlNode *n;
	
	id = getThis();
	n = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	if (!n) {
		RETURN_FALSE;
	}
  RETURN_LONG(n->type);
}
/* }}} */

/* {{{ proto object domxml_node_first_child()
   Returns first child from list of children */
PHP_FUNCTION(domxml_node_first_child)
{
	zval *id, *rv;
	xmlNode *nodep, *first;
	int ret;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	first = nodep->children;
	if (!first) {
		RETURN_FALSE;
	}

	rv = php_domobject_new(first, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_last_child()
   Returns last child from list of children */
PHP_FUNCTION(domxml_node_last_child)
{
	zval *id, *rv;
	xmlNode *nodep, *last;
	int ret;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	last = nodep->last;
	if (!last) {
		RETURN_FALSE;
	}

	rv = php_domobject_new(last, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_next_sibling()
   Returns next child from list of children */
PHP_FUNCTION(domxml_node_next_sibling)
{
	zval *id, *rv;
	xmlNode *nodep, *first;
	int ret;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	first = nodep->next;
	if (!first) {
		RETURN_FALSE;
	}

	rv = php_domobject_new(first, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_previous_sibling()
   Returns previous child from list of children */
PHP_FUNCTION(domxml_node_previous_sibling)
{
	zval *id, *rv;
	xmlNode *nodep, *first;
	int ret;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	first = nodep->prev;
	if (!first) {
		RETURN_FALSE;
	}

	rv = php_domobject_new(first, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_owner_document()
   Returns document this node belongs to */
PHP_FUNCTION(domxml_node_owner_document)
{
	zval *id, *rv;
	xmlNode *nodep;
 	xmlDocPtr	docp;
	int ret;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	docp = nodep->doc;
	if (!docp) {
		RETURN_FALSE;
	}

	rv = php_domobject_new((xmlNodePtr) docp, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_has_child_nodes()
   Returns true if node has children */
PHP_FUNCTION(domxml_node_has_child_nodes)
{
	zval *id;
	xmlNode *nodep;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	if (nodep->children) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object domxml_node_has_attributes()
   Returns true if node has attributes */
PHP_FUNCTION(domxml_node_has_attributes)
{
	zval *id;
	xmlNode *nodep;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	if(nodep->type != XML_ELEMENT_NODE)
		RETURN_FALSE;

	if (nodep->properties) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object domxml_node_prefix()
   Returns namespace prefix of node */
PHP_FUNCTION(domxml_node_prefix)
{
	zval *id;
	xmlNode *nodep;
 	xmlNsPtr	ns;
	
	id = getThis();
	if(NULL == (nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC)))
		RETURN_FALSE;

	ns = nodep->ns;
	if (!ns) {
		RETURN_EMPTY_STRING();
	}

	if(ns->prefix) {
		RETURN_STRING((char *) (ns->prefix), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto object domxml_node_parent()
   Returns parent of node */
PHP_FUNCTION(domxml_node_parent)
{
	zval *id, *rv;
	xmlNode *nodep, *last;
	int ret;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	last = nodep->parent;
	if (!last) {
		RETURN_FALSE;
	}
	rv = php_domobject_new(last, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto array domxml_node_children()
   Returns list of children nodes */
PHP_FUNCTION(domxml_node_children)
{
	zval *id;
	xmlNode *nodep, *last;
	int ret;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, le_domxmldocp TSRMLS_CC);

	/* Even if the nodep is a XML_DOCUMENT_NODE the type is at the
	   same position.
	*/
	if(nodep->type == XML_DOCUMENT_NODE)
		last = ((xmlDoc *) nodep)->children;
	else
		last = nodep->children;
	if (!last) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(last) {
		zval *child;
		child = php_domobject_new(last, &ret TSRMLS_CC);
		add_next_index_zval(return_value, child);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto object domxml_node_unlink_node()
   Deletes node */
PHP_FUNCTION(domxml_node_unlink_node)
{
	zval *id;
	xmlNode *nodep;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	xmlUnlinkNode(nodep);
	xmlFreeNode(nodep);
	zval_dtor(id); /* This is not enough because the children won't be deleted */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object domxml_node_add_child(int domnode)
   Adds existing node to parent node */
PHP_FUNCTION(domxml_node_add_child)
{
	zval *id, *rv, *node;
	xmlNodePtr child, nodep;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &node) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	child = php_dom_get_object(node, le_domxmlnodep, 0 TSRMLS_CC);

	if (!child || !nodep) {
		RETURN_FALSE;
	}

	child = xmlAddChild(nodep, child);
	rv = php_domobject_new(child, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_append_child(int domnode)
   Adds node to list of children */
PHP_FUNCTION(domxml_node_append_child)
{
	zval *id, *rv, *node;
	xmlNodePtr child, nodep;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &node) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	child = php_dom_get_object(node, le_domxmlnodep, 0 TSRMLS_CC);

	if (!child || !nodep) {
		RETURN_FALSE;
	}

	child = xmlAddSibling(nodep, child);
	rv = php_domobject_new(child, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_node_insert_before(int newnode, int refnode)
   Adds node in list of nodes before given node */
PHP_FUNCTION(domxml_node_insert_before)
{
	zval *id, *rv, *node, *ref;
	xmlNodePtr child, nodep, refp;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &node, &ref) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	child = php_dom_get_object(node, le_domxmlnodep, 0 TSRMLS_CC);
	refp = php_dom_get_object(ref, le_domxmlnodep, 0 TSRMLS_CC);

	if (!child || !nodep || !refp) {
		RETURN_FALSE;
	}

	child = xmlAddPrevSibling(refp, child);
	rv = php_domobject_new(child, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto bool domxml_node_set_name(string name)
   Sets name of a node */
PHP_FUNCTION(domxml_node_set_name)
{
	zval *id, *name;
	xmlNode *nodep;

	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	convert_to_string(name);

	if(Z_STRLEN_P(name))
		xmlNodeSetName(nodep, Z_STRVAL_P(name));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array domxml_node_attributes()
   Returns list of attributes of node */
PHP_FUNCTION(domxml_node_attributes)
{
	zval *id, *attrs;
	xmlNode *nodep;
#ifdef oldstyle_for_libxml_1_8_7
	xmlAttr *attr;
#endif
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	if(node_attributes(&attrs, nodep TSRMLS_CC) < 0)
		RETURN_FALSE;

	*return_value = *attrs;
	FREE_ZVAL(attrs);

#ifdef oldstyle_for_libxml_1_8_7
	attr = nodep->properties;
	if (!attr) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(attr) {
		add_assoc_string(return_value, (char *) attr->name, xmlNodeGetContent(attr), 1);
		attr = attr->next;
	}
#endif
}
/* }}} */

/* {{{ proto object domxml_new_child(string name, string content)
   Adds child node to parent node */
PHP_FUNCTION(domxml_node_new_child)
{
	zval *id, *name, *content, *rv;
	xmlNodePtr child, nodep;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &name, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);

	convert_to_string(name);
	convert_to_string(content);

	if(Z_STRLEN_P(content))
		child = xmlNewChild(nodep, NULL, Z_STRVAL_P(name), Z_STRVAL_P(content));
	else
		child = xmlNewChild(nodep, NULL, Z_STRVAL_P(name), NULL);
	if (!child) {
		RETURN_FALSE;
	}
	rv = php_domobject_new(child, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto bool domxml_node_set_content(string content)
   Set content of a node */
PHP_FUNCTION(domxml_node_set_content)
{
	zval *id, *content;
	xmlNode *nodep;

	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	convert_to_string(content);

	if(Z_STRLEN_P(content))
		xmlNodeSetContent(nodep, Z_STRVAL_P(content));

	/* FIXME: Actually the property 'content' of the node has to be updated
	   as well. Since 'content' should disappear sooner or later and being
	   replaces by a function 'content()' I skip this for now
	*/
	RETURN_TRUE;
}

/* End of Methods DomNode }}} */

/* {{{ Methods of Class DomNotation */

/* {{{ proto string domxml_notation_public_id()
   Returns public id of notation node */
PHP_FUNCTION(domxml_notation_public_id)
{
	zval *id;
	xmlNotationPtr nodep;
	
	id = getThis();
	nodep = (xmlNotationPtr) php_dom_get_object(id, le_domxmlnotationp, 0 TSRMLS_CC);

	RETURN_STRING((char *) (nodep->PublicID), 1);
}
/* }}} */

/* {{{ proto string domxml_notation_system_id()
   Returns system id of notation node */
PHP_FUNCTION(domxml_notation_system_id)
{
	zval *id;
	xmlNotationPtr nodep;
	
	id = getThis();
	nodep = (xmlNotationPtr) php_dom_get_object(id, le_domxmlnotationp, 0 TSRMLS_CC);

	RETURN_STRING((char *) (nodep->SystemID), 1);
}
/* }}} */

/* End of Methods DomNotation }}} */

/* {{{ Methods of Class DomElement */

/* {{{ proto object domxml_element(string name)
   Constructor of DomElement */
PHP_FUNCTION(domxml_element)
{
	zval *arg, *rv;
	xmlNode *node;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewNode(NULL, Z_STRVAL_P(arg));
	if (!node) {
		RETURN_FALSE;
	}
	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string domxml_elem_tagname()
   Returns tag name of element node */
PHP_FUNCTION(domxml_elem_tagname)
{
	zval *id;
	xmlNode *nodep;
	
	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);

	RETURN_STRING((char *) (nodep->name), 1);
}
/* }}} */

/* {{{ proto string domxml_elem_get_attribute(string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_elem_get_attribute)
{
	zval *id, *arg1;
	xmlNode *nodep;
	char *value;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);

	value = xmlGetProp(nodep, Z_STRVAL_P(arg1));
	if (!value) {
		RETURN_EMPTY_STRING();
	} else {
		RETURN_STRING(value, 1);
	}
}
/* }}} */

/* {{{ proto bool domxml_elem_set_attribute(string attrname, string value)
   Sets value of given attribute */
PHP_FUNCTION(domxml_elem_set_attribute)
{
	zval *id, *rv, *arg1, *arg2;
	xmlNode *nodep;
	xmlAttr *attr;
	int ret;

	if ((ZEND_NUM_ARGS() == 2) && getParameters(ht, 2, &arg1, &arg2) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);
	convert_to_string(arg2);

	attr = xmlSetProp(nodep, Z_STRVAL_P(arg1), Z_STRVAL_P(arg2));
	if (!attr) {
		php_error(E_WARNING, "No such attribute '%s'", Z_STRVAL_P(arg1));
		RETURN_FALSE;
	}
	rv = php_domobject_new((xmlNodePtr) attr, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string domxml_elem_remove_attribute(string attrname)
   Remove given attribute */
PHP_FUNCTION(domxml_elem_remove_attribute)
{
	zval *id, *arg1;
	xmlNode *nodep;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);

	/* FIXME: not implemented */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_elem_get_attribute_node(string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_elem_get_attribute_node)
{
	zval *id, *arg1;
	xmlNode *nodep;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);

	/* FIXME: not implemented */

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_elem_set_attribute_node(int attr)
   Sets value of given attribute */
PHP_FUNCTION(domxml_elem_set_attribute_node)
{
	zval *id, *arg1;
	xmlNode *nodep;
	xmlAttr *attrp;

	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
		attrp = php_dom_get_object(arg1, le_domxmlattrp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}

	/* FIXME: not implemented */

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_elem_get_element_by_tagname(string tagname)
   Returns element for given attribute */
PHP_FUNCTION(domxml_elem_get_element_by_tagname)
{
	zval *id, *arg1;
	xmlNode *nodep;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);

	/* FIXME: not implemented */

}
/* }}} */
/* End of Methods DomElement }}} */

/* {{{ Methods of Class DomDocumentType */

/* {{{ proto array domxml_doctype_name()
   Return name of DocumentType */
PHP_FUNCTION(domxml_doctype_name)
{
	zval *id;
	xmlNodePtr attrp;
	
	id = getThis();
	attrp = php_dom_get_object(id, le_domxmldoctypep, 0 TSRMLS_CC);
		
	RETURN_STRING((char *) (attrp->name), 1);
}
/* }}} */

/* End of Methods DomElementType }}} */

/* {{{ Methods of Class DomDocument */

/* {{{ proto object domxml_doc_doctype()
   Returns DomDocumentType */
PHP_FUNCTION(domxml_doc_doctype)
{
	zval *id, *rv;
	xmlDtdPtr dtd;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	dtd = xmlGetIntSubset(docp);
	rv = php_domobject_new((xmlNodePtr) dtd, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_implementation()
   Returns DomeDOMImplementation */
PHP_FUNCTION(domxml_doc_implementation)
{
	zval *id;
	xmlDocPtr docp;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

/*	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
*/
}
/* }}} */

/* {{{ proto array domxml_doc_document_element()
   Returns root node of document */
PHP_FUNCTION(domxml_doc_document_element)
{
	zval *id;
	xmlDoc *docp;
	xmlNode *node;
	int ret;
	
	id = getThis();
	
	if (!id) {
		if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
			RETURN_FALSE;
		}
	}

	docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC);

	node = docp->children;
	if (!node) {
		RETURN_FALSE;
	}

	while(node) {
		if(node->type == XML_ELEMENT_NODE) {
			zval *rv;
			rv = php_domobject_new(node, &ret TSRMLS_CC);
			SEPARATE_ZVAL(&rv);
			*return_value = *rv;
			FREE_ZVAL(rv);
			return;
		}
		node = node->next;
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_element(string name)
   Creates new element node */
PHP_FUNCTION(domxml_doc_create_element)
{
	zval *arg, *id, *rv;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewNode(NULL, Z_STRVAL_P(arg));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_create_text_node(string name)
   Creates new text node */
PHP_FUNCTION(domxml_doc_create_text_node)
{
	zval *arg, *id, *rv;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewText(Z_STRVAL_P(arg));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_create_comment(string name)
   Creates new comment node */
PHP_FUNCTION(domxml_doc_create_comment)
{
	zval *arg, *id, *rv;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewComment(Z_STRVAL_P(arg));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_create_attribute(string name)
   Creates new attribute node */
PHP_FUNCTION(domxml_doc_create_attribute)
{
	zval *arg1, *arg2, *id, *rv;
	xmlAttrPtr node;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_string(arg2);

	node = xmlNewProp(NULL, Z_STRVAL_P(arg1), Z_STRVAL_P(arg2));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	rv = php_domobject_new((xmlNodePtr) node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_create_processing_instruction(string name)
   Creates new processing_instruction node */
PHP_FUNCTION(domxml_doc_create_processing_instruction)
{
	zval *arg1, *arg2, *id, *rv;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_string(arg2);

	node = xmlNewPI(Z_STRVAL_P(arg1), Z_STRVAL_P(arg2));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_doc_imported_node(int node, bool recursive)
   Creates new element node */
PHP_FUNCTION(domxml_doc_imported_node)
{
	zval *arg1, *arg2, *id, *rv;
	xmlNodePtr node, srcnode;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	srcnode =  php_dom_get_object(arg1, le_domxmlnodep, 0 TSRMLS_CC);
	if(!srcnode)
		RETURN_FALSE;

	convert_to_long(arg2);

	node = xmlCopyNode(srcnode, Z_LVAL_P(arg2));
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp; /* Not enough because other nodes in the tree are not set */

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_dtd([int doc_handle])
   Returns DTD of document */
PHP_FUNCTION(domxml_intdtd)
{
	zval *id, *rv;
	xmlDoc *docp;
	xmlDtd *dtd;
	int ret;
	
	id = getThis();
	docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC);

	dtd = xmlGetIntSubset(docp);
	if (!dtd) {
		RETURN_FALSE;
	}
	rv = php_domobject_new((xmlNodePtr) dtd, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string domxml_dumpmem([int doc_handle])
   Dumps document into string */
PHP_FUNCTION(domxml_dumpmem)
{
	zval *id;
	xmlDoc *docp;
	xmlChar *mem;
	int size;
	
	id = getThis();
	if(NULL == (docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC))) {
		RETURN_FALSE;
	}

	xmlDocDumpMemory(docp, &mem, &size);
	if (!size) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(mem, size, 1);
}
/* }}} */

/* {{{ proto object xmldoc(string xmldoc)
   Creates DOM object of XML document */
PHP_FUNCTION(xmldoc)
{
	zval **argv[2], *rv;
	int argc = ZEND_NUM_ARGS();
	xmlDoc *docp;
	int ret;

	if(argc > 2)
		WRONG_PARAM_COUNT;
	if(zend_get_parameters_array_ex(argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(argv[0]);
	if(argc == 2) {
		convert_to_boolean_ex(argv[1]);
		if(Z_BVAL_PP(argv[1])) {
			docp = xmlParseFile(Z_STRVAL_PP(argv[0]));
		} else {
			docp = xmlParseDoc(Z_STRVAL_PP(argv[0]));
		}
	} else {
		docp = xmlParseDoc(Z_STRVAL_PP(argv[0]));
	}
	if (!docp)
		RETURN_FALSE;
	rv = php_domobject_new((xmlNodePtr) docp, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object xmldocfile(string filename)
   Creates DOM object of XML document in file*/
PHP_FUNCTION(xmldocfile)
{
	zval *arg, *rv;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlParseFile(Z_STRVAL_P(arg));
	if (!docp) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(docp, le_domxmldocp);

	rv = php_domobject_new((xmlNodePtr) docp, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	add_property_resource(return_value, "doc", ret);
	if(docp->name)
		add_property_stringl(return_value, "name", (char *) docp->name, strlen(docp->name), 1);
	if(docp->URL)
		add_property_stringl(return_value, "url", (char *) docp->URL, strlen(docp->URL), 1);
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	if(docp->encoding)
		add_property_stringl(return_value, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
	add_property_long(return_value, "standalone", docp->standalone);
	add_property_long(return_value, "type", docp->type);
	add_property_long(return_value, "compression", docp->compression);
	add_property_long(return_value, "charset", docp->charset);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto bool domxml_node_text_concat(string content)
   Add string tocontent of a node */
PHP_FUNCTION(domxml_node_text_concat)
{
	zval *id, *content;
	xmlNode *nodep;

	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlnodep, 0 TSRMLS_CC);
	convert_to_string(content);

	if(Z_STRLEN_P(content))
		xmlTextConcat(nodep, Z_STRVAL_P(content), Z_STRLEN_P(content));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object domxml_add_root(string name)
   Adds root node to document */
PHP_FUNCTION(domxml_add_root)
{
	zval *id, *name, *rv;
	xmlDoc *docp;
	xmlNode *nodep;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &name)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();
	docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC);
	convert_to_string(name);

	nodep = xmlNewDocNode(docp, NULL, Z_STRVAL_P(name), NULL);
	if (!nodep) {
		RETURN_FALSE;
	}
	
	xmlDocSetRootElement(docp, nodep); 
	rv = php_domobject_new(nodep, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_new_xmldoc(string version)
   Creates new xmldoc */
PHP_FUNCTION(domxml_new_xmldoc)
{
	zval *arg, *rv;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlNewDoc(Z_STRVAL_P(arg));
	if (!docp) {
		RETURN_FALSE;
	}
	rv = php_domobject_new((xmlNodePtr) docp, &ret TSRMLS_CC);
	if (!rv) {
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

#ifdef newcode
/* {{{ proto int node_namespace([int node])
   Returns list of namespaces */
static int node_namespace(zval **attributes, xmlNode *nodep TSRMLS_DC)
{
	xmlNs *ns;

	/* Get the children of the current node */	
	ns = nodep->ns;
	if (!ns) {
		return -1;
	}

	/* create an php array for the children */
	MAKE_STD_ZVAL(*attributes);
	if (array_init(*attributes) == FAILURE) {
		return -1;
	}

	while(ns) {
		zval *pattr;
		int ret;

		pattr = php_domobject_new((xmlNodePtr) ns, &ret TSRMLS_CC);
		SEPARATE_ZVAL(&pattr);
/*		if(!ret) { */
			if(ns->href)
				add_property_stringl(pattr, "href", (char *) ns->href, strlen(ns->href), 1);
			if(ns->prefix)
				add_property_stringl(pattr, "prefix", (char *) ns->prefix, strlen(ns->prefix), 1);
			add_property_long(pattr, "type", ns->type);
/*		} */
		zend_hash_next_index_insert(Z_ARRVAL_PP(attributes), &pattr, sizeof(zval *), NULL);
		ns = ns->next;
	}
	return 0;
}
#endif

/* {{{ proto int node_attributes(zval **attributes, int node)
   Returns list of children nodes */
static int node_attributes(zval **attributes, xmlNode *nodep TSRMLS_DC)
{
	xmlAttr *attr;
	int count = 0;

	/* Get the children of the current node */	
	if(nodep->type != XML_ELEMENT_NODE)
		return -1;
	attr = nodep->properties;
	if (!attr)
		return -1;

	/* create an php array for the children */
	MAKE_STD_ZVAL(*attributes);
	array_init(*attributes);

	while(attr) {
		zval *pattr;
		int ret;

		pattr = php_domobject_new((xmlNodePtr) attr, &ret TSRMLS_CC);
		/** XXX FIXME XXX */
/*		if(0 <= (n = node_children(&children, attr->children TSRMLS_CC))) {
			zend_hash_update(Z_OBJPROP_P(value), "children", sizeof("children"), (void *) &children, sizeof(zval *), NULL);
		}
*/		add_property_string(pattr, "name", (char *) (attr->name), 1);
		add_property_string(pattr, "value", xmlNodeGetContent((xmlNodePtr) attr), 1);
		zend_hash_next_index_insert(Z_ARRVAL_PP(attributes), &pattr, sizeof(zval *), NULL);
		attr = attr->next;
		count++;
	}
	return count;
}

/* {{{ proto int node_children([int node])
   Returns list of children nodes */
static int node_children(zval **children, xmlNode *nodep TSRMLS_DC)
{
	zval *mchildren, *attributes;
	/* zval *namespace; */
	xmlNode *last;
	int count = 0;

	/* Get the children of the current node */	
	last = nodep;
	if (!last) {
		return -1;
	}

	/* create an php array for the children */
	MAKE_STD_ZVAL(*children);
	array_init(*children);

	while(last) {
		zval *child;
		int ret;
	
		child = php_domobject_new(last, &ret TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_PP(children), &child, sizeof(zval *), NULL);

		/* Get the namespace of the current node and add it as a property */
		/* XXX FIXME XXX */
/*		if(!node_namespace(&namespace, last))
			zend_hash_update(Z_OBJPROP_P(child), "namespace", sizeof("namespace"), (void *) &namespace, sizeof(zval *), NULL);
*/

		/* Get the attributes of the current node and add it as a property */
		if(node_attributes(&attributes, last TSRMLS_CC) >= 0)
			zend_hash_update(Z_OBJPROP_P(child), "attributes", sizeof("attributes"), (void *) &attributes, sizeof(zval *), NULL);

		/* Get recursively the children of the current node and add it as a property */
		if(node_children(&mchildren, last->children TSRMLS_CC) >= 0)
			zend_hash_update(Z_OBJPROP_P(child), "children", sizeof("children"), (void *) &mchildren, sizeof(zval *), NULL);
		count++;
		last = last->next;
	}
	return count;
}
/* }}} */

/* {{{ proto object xmltree(string xmltree)
   Create a tree of PHP objects from an XML document */
PHP_FUNCTION(xmltree)
{
	zval *arg, *children, *rv;
	xmlDoc *docp;
	xmlNode *root;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Create a new xml document */
	docp = xmlParseDoc(Z_STRVAL_P(arg));
	if (!docp) {
		RETURN_FALSE;
	}

	/* get the root and add as a property to the document */
	root = docp->children;
	if (!root) {
		xmlFreeDoc(docp);
		RETURN_FALSE;
	}

	rv = php_domobject_new((xmlNodePtr) docp, &ret TSRMLS_CC);
	if (!rv) {
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	/* The root itself maybe an array. Though you may not have two Elements
	   as root, you may have a comment, pi and and element as root.
	   Thanks to Paul DuBois for pointing me at this.
	*/
	if (node_children(&children, root TSRMLS_CC) >= 0) {
		zend_hash_update(Z_OBJPROP_P(return_value), "children",
						 sizeof("children"), (void *) &children, sizeof(zval *), NULL);
	}
/*	xmlFreeDoc(docp); */
}
/* }}} */

#if defined(LIBXML_XPATH_ENABLED)
/* {{{ proto bool xpath_init(void)
   Initializing XPath environment */
PHP_FUNCTION(xpath_init)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	xmlXPathInit();
	RETURN_TRUE;
}
/* }}} */

static void php_xpathptr_new_context(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *id, *rv;
	xmlXPathContextPtr ctx;
	xmlDocPtr docp;
	int ret;
	
	id = getThis();
	docp = php_dom_get_object(id, le_domxmldocp, 0 TSRMLS_CC);
		
#if defined(LIBXML_XPTR_ENABLED)
	if(mode == PHP_XPTR)
		ctx = xmlXPtrNewContext(docp, NULL, NULL);
	else
#endif
		ctx = xmlXPathNewContext(docp);
	if (!ctx) {
		RETURN_FALSE;
	}
	rv = php_xpathcontext_new(ctx, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string xpath_new_context([int doc_handle])
   Create new XPath context */
PHP_FUNCTION(xpath_new_context)
{
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH);
}
/* }}} */

/* {{{ proto string xptr_new_context([int doc_handle])
   Create new XPath context */
PHP_FUNCTION(xptr_new_context)
{
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR);
}
/* }}} */

/* {{{ */
static void php_xpathptr_eval(INTERNAL_FUNCTION_PARAMETERS, int mode, int expr)
{
	zval *id, *str, *rv, *contextnode;
	xmlXPathContextPtr ctxp;
	xmlXPathObjectPtr xpathobjp;
	xmlNode *contextnodep;
	int ret;
	
	contextnode = NULL;
	contextnodep = NULL;

	id = getThis();
	
	if (! id) {
		switch(ZEND_NUM_ARGS()) {
			case 2:
				if ((getParameters(ht, 2, &id, &str)) == FAILURE) 
				{
					WRONG_PARAM_COUNT;
				}
				break;
				
			case 3:
				if ((getParameters(ht, 3, &id, &str, &contextnode)) == FAILURE) 
				{
					WRONG_PARAM_COUNT;
				}
				break;
				
			default:
				WRONG_PARAM_COUNT;
		}
	}
	
	
	ctxp = php_xpath_get_context(id, le_xpathctxp, 0 TSRMLS_CC);
	convert_to_string(str);
	
	if (contextnode) {
		contextnodep = php_dom_get_object(contextnode, le_domxmlnodep, 0 TSRMLS_CC);
	}
	ctxp->node = contextnodep;

#if defined(LIBXML_XPTR_ENABLED)
	if(mode == PHP_XPTR) {
		xpathobjp = xmlXPtrEval(BAD_CAST Z_STRVAL_P(str), ctxp);
	} else{
#endif
		if(expr)
			xpathobjp = xmlXPathEvalExpression(Z_STRVAL_P(str), ctxp);
		else
			xpathobjp = xmlXPathEval(Z_STRVAL_P(str), ctxp);
#if defined(LIBXML_XPTR_ENABLED)
	}
#endif

	ctxp->node = NULL;
	if (!xpathobjp) {
		RETURN_FALSE;
	}

	rv = php_xpathobject_new(xpathobjp, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);

	add_property_long(rv, "type", xpathobjp->type);
	switch(xpathobjp->type) {
		case XPATH_UNDEFINED:
			break;
		case XPATH_NODESET: {
			int i;
			zval *arr;
			xmlNodeSetPtr nodesetp;

			MAKE_STD_ZVAL(arr);
			if (array_init(arr) == FAILURE) {
				zval_dtor(rv);
				RETURN_FALSE;
			}
			if(NULL == (nodesetp = xpathobjp->nodesetval)) {
				zval_dtor(rv);
				RETURN_FALSE;
			}

			for(i = 0;i < nodesetp->nodeNr;i++) {
				xmlNodePtr node = nodesetp->nodeTab[i];
				zval *child;
				int retnode;

				/* construct a node object */
				child = php_domobject_new(node, &retnode TSRMLS_CC);
				zend_hash_next_index_insert(Z_ARRVAL_P(arr), &child, sizeof(zval *), NULL);
			}
			zend_hash_update(Z_OBJPROP_P(rv), "nodeset", sizeof("nodeset"), (void *) &arr, sizeof(zval *), NULL);
			break;
		}
		case XPATH_BOOLEAN:
			add_property_bool(rv, "value", xpathobjp->boolval);
			break;
		case XPATH_NUMBER:
			add_property_double(rv, "value", xpathobjp->floatval);
			break;
		case XPATH_STRING:
			add_property_string(rv, "value", xpathobjp->stringval, 1);
			break;
		case XPATH_POINT:
			break;
		case XPATH_RANGE:
			break;
		case XPATH_LOCATIONSET:
			break;
		case XPATH_USERS:
			break;
		case XPATH_XSLT_TREE:
			break;
	}
	*return_value = *rv;
}
/* }}} */

/* {{{ proto int xpath_eval([int xpathctx_handle, ] string str)
   Evaluate the XPath Location Path in the given string */
PHP_FUNCTION(xpath_eval)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 0);
}
/* }}} */

/* {{{ proto int xpath_eval_expression([int xpathctx_handle, ] string str)
   Evaluate the XPath Location Path in the given string */
PHP_FUNCTION(xpath_eval_expression)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 1);
}
/* }}} */
#endif /* defined(LIBXML_XPATH_ENABLED) */

#if defined(LIBXML_XPTR_ENABLED)
/* {{{ proto int xptr_eval([int xpathctx_handle, ] string str)
   Evaluate the XPtr Location Path in the given string */
PHP_FUNCTION(xptr_eval)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR, 0);
}
/* }}} */
#endif /* LIBXML_XPTR_ENABLED */

/* {{{ proto string domxml_version()
   Dumps document into string */
PHP_FUNCTION(domxml_version)
{
	
	RETURN_STRING(LIBXML_DOTTED_VERSION, 1);
}
/* }}} */

#endif /* HAVE_DOMXML */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
