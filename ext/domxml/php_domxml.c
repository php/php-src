/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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


#include "php.h"
#include "php_domxml.h"

#if HAVE_DOMXML
#include "ext/standard/info.h"
/*#define newcode*/
#define PHP_XPATH 1
#define PHP_XPTR 2

static int le_domxmldocp;
static int le_domxmldtdp;
static int le_domxmlnodep;
static int le_domxmlattrp;
/* static int le_domxmlnsp; */

#if defined(LIBXML_XPATH_ENABLED)
static int le_xpathctxp;
static int le_xpathobjectp;
static zend_class_entry *xpathctx_class_entry_ptr;
static zend_class_entry *xpathobject_class_entry_ptr;
#endif

static zend_class_entry *domxmldoc_class_entry_ptr;
static zend_class_entry *domxmldtd_class_entry_ptr;
static zend_class_entry *domxmlnode_class_entry_ptr;
static zend_class_entry *domxmlattr_class_entry_ptr;
static zend_class_entry *domxmlns_class_entry_ptr;

#ifdef newcode
static zend_class_entry *domxmltestnode_class_entry_ptr;
#endif

static int node_attributes(zval **attributes, xmlNode *nodep);
static int node_children(zval **children, xmlNode *nodep);

static zend_function_entry domxml_functions[] = {
	PHP_FE(xmldoc,	NULL)
	PHP_FE(xmldocfile,	NULL)
	PHP_FE(xmltree,	NULL)
	PHP_FE(domxml_root,	NULL)
	PHP_FE(domxml_add_root,	NULL)
	PHP_FE(domxml_dumpmem,	NULL)
	PHP_FE(domxml_attributes,	NULL)
	PHP_FE(domxml_get_attribute,	NULL)
	PHP_FALIAS(domxml_getattr,	domxml_get_attribute, NULL)
	PHP_FE(domxml_set_attribute,	NULL)
	PHP_FALIAS(domxml_setattr,	domxml_set_attribute, NULL)
	PHP_FE(domxml_children,	NULL)
	PHP_FE(domxml_new_child,	NULL)
	PHP_FE(domxml_node,	NULL)
	PHP_FE(domxml_unlink_node,	NULL)
	PHP_FE(domxml_set_content,	NULL)
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


static zend_function_entry php_domxmldoc_class_functions[] = {
	PHP_FALIAS(root,	domxml_root,	NULL)
	PHP_FALIAS(children,	domxml_children,	NULL)
	PHP_FALIAS(add_root,	domxml_add_root,	NULL)
	PHP_FALIAS(dtd,	domxml_intdtd,	NULL)
	PHP_FALIAS(dumpmem,	domxml_dumpmem,	NULL)
#if defined(LIBXML_XPATH_ENABLED)
	PHP_FALIAS(xpath_init, xpath_init, NULL)
	PHP_FALIAS(xpath_new_context, xpath_new_context, NULL)
	PHP_FALIAS(xptr_new_context, xpath_new_context, NULL)
#endif
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnode_class_functions[] = {
	PHP_FALIAS(lastchild,	domxml_last_child,	NULL)
	PHP_FALIAS(last_child,	domxml_last_child,	NULL)
	PHP_FALIAS(children,	domxml_children,	NULL)
	PHP_FALIAS(parent,	domxml_parent,		NULL)
	PHP_FALIAS(new_child,	domxml_new_child,		NULL)
	PHP_FALIAS(getattr,	domxml_get_attribute,		NULL)
	PHP_FALIAS(get_attribute,	domxml_get_attribute,		NULL)
	PHP_FALIAS(setattr,	domxml_set_attribute,		NULL)
	PHP_FALIAS(set_attribute,	domxml_set_attribute,		NULL)
	PHP_FALIAS(attributes,	domxml_attributes,	NULL)
	PHP_FALIAS(node,	domxml_node,	NULL)
	PHP_FALIAS(unlink,	domxml_unlink_node,	NULL)
	PHP_FALIAS(set_content,	domxml_set_content,	NULL)
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

#ifdef newcode
static zend_function_entry php_domxmltestnode_class_functions[] = {
	PHP_FE(domxml_test,	NULL)
	{NULL, NULL, NULL}
};
#endif

static zend_function_entry php_domxmlattr_class_functions[] = {
	PHP_FALIAS(name,	domxml_attrname,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlns_class_functions[] = {
	{NULL, NULL, NULL}
};

#ifdef newcode
void domxmltestnode_class_startup();
#endif

zend_module_entry domxml_module_entry = {
	"domxml", domxml_functions, PHP_MINIT(domxml), NULL, NULL, NULL, PHP_MINFO(domxml), STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DOMXML
ZEND_GET_MODULE(domxml)
#endif

static void php_free_xml_doc(zend_rsrc_list_entry *rsrc)
{
	xmlDoc *doc = (xmlDoc *)rsrc->ptr;
	xmlFreeDoc(doc);
}

void _free_node(zend_rsrc_list_entry *rsrc) {
/*fprintf(stderr, "Freeing node: %s\n", tmp->name);*/
}

#if defined(LIBXML_XPATH_ENABLED)
static void php_free_xpath_context(zend_rsrc_list_entry *rsrc)
{
	xmlXPathContextPtr ctx = (xmlXPathContextPtr)rsrc->ptr;
	xmlXPathFreeContext(ctx);
}

static void php_free_xpath_object(zend_rsrc_list_entry *rsrc)
{
	xmlXPathObjectPtr obj = (xmlXPathObjectPtr)rsrc->ptr;
	xmlXPathFreeObject(obj);
}
#endif

PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry domxmldoc_class_entry;
	zend_class_entry domxmldtd_class_entry;
	zend_class_entry domxmlnode_class_entry;
	zend_class_entry domxmlattr_class_entry;
	zend_class_entry domxmlns_class_entry;
#if defined(LIBXML_XPATH_ENABLED)
	zend_class_entry xpathctx_class_entry;
	zend_class_entry xpathobject_class_entry;
#endif

#ifdef newcode
  domxmltestnode_class_startup();
#endif

	le_domxmldocp = zend_register_list_destructors_ex(php_free_xml_doc, NULL, "domxml document", module_number);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	*/
	le_domxmlnodep = zend_register_list_destructors_ex(_free_node, NULL, "domxml node", module_number);
	le_domxmlattrp = zend_register_list_destructors_ex(NULL, NULL, "domxml attribute", module_number);
#if defined(LIBXML_XPATH_ENABLED)
	le_xpathctxp = zend_register_list_destructors_ex(php_free_xpath_context, NULL, "xpath context", module_number);
	le_xpathobjectp = zend_register_list_destructors_ex(php_free_xpath_object, NULL, "xpath object", module_number);
#endif

/*	le_domxmlnsp = register_list_destructors(NULL, NULL); */

	INIT_CLASS_ENTRY(domxmldoc_class_entry, "DomDocument", php_domxmldoc_class_functions);
	INIT_CLASS_ENTRY(domxmldtd_class_entry, "Dtd", php_domxmldtd_class_functions);
	INIT_CLASS_ENTRY(domxmlnode_class_entry, "DomNode", php_domxmlnode_class_functions);
	INIT_CLASS_ENTRY(domxmlattr_class_entry, "DomAttribute", php_domxmlattr_class_functions);
	INIT_CLASS_ENTRY(domxmlns_class_entry, "DomNamespace", php_domxmlns_class_functions);
#if defined(LIBXML_XPATH_ENABLED)
	INIT_CLASS_ENTRY(xpathctx_class_entry, "XPathContext", php_xpathctx_class_functions);
	INIT_CLASS_ENTRY(xpathobject_class_entry, "XPathObject", php_xpathobject_class_functions);
#endif

	domxmldoc_class_entry_ptr = zend_register_internal_class(&domxmldoc_class_entry);
	domxmldtd_class_entry_ptr = zend_register_internal_class(&domxmldtd_class_entry);
	domxmlnode_class_entry_ptr = zend_register_internal_class(&domxmlnode_class_entry);
	domxmlattr_class_entry_ptr = zend_register_internal_class(&domxmlattr_class_entry);
	domxmlns_class_entry_ptr = zend_register_internal_class(&domxmlns_class_entry);
#if defined(LIBXML_XPATH_ENABLED)
	xpathctx_class_entry_ptr = zend_register_internal_class(&xpathctx_class_entry);
	xpathobject_class_entry_ptr = zend_register_internal_class(&xpathobject_class_entry);
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

#ifdef newcode
/* {{{ Node Class */
pval domxmltestnode_class_get_property(zend_property_reference *property_reference)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;


	printf("Reading a property from a OverloadedTestClass object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
		
	result.value.str.val = estrndup("testing", 7);
	result.value.str.len = 7;
	result.type = IS_STRING;
	return result;
}


int domxmltestnode_class_set_property(zend_property_reference *property_reference, pval *value)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a OverloadedTestClass object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
		
	return 0;
}

void domxmltestnode_class_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	pval *object = property_reference->object;
	zend_llist_element *element;


	printf("Invoking a method on OverloadedTestClass object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
			case OE_IS_METHOD: {
				pval *object_handle;
				printf("Overloaded method:  ");
				PHP_FN(xmltree)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
				if (zend_is_true(return_value)) {
					var_reset(object);
					return;
				}
				ALLOC_ZVAL(object_handle);
				*object_handle = *return_value;
				pval_copy_constructor(object_handle);
				INIT_PZVAL(object_handle);
				zend_hash_index_update(object->value.obj.properties, 0, &object_handle, sizeof(pval *), NULL);
				pval_destructor(&overloaded_property->element);
			}
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
		
	printf("%d arguments\n", ZEND_NUM_ARGS());
	return_value->value.str.val = estrndup("testing", 7);
	return_value->value.str.len = 7;
	return_value->type = IS_STRING;
}


void domxmltestnode_class_startup()
{
	zend_class_entry domxmltestnode_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(domxmltestnode_class_entry, "TestNode",
	              php_domxmltestnode_class_functions,
								domxmltestnode_class_call_function,
								domxmltestnode_class_get_property,
								domxmltestnode_class_set_property);

	domxmltestnode_class_entry_ptr = zend_register_internal_class(&domxmltestnode_class_entry);
}
#endif 

/* {{{ proto int domxml_test(int id)
   Unity function for testing */
PHP_FUNCTION(domxml_test)
{
	zval *id;
	
	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(id);
	RETURN_LONG(id->value.lval);
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

/* {{{ proto array domxml_attrname([int dir_handle])
   Returns list of attribute objects */
PHP_FUNCTION(domxml_attrname)
{
	zval *id, **tmp;
	xmlNodePtr nodep;
	xmlAttr *attr;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}
		
	attr = nodep->properties;
	if (!attr) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(attr) {
		zval *pattr;
		ret = zend_list_insert(attr, le_domxmlattrp);

		/* construct an object with some methods */
		object_init_ex(pattr, domxmlattr_class_entry_ptr);
		add_property_resource(pattr, "attribute", ret);
		add_property_stringl(pattr, "name", (char *) attr->name, strlen(attr->name), 1);
/*		add_property_stringl(pattr, "content", (char *) attr->val->content, strlen(attr->val->content), 1); */
		zend_hash_next_index_insert(return_value->value.ht, &pattr, sizeof(zval *), NULL);
		attr = attr->next;
	}
}
/* }}} */

/* {{{ proto object domxml_node(string name)
   Creates node */
PHP_FUNCTION(domxml_node)
{
	zval *arg;
	xmlNode *node;
	xmlChar *content;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	node = xmlNewNode(NULL, arg->value.str.val);
	if (!node) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(node, le_domxmlnodep);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmlnode_class_entry_ptr);
	add_property_resource(return_value, "node", ret);
	add_property_long(return_value, "type", node->type);
	add_property_stringl(return_value, "name", (char *) node->name, strlen(node->name), 1);
	content = xmlNodeGetContent(node);
	if(content)
		add_property_stringl(return_value, "content", (char *) content, strlen(content), 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto object domxml_last_child([int node])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_last_child)
{
	zval *id, **tmp;
	xmlNode *nodep, *last;
	xmlChar *content;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}

	last = nodep->last;
	if (!last) {
		RETURN_FALSE;
	}

	ret = zend_list_insert(last, le_domxmlnodep);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmlnode_class_entry_ptr);
	add_property_resource(return_value, "node", ret);
	add_property_long(return_value, "type", last->type);
	add_property_stringl(return_value, "name", (char *) last->name, strlen(last->name), 1);
	content = xmlNodeGetContent(last);
	if(content)
		add_property_stringl(return_value, "content", (char *) content, strlen(content), 1);
}
/* }}} */

/* {{{ proto object domxml_parent([int node])
   Returns parent of node */
PHP_FUNCTION(domxml_parent)
{
	zval *id, **tmp;
	xmlNode *nodep, *last;
	xmlChar *content;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}

	last = nodep->parent;
	if (!last) {
		RETURN_FALSE;
	}

	ret = zend_list_insert(last, le_domxmlnodep);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmlnode_class_entry_ptr);
	add_property_resource(return_value, "node", ret);
	add_property_long(return_value, "type", last->type);
	add_property_stringl(return_value, "name", (char *) last->name, strlen(last->name), 1);
	content = xmlNodeGetContent(last);
	if(content)
		add_property_stringl(return_value, "content", (char *) content, strlen(content), 1);
}
/* }}} */

/* {{{ proto array domxml_children([int node])
   Returns list of children nodes */
PHP_FUNCTION(domxml_children)
{
	zval *id, **tmp;
	xmlNode *nodep, *last;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
					php_error(E_WARNING, "unable to find my handle property");
					RETURN_FALSE;
				} else {
					ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
				}
			} else {
				ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
			}
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if ((zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) &&
		    (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE)) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}
		
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
		xmlChar *content;
		MAKE_STD_ZVAL(child);

		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object */
		object_init_ex(child, domxmlnode_class_entry_ptr);
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		content = xmlNodeGetContent(last);
		if(content)
			add_property_stringl(child, "content", (char *) content, strlen(content), 1);
		add_property_resource(child, "node", ret);
		add_property_long(child, "type", last->type);
		zend_hash_next_index_insert(return_value->value.ht, &child, sizeof(zval *), NULL);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto object domxml_unlink_node([int node])
   Deletes node */
PHP_FUNCTION(domxml_unlink_node)
{
	zval *id, **tmp;
	xmlNode *nodep, *last;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}

	xmlUnlinkNode(nodep);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_get_attribute([int node,] string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_get_attribute)
{
	zval *id, *arg1, **tmp;
	xmlNode *nodep;
	char *value;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() == 2) && getParameters(ht, 2, &id, &arg1) == SUCCESS) {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);

	value = xmlGetProp(nodep, arg1->value.str.val);
	if (!value) {
		php_error(E_WARNING, "No such attribute '%s'", arg1->value.str.val);
		RETURN_FALSE;
	}
	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto bool domxml_set_attribute([int node,] string attrname, string value)
   Sets value of given attribute */
PHP_FUNCTION(domxml_set_attribute)
{
	zval *id, *arg1, *arg2, **tmp;
	xmlNode *nodep;
	xmlAttr *attr;

	if ((ZEND_NUM_ARGS() == 2) && getParameters(ht, 2, &arg1, &arg2) == SUCCESS) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() == 3) && getParameters(ht, 3, &id, &arg1, &arg2) == SUCCESS) {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	} else {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_string(arg1);
	convert_to_string(arg2);

	attr = xmlSetProp(nodep, arg1->value.str.val, arg2->value.str.val);
	if (!attr) {
		php_error(E_WARNING, "No such attribute '%s'", arg1->value.str.val);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array domxml_attributes([int node])
   Returns list of attributes of node */
PHP_FUNCTION(domxml_attributes)
{
	zval *id, **tmp;
	xmlNode *nodep;
#ifdef oldstyle_for_libxml_1_8_7
	xmlAttr *attr;
#endif
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}

	if(0 > node_attributes(&return_value, nodep))
		RETURN_FALSE;

#ifdef oldstyle_for_libxml_1_8_7
	attr = nodep->properties;
	if (!attr) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(attr) {
fprintf(stderr, "ATTRNAME = %s\n", attr->name);
		if(attr->children) {
			fprintf(stderr, "ATTRVALUE present\n");
			if(attr->children->content)
				fprintf(stderr, "ATTRVALUE = %s\n", attr->children->content);
				add_assoc_string(return_value, (char *) attr->name, attr->children->content, 1);
		}
		attr = attr->next;
	}
#endif
}
/* }}} */

/* {{{ proto array domxml_rootnew([int doc])
   Returns root nodes */
PHP_FUNCTION(domxml_rootnew)
{
	zval *id, **tmp;
	xmlDoc *docp;
	xmlNode *last;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
	}

	last = docp->children;
	if (!last) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(last) {
		zval *child;
		xmlChar *content;
		MAKE_STD_ZVAL(child);

		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object */
		object_init_ex(child, domxmlnode_class_entry_ptr);
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		content = xmlNodeGetContent(last);
		if(content)
			add_property_stringl(child, "content", (char *) content, strlen(content), 1);
		add_property_resource(child, "node", ret);
		add_property_long(child, "type", last->type);
		zend_hash_next_index_insert(return_value->value.ht, &child, sizeof(zval *), NULL);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto array domxml_root([int doc_handle])
   Returns root node of document */
PHP_FUNCTION(domxml_root)
{
	zval *id, **tmp;
	xmlDoc *docp;
	xmlNode *node;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
	}

	node = docp->children;
	if (!node) {
		RETURN_FALSE;
	}

	while(node) {
		xmlChar *content;
		if(node->type == XML_ELEMENT_NODE) {
			ret = zend_list_insert(node, le_domxmlnodep);

			/* construct an object with some methods */
			object_init_ex(return_value, domxmlnode_class_entry_ptr);
			add_property_resource(return_value, "node", ret);
			add_property_long(return_value, "type", node->type);
			add_property_stringl(return_value, "name", (char *) node->name, strlen(node->name), 1);
			content = xmlNodeGetContent(node);
			if(content)
				add_property_stringl(return_value, "content", (char *) content, strlen(content), 1);
			zend_list_addref(ret);
			return;
		}
		node = node->next;
	}
}
/* }}} */

/* {{{ proto object domxml_dtd([int doc_handle])
   Returns DTD of document */
PHP_FUNCTION(domxml_intdtd)
{
	zval *id, **tmp;
	xmlDoc *docp;
	xmlDtd *dtd;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
	}

	dtd = docp->intSubset;
	if (!dtd) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(dtd, le_domxmldtdp);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmldtd_class_entry_ptr);
	add_property_resource(return_value, "dtd", ret);
	if(dtd->ExternalID)
		add_property_string(return_value, "extid", (char *) dtd->ExternalID, 1);
	add_property_string(return_value, "sysid", (char *) dtd->SystemID, 1);
	add_property_string(return_value, "name", (char *) dtd->name, 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_dumpmem([int doc_handle])
   Dumps document into string */
PHP_FUNCTION(domxml_dumpmem)
{
	zval *id, **tmp;
	xmlDoc *docp;
	xmlChar *mem;
	int size;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
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
	zval *arg;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlParseDoc(arg->value.str.val); /*, arg->value.str.len); */
	if (!docp) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(docp, le_domxmldocp);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmldoc_class_entry_ptr);
	add_property_resource(return_value, "doc", ret);
	if(docp->name)
		add_property_stringl(return_value, "name", (char *) docp->name, strlen(docp->name), 1);
	else
		add_property_stringl(return_value, "name", "", 0, 1);
	if(docp->URL)
		add_property_stringl(return_value, "url", (char *) docp->URL, strlen(docp->URL), 1);
	else
		add_property_stringl(return_value, "url", "", 0, 1);
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

/* {{{ proto object xmldocfile(string filename)
   Creates DOM object of XML document in file*/
PHP_FUNCTION(xmldocfile)
{
	zval *arg;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlParseFile(arg->value.str.val);
	if (!docp) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(docp, le_domxmldocp);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmldoc_class_entry_ptr);
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

/* {{{ proto object domxml_new_child([int node_handle,] string name, string content)
   Adds child node to parent node */
PHP_FUNCTION(domxml_new_child)
{
	zval *id, *name, *content, **tmp;
	xmlNode *child, *nodep;
	int ret;

	if (ZEND_NUM_ARGS() == 2) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
			if(getParameters(ht, 2, &name, &content) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 3) || getParameters(ht, 3, &id, &name, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}
	convert_to_string(name);
	convert_to_string(content);
		
	if(content->value.str.len)
		child = xmlNewChild(nodep, NULL, name->value.str.val, content->value.str.val);
	else
		child = xmlNewChild(nodep, NULL, name->value.str.val, NULL);
	if (!child) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(child, le_domxmlnodep);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmlnode_class_entry_ptr);
	add_property_resource(return_value, "node", ret);
	add_property_long(return_value, "type", child->type);
	add_property_stringl(return_value, "name", (char *) child->name, strlen(child->name), 1);
	if(content->value.str.val)
		add_property_stringl(return_value, "content", content->value.str.val, content->value.str.len, 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto bool domxml_set_content([int node_handle,] string content)
   Set content of a node */
PHP_FUNCTION(domxml_set_content)
{
	zval *id, *content, **tmp;
	xmlNode *nodep;

	if (ZEND_NUM_ARGS() == 1) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
			if(getParameters(ht, 1, &content) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 2) || getParameters(ht, 2, &id, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(nodep,xmlNodePtr,tmp,-1, "DomNode", le_domxmlnodep)
	}
	convert_to_string(content);

	if(content->value.str.len)
		xmlNodeSetContent(nodep, content->value.str.val);

	/* FIXME: Actually the property 'content' of the node has to be updated
	   as well. Since 'content' should disappear sooner or later and being
	   replaces by a function 'content()' I skip this for now
	*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object domxml_add_root([int doc_handle,] string name)
   Adds root node to document */
PHP_FUNCTION(domxml_add_root)
{
	zval *id, *name, **tmp;
	xmlDoc *docp;
	xmlNode *node;
	xmlChar *content;
	int ret;
	
	if (ZEND_NUM_ARGS() == 1) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
			if (getParameters(ht, 1, &name) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 2) || getParameters(ht, 2, &id, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
	}
	convert_to_string(name);
		
	node = xmlNewDocNode(docp, NULL, name->value.str.val, NULL);
	if (!node) {
		RETURN_FALSE;
	}
	xmlDocSetRootElement(docp, node);
	ret = zend_list_insert(node, le_domxmlnodep);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmlnode_class_entry_ptr);
	add_property_resource(return_value, "node", ret);
	add_property_long(return_value, "type", node->type);
	add_property_stringl(return_value, "name", (char *) node->name, strlen(node->name), 1);
	content = xmlNodeGetContent(node);
	if(content)
		add_property_stringl(return_value, "content", (char *) content, strlen(content), 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto object domxml_new_xmldoc(string version)
   Creates new xmldoc */
PHP_FUNCTION(domxml_new_xmldoc)
{
	zval *arg;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlNewDoc(arg->value.str.val);
	if (!docp) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(docp, le_domxmldocp);

	/* construct an object with some methods */
	object_init_ex(return_value, domxmldoc_class_entry_ptr);
	add_property_resource(return_value, "doc", ret);
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	if(docp->encoding)
		add_property_stringl(return_value, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
	add_property_long(return_value, "standalone", docp->standalone);
	zend_list_addref(ret);
}
/* }}} */

#ifdef newcode
/* {{{ proto int node_namespace([int node])
   Returns list of namespaces */
static int node_namespace(zval **attributes, xmlNode *nodep)
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
		MAKE_STD_ZVAL(pattr);
/*		ret = zend_list_insert(attr, le_domxmlattrp); */

		/* construct an object with some methods */
		object_init_ex(pattr, domxmlns_class_entry_ptr);
/*		add_property_resource(pattr, "attribute", ret); */
		if(ns->href)
			add_property_stringl(pattr, "href", (char *) ns->href, strlen(ns->href), 1);
		if(ns->prefix)
			add_property_stringl(pattr, "prefix", (char *) ns->prefix, strlen(ns->prefix), 1);
		add_property_long(pattr, "type", ns->type);
		zend_hash_next_index_insert((*attributes)->value.ht, &pattr, sizeof(zval *), NULL);
		ns = ns->next;
	}
	return 0;
}
#endif

/* {{{ proto int node_attributes(zval **attributes, int node)
   Returns list of children nodes */
static int node_attributes(zval **attributes, xmlNode *nodep)
{
	zval *children;
	xmlAttr *attr;
	int count = 0;

	/* Get the children of the current node */	
	if(nodep->type != XML_ELEMENT_NODE)
		return -1;
	attr = nodep->properties;
	if (!attr) {
		return -1;
	}

	/* create an php array for the children */
/*	MAKE_STD_ZVAL(*attributes); *//* Don't do this if *attributes are the return_value */
	if (array_init(*attributes) == FAILURE) {
		return -1;
	}

	while(attr) {
		zval *pattr;
		int n;
		MAKE_STD_ZVAL(pattr);

		/* construct an object with some methods */
		object_init_ex(pattr, domxmlattr_class_entry_ptr);
		add_property_stringl(pattr, "name", (char *) attr->name, strlen(attr->name), 1);
		if(0 <= (n = node_children(&children, attr->children))) {
			zend_hash_update(pattr->value.obj.properties, "children", sizeof("children"), (void *) &children, sizeof(zval *), NULL);
		}
		zend_hash_next_index_insert((*attributes)->value.ht, &pattr, sizeof(zval *), NULL);
		attr = attr->next;
		count++;
	}
	return count;
}

/* {{{ proto int node_children([int node])
   Returns list of children nodes */
static int node_children(zval **children, xmlNode *nodep)
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
	if (array_init(*children) == FAILURE) {
		return -1;
	}

	while(last) {
		zval *child;
		xmlChar *content;
		int ret;
	
/*		if(last->type != XML_TEXT_NODE) { */
		/* Each child is a node object */
		MAKE_STD_ZVAL(child);
		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object for each child */
		object_init_ex(child, domxmlnode_class_entry_ptr);

		/* Add the node object to the array of children */
		zend_hash_next_index_insert((*children)->value.ht, &child, sizeof(zval *), NULL);

		/* Add name, content and type as properties */
		if(last->name)
			add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		if(last->type)
			add_property_long(child, "type", last->type);
		content = xmlNodeGetContent(last);
		if(content)
			add_property_stringl(child, "content", (char *) content, strlen(content), 1);
		add_property_resource(child, "node", ret);

		/* Get the namespace of the current node and add it as a property */
/*		if(!node_namespace(&namespace, last))
			zend_hash_update(child->value.obj.properties, "namespace", sizeof("namespace"), (void *) &namespace, sizeof(zval *), NULL);
*/

		/* Get the attributes of the current node and add it as a property */
		MAKE_STD_ZVAL(attributes); /* Because it was taken out of node_attributes() */
		if(0 <= node_attributes(&attributes, last))
			zend_hash_update(child->value.obj.properties, "attributes", sizeof("attributes"), (void *) &attributes, sizeof(zval *), NULL);

		/* Get recursively the children of the current node and add it as a property */
		if(0 <= node_children(&mchildren, last->children))
			zend_hash_update(child->value.obj.properties, "children", sizeof("children"), (void *) &mchildren, sizeof(zval *), NULL);

		count++;
/*		} */
		last = last->next;
	}
	return count;
}
/* }}} */

/* {{{ proto object xmltree(string xmldoc)
   Create a tree of PHP objects from an XML document */
PHP_FUNCTION(xmltree)
{
	zval *arg, *children;
	xmlDoc *docp;
	xmlNode *root;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Create a new xml document */
	docp = xmlParseMemory(arg->value.str.val, arg->value.str.len);
	if (!docp) {
		RETURN_FALSE;
	}

	/* construct the document is a php object for return */
	object_init_ex(return_value, domxmldoc_class_entry_ptr);
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	if(docp->encoding)
		add_property_stringl(return_value, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
	add_property_long(return_value, "standalone", docp->standalone);
	add_property_long(return_value, "type", docp->type);

	/* get the root and add as a property to the document */
	root = docp->children;
	if (!root) {
		xmlFreeDoc(docp);
		RETURN_FALSE;
	}

#ifdef newcode
	{
	zval *child;
	MAKE_STD_ZVAL(child);
	object_init_ex(child, domxmltestnode_class_entry_ptr);
	zend_hash_update(return_value->value.obj.properties, "testnode", sizeof("testnode"), &child, sizeof(zval *), NULL);
	add_property_stringl(child, "name", "Testname", sizeof("Testname"), 1);
	}
#endif

	/* The root itself maybe an array. Though you may not have two Elements
	   as root, you may have a comment, pi and and element as root.
	   Thanks to Paul DuBois for pointing me at this.
	*/
	if(0 <= node_children(&children, root)) {
		int i, count;
		HashTable *lht;
		zend_hash_update(return_value->value.obj.properties, "children", sizeof("children"), (void *) &children, sizeof(zval *), NULL);

		/* Find the child of xml type element */
		lht = children->value.ht;
		count = zend_hash_num_elements(lht);
		zend_hash_internal_pointer_reset(lht);
		for(i=0; i<count; i++) {
			zval **prop, **keydata;
			zend_hash_get_current_data(lht, (void **) &keydata);
			if((*keydata)->type == IS_OBJECT) {
			  if (zend_hash_find((*keydata)->value.obj.properties, "type", sizeof("type"), (void **)&prop) == SUCCESS) {
					if((*prop)->value.lval == XML_ELEMENT_NODE) {
						zend_hash_update(return_value->value.obj.properties, "root", sizeof("root"), (void **) keydata, sizeof(zval *), NULL);
						(*keydata)->is_ref = 1;
						(*keydata)->refcount++;
					}
				}
			}
			zend_hash_move_forward(lht);
		}
	}
	xmlFreeDoc(docp);
}
/* }}} */

#if defined(LIBXML_XPATH_ENABLED)
/* {{{ proto bool xpath_init(void)
   Initializing XPath environment */
PHP_FUNCTION(xpath_init)
{
	xmlXPathInit();
	RETURN_TRUE;
}
/* }}} */

static void php_xpathptr_new_context(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *id, **tmp;
	xmlXPathContextPtr ctx;
	xmlDocPtr docp;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my handle property");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(docp,xmlDocPtr,tmp,-1, "DomDocument", le_domxmldocp)
	}
		
#if defined(LIBXML_XPTR_ENABLED)
	if(mode == PHP_XPTR)
		ctx = xmlXPtrNewContext(docp, NULL, NULL);
	else
#endif
		ctx = xmlXPathNewContext(docp);
	if (!ctx) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(ctx, le_xpathctxp);

	/* construct an object with some methods */
	object_init_ex(return_value, xpathctx_class_entry_ptr);
	add_property_resource(return_value, "xpathctx", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string xpath_new_context([int doc_handle])
   Create new XPath context */
PHP_FUNCTION(xpath_new_context) {
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH);
}
/* }}} */

/* {{{ proto string xptr_new_context([int doc_handle])
   Create new XPath context */
PHP_FUNCTION(xptr_new_context) {
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR);
}
/* }}} */

static void php_xpathptr_eval(INTERNAL_FUNCTION_PARAMETERS, int mode, int expr)
{
	zval *id, *str, **tmp;
	xmlXPathContextPtr ctxp;
	xmlXPathObjectPtr xpathobjp;
	int ret;
	
	if (ZEND_NUM_ARGS() == 1) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "xpathctx", sizeof("xpathctx"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my xpath context");
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE(ctxp,xmlXPathContextPtr,tmp,-1, "XPathContext", le_xpathctxp)
			if (getParameters(ht, 1, &str) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 2) || getParameters(ht, 2, &id, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		if (zend_hash_find(id->value.obj.properties, "xpathctx", sizeof("xpathctx"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my own xpath context");
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(ctxp,xmlXPathContextPtr,tmp,-1, "XPathContext", le_xpathctxp)
	}
	convert_to_string(str);

#if defined(LIBXML_XPTR_ENABLED)
	if(mode == PHP_XPTR) {
		xpathobjp = xmlXPtrEval(BAD_CAST str->value.str.val, ctxp);
	} else{
#endif
		if(expr)
			xpathobjp = xmlXPathEvalExpression(str->value.str.val, ctxp);
		else
			xpathobjp = xmlXPathEval(str->value.str.val, ctxp);
#if defined(LIBXML_XPTR_ENABLED)
	}
#endif
		
	if (!xpathobjp) {
		RETURN_FALSE;
	}

	ret = zend_list_insert(xpathobjp, le_xpathobjectp);
	zend_list_addref(ret);

	/* construct an object with some methods */
	object_init_ex(return_value, xpathobject_class_entry_ptr);
	add_property_resource(return_value, "xpathobject", ret);
	add_property_long(return_value, "type", xpathobjp->type);
	switch(xpathobjp->type) {
		case XPATH_UNDEFINED:
			break;
		case XPATH_NODESET: {
			int i;
			zval *arr;
			xmlNodeSetPtr nodesetp;

			MAKE_STD_ZVAL(arr);
			if (array_init(arr) == FAILURE) {
				xmlXPathFreeObject(xpathobjp);
				RETURN_FALSE;
			}
			if(NULL == (nodesetp = xpathobjp->nodesetval)) {
				xmlXPathFreeObject(xpathobjp);
				RETURN_FALSE;
			}

			for(i = 0;i < nodesetp->nodeNr;i++) {
				xmlNodePtr node = nodesetp->nodeTab[i];
				zval *child;
				xmlChar *content;
				int retnode;
				MAKE_STD_ZVAL(child);

				retnode = zend_list_insert(node, le_domxmlnodep);

				/* construct a node object */
				object_init_ex(child, domxmlnode_class_entry_ptr);
				add_property_long(child, "type", node->type);
				add_property_stringl(child, "name", (char *) node->name, strlen(node->name), 1);
				content = xmlNodeGetContent(node);
				if(content)
					add_property_stringl(child, "content", (char *) content, strlen(content), 1);
				add_property_resource(child, "node", retnode);
				zend_hash_next_index_insert(arr->value.ht, &child, sizeof(zval *), NULL);
			}
			zend_hash_update(return_value->value.obj.properties, "nodeset", sizeof("nodeset"), (void *) &arr, sizeof(zval *), NULL);
			break;
		}
		case XPATH_BOOLEAN:
			add_property_bool(return_value, "value", xpathobjp->boolval);
			break;
		case XPATH_NUMBER:
			add_property_double(return_value, "value", xpathobjp->floatval);
			break;
		case XPATH_STRING:
			add_property_string(return_value, "value", xpathobjp->stringval, 1);
			break;
		case XPATH_POINT:
			break;
		case XPATH_RANGE:
			break;
		case XPATH_LOCATIONSET:
			break;
		case XPATH_USERS:
			break;
	}
}
/* }}} */

/* {{{ proto int xpath_eval([int xpathctx_handle,] string str)
   Evaluate the XPath Location Path in the given string */
PHP_FUNCTION(xpath_eval) {
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 0);
}
/* }}} */

/* {{{ proto int xpath_eval_expression([int xpathctx_handle,] string str)
   Evaluate the XPath Location Path in the given string */
PHP_FUNCTION(xpath_eval_expression) {
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 1);
}
/* }}} */
#endif /* defined(LIBXML_XPATH_ENABLED) */

#if defined(LIBXML_XPTR_ENABLED)
/* {{{ proto int xptr_eval([int xpathctx_handle,] string str)
   Evaluate the XPtr Location Path in the given string */
PHP_FUNCTION(xptr_eval) {
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR, 0);
}
/* }}} */
#endif /* LIBXML_XPTR_ENABLED */

#endif /* HAVE_DOMXML */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
