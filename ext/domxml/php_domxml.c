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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#include "php.h"
#include "php_domxml.h"

#if HAVE_DOMXML
#include "ext/standard/info.h"
/*#define newcode*/

static int le_domxmldocp;
static int le_domxmldtdp;
static int le_domxmlnodep;
static int le_domxmlattrp;
static int le_domxmlnsp;
static zend_class_entry *domxmldoc_class_entry_ptr;
static zend_class_entry *domxmldtd_class_entry_ptr;
static zend_class_entry *domxmlnode_class_entry_ptr;
static zend_class_entry *domxmlattr_class_entry_ptr;
static zend_class_entry *domxmlns_class_entry_ptr;
static zend_class_entry *domxmltestnode_class_entry_ptr;

static int node_attributes(zval **attributes, xmlNode *nodep);
static int node_children(zval **children, xmlNode *nodep);

static zend_function_entry php_domxml_functions[] = {
	PHP_FE(xmldoc,	NULL)
	PHP_FE(xmldocfile,	NULL)
	PHP_FE(xmltree,	NULL)
	PHP_FE(domxml_root,	NULL)
	PHP_FE(domxml_add_root,	NULL)
	PHP_FE(domxml_dumpmem,	NULL)
	PHP_FE(domxml_attributes,	NULL)
	PHP_FE(domxml_getattr,	NULL)
	PHP_FE(domxml_setattr,	NULL)
	PHP_FE(domxml_children,	NULL)
	PHP_FE(domxml_new_child,	NULL)
	PHP_FE(domxml_node,	NULL)
	PHP_FE(domxml_new_xmldoc,	NULL)
	PHP_FALIAS(new_xmldoc, domxml_new_xmldoc,	NULL)
	{NULL, NULL, NULL}
};


static zend_function_entry php_domxmldoc_class_functions[] = {
	PHP_FALIAS(root,	domxml_root,	NULL)
	PHP_FALIAS(children,	domxml_children,	NULL)
	PHP_FALIAS(add_root,	domxml_add_root,	NULL)
	PHP_FALIAS(dtd,	domxml_intdtd,	NULL)
	PHP_FALIAS(dumpmem,	domxml_dumpmem,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnode_class_functions[] = {
	PHP_FALIAS(lastchild,	domxml_lastchild,	NULL)
	PHP_FALIAS(children,	domxml_children,	NULL)
	PHP_FALIAS(parent,	domxml_parent,		NULL)
	PHP_FALIAS(new_child,	domxml_new_child,		NULL)
	PHP_FALIAS(getattr,	domxml_getattr,		NULL)
	PHP_FALIAS(setattr,	domxml_setattr,		NULL)
	PHP_FALIAS(attributes,	domxml_attributes,	NULL)
	PHP_FALIAS(node,	domxml_node,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmltestnode_class_functions[] = {
	PHP_FE(domxml_test,	NULL)
	{NULL, NULL, NULL}
};

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

zend_module_entry php_domxml_module_entry = {
	"domxml", php_domxml_functions, PHP_MINIT(domxml), NULL, NULL, NULL, PHP_MINFO(domxml), STANDARD_MODULE_PROPERTIES
};

void _free_node(xmlNode *tmp) {
/*fprintf(stderr, "Freeing node: %s\n", tmp->name);*/
}

PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry domxmldoc_class_entry;
	zend_class_entry domxmldtd_class_entry;
	zend_class_entry domxmlnode_class_entry;
	zend_class_entry domxmlattr_class_entry;
	zend_class_entry domxmlns_class_entry;

#ifdef newcode
  domxmltestnode_class_startup();
#endif

	le_domxmldocp = register_list_destructors(xmlFreeDoc, NULL);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	*/
	le_domxmlnodep = register_list_destructors(_free_node, NULL);
	le_domxmlattrp = register_list_destructors(NULL, NULL);
/*	le_domxmlnsp = register_list_destructors(NULL, NULL); */

	INIT_CLASS_ENTRY(domxmldoc_class_entry, "DomDocument", php_domxmldoc_class_functions);
	INIT_CLASS_ENTRY(domxmldtd_class_entry, "Dtd", php_domxmldtd_class_functions);
	INIT_CLASS_ENTRY(domxmlnode_class_entry, "DomNode", php_domxmlnode_class_functions);
	INIT_CLASS_ENTRY(domxmlattr_class_entry, "DomAttribute", php_domxmlattr_class_functions);
	INIT_CLASS_ENTRY(domxmlns_class_entry, "DomNamespace", php_domxmlns_class_functions);

	domxmldoc_class_entry_ptr = zend_register_internal_class(&domxmldoc_class_entry);
	domxmldtd_class_entry_ptr = zend_register_internal_class(&domxmldtd_class_entry);
	domxmlnode_class_entry_ptr = zend_register_internal_class(&domxmlnode_class_entry);
	domxmlattr_class_entry_ptr = zend_register_internal_class(&domxmlattr_class_entry);
	domxmlns_class_entry_ptr = zend_register_internal_class(&domxmlns_class_entry);

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
	REGISTER_LONG_CONSTANT("XML_GLOBAL_NAMESPACE", XML_GLOBAL_NAMESPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_LOCAL_NAMESPACE", XML_LOCAL_NAMESPACE, CONST_CS | CONST_PERSISTENT);
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

/* {{{ proto string domxml_test(int id)
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
	php_info_print_table_row(2, "DOM/XML Support", "enabled");
/*	php_info_print_table_row(2, "libmxl Version", LIBXML_VERSION ); */
	php_info_print_table_end();
}

/* {{{ proto string domxml_attrname([int dir_handle])
   Returns list of attribute objects */
PHP_FUNCTION(domxml_attrname)
{
	zval *id, **tmp;
	int id_to_find;
	xmlNode *nodep;
	xmlAttr *attr;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
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

/* {{{ proto class domxml_node(string name)
   Creates node */
PHP_FUNCTION(domxml_node)
{
	zval *arg;
	xmlNode *node;
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
	if(node->content)
		add_property_stringl(return_value, "content", (char *) node->content, strlen(node->content), 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_lastchild([int node])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_lastchild)
{
	zval *id, **tmp;
	int id_to_find;
	xmlNode *nodep, *last;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
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
	if(last->content)
		add_property_stringl(return_value, "content", (char *) last->content, strlen(last->content), 1);
}
/* }}} */

/* {{{ proto string domxml_parent([int node])
   Returns parent of node */
PHP_FUNCTION(domxml_parent)
{
	zval *id, **tmp;
	int id_to_find;
	xmlNode *nodep, *last;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
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
	if(last->content)
		add_property_stringl(return_value, "content", (char *) last->content, strlen(last->content), 1);
}
/* }}} */

/* {{{ proto string domxml_children([int node])
   Returns list of children nodes */
PHP_FUNCTION(domxml_children)
{
	zval *id, **tmp;
	int id_to_find;
	xmlNode *nodep, *last;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if ((zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) &&
			    (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE)) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || (type != le_domxmlnodep && type != le_domxmldocp)) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
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
		MAKE_STD_ZVAL(child);

		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object */
		object_init_ex(child, domxmlnode_class_entry_ptr);
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		if(last->content)
			add_property_stringl(child, "content", (char *) last->content, strlen(last->content), 1);
		add_property_resource(child, "node", ret);
		add_property_long(child, "type", last->type);
		zend_hash_next_index_insert(return_value->value.ht, &child, sizeof(zval *), NULL);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto string domxml_getattr([int node,] string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_getattr)
{
	zval *id, *arg1, **tmp;
	int id_to_find;
	xmlNode *nodep;
	char *value;
	int type;
	
	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
		convert_to_string(arg1);
	} else if ((ZEND_NUM_ARGS() == 2) && getParameters(ht, 2, &id, &arg1) == SUCCESS) {
		convert_to_long(id);
		id_to_find = id->value.lval;
		convert_to_string(arg1);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

	value = xmlGetProp(nodep, arg1->value.str.val);
	if (!value) {
		php_error(E_WARNING, "No such attribute '%s'", arg1->value.str.val);
		RETURN_FALSE;
	}
	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto string domxml_setattr([int node,] string attrname, string value)
   Sets value of given attribute */
PHP_FUNCTION(domxml_setattr)
{
	zval *id, *arg1, *arg2, **tmp;
	int id_to_find;
	xmlNode *nodep;
	xmlAttr *attr;
	int type;
	
	if ((ZEND_NUM_ARGS() == 2) && getParameters(ht, 2, &arg1, &arg2) == SUCCESS) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
		convert_to_string(arg1);
		convert_to_string(arg2);
	} else if ((ZEND_NUM_ARGS() == 3) && getParameters(ht, 3, &id, &arg1, &arg2) == SUCCESS) {
		convert_to_long(id);
		id_to_find = id->value.lval;
		convert_to_string(arg1);
		convert_to_string(arg2);
	} else {
		WRONG_PARAM_COUNT;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

	attr = xmlSetProp(nodep, arg1->value.str.val, arg2->value.str.val);
	if (!attr) {
		php_error(E_WARNING, "No such attribute '%s'", arg1->value.str.val);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_attributes([int node])
   Returns list of attributes of node */
PHP_FUNCTION(domxml_attributes)
{
	zval *id, **tmp;
	int id_to_find;
	xmlNode *nodep;
	xmlAttr *attr;
	int type;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my node (%d)", id);
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find node identifier (%d)", id_to_find);
		RETURN_FALSE;
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

/* {{{ proto string domxml_rootnew([int doc])
   Returns list of children nodes */
PHP_FUNCTION(domxml_rootnew)
{
	zval *id, **tmp;
	int id_to_find;
	xmlDoc *nodep;
	xmlNode *last;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	nodep = (xmlDoc *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmldocp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

	last = nodep->children;
	if (!last) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(last) {
		zval *child;
		MAKE_STD_ZVAL(child);

		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object */
		object_init_ex(child, domxmlnode_class_entry_ptr);
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		if(last->content)
			add_property_stringl(child, "content", (char *) last->content, strlen(last->content), 1);
		add_property_resource(child, "node", ret);
		add_property_long(child, "type", last->type);
		zend_hash_next_index_insert(return_value->value.ht, &child, sizeof(zval *), NULL);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto string domxml_root([int doc_handle])
   Returns root node of document */
PHP_FUNCTION(domxml_root)
{
	zval *id, **tmp;
	int id_to_find;
	xmlDoc *docp;
	xmlNode *node;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	docp = (xmlDoc *)zend_list_find(id_to_find, &type);
	if (!docp || type != le_domxmldocp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

	node = docp->children;
	if (!node) {
		RETURN_FALSE;
	}

	while(node) {
		if(node->type == XML_ELEMENT_NODE) {
			ret = zend_list_insert(node, le_domxmlnodep);

			/* construct an object with some methods */
			object_init_ex(return_value, domxmlnode_class_entry_ptr);
			add_property_resource(return_value, "node", ret);
			add_property_long(return_value, "type", node->type);
			add_property_stringl(return_value, "name", (char *) node->name, strlen(node->name), 1);
			if(node->content)
				add_property_stringl(return_value, "content", (char *) node->content, strlen(node->content), 1);
			zend_list_addref(ret);
			return;
		}
		node = node->next;
	}
}
/* }}} */

/* {{{ proto string domxml_dtd([int doc_handle])
   Returns DTD of document */
PHP_FUNCTION(domxml_intdtd)
{
	zval *id, **tmp;
	int id_to_find;
	xmlDoc *docp;
	xmlDtd *dtd;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	docp = (xmlDoc *)zend_list_find(id_to_find, &type);
	if (!docp || type != le_domxmldocp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
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
	int id_to_find;
	xmlDoc *docp;
/*	char *mem; */
	xmlChar *mem;
	int size;
	int type;
	
	if (ZEND_NUM_ARGS() == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	docp = (xmlDoc *)zend_list_find(id_to_find, &type);
	if (!docp || type != le_domxmldocp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

	xmlDocDumpMemory(docp, &mem, &size);
	if (!size) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(mem, size, 1);
}
/* }}} */

/* {{{ proto class xmldoc(string xmldoc)
   Creates dom object of xml document */
PHP_FUNCTION(xmldoc)
{
	zval *arg;
	xmlDoc *docp;
	int ret;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	docp = xmlParseMemory(arg->value.str.val, arg->value.str.len);
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
	add_property_long(return_value, "type", docp->type);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto class xmldocfile(string filename)
   Creates dom object of xml document in file*/
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
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	if(docp->encoding)
		add_property_stringl(return_value, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
	add_property_long(return_value, "standalone", docp->standalone);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_new_child([int node_handle,] string name, string content)
   Adds child node to parent node */
PHP_FUNCTION(domxml_new_child)
{
	zval *id, *name, *content, **tmp;
	int id_to_find;
	xmlNode *child, *nodep;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 2) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "node", sizeof("node"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
			if(getParameters(ht, 2, &name, &content) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 3) || getParameters(ht, 3, &id, &name, &content) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
	convert_to_string(name);
	convert_to_string(content);
		
	nodep = (xmlNode *)zend_list_find(id_to_find, &type);
	if (!nodep || type != le_domxmlnodep) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

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

/* {{{ proto string domxml_add_root([int doc_handle,] string name)
   Adds root node to document */
PHP_FUNCTION(domxml_add_root)
{
	zval *id, *name, **tmp;
	int id_to_find;
	xmlDoc *docp;
	xmlNode *node;
	int type;
	int ret;
	
	if (ZEND_NUM_ARGS() == 1) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "doc", sizeof("doc"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
			if (getParameters(ht, 1, &name) == FAILURE)
				WRONG_PARAM_COUNT;
		} else {
			RETURN_FALSE;
		}
	} else if ((ZEND_NUM_ARGS() != 2) || getParameters(ht, 2, &id, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
	convert_to_string(name);
		
	docp = (xmlDoc *)zend_list_find(id_to_find, &type);
	if (!docp || type != le_domxmldocp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}

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
	if(node->content)
		add_property_stringl(return_value, "content", (char *) node->content, strlen(node->content), 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto class domxml_new_xmldoc(string version)
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

/* {{{ proto string node_namespace([int node])
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

/* {{{ proto string node_attributes([int node])
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

/* {{{ proto string domxml_children([int node])
   Returns list of children nodes */
static int node_children(zval **children, xmlNode *nodep)
{
	zval *mchildren, *attributes, *namespace;
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

		/* Each child is a node object */
		MAKE_STD_ZVAL(child);
/*		ret = zend_list_insert(last, le_domxmlnodep); */

		/* construct a node object for each child */
		object_init_ex(child, domxmlnode_class_entry_ptr);

		/* Add the node object to the array of children */
		zend_hash_next_index_insert((*children)->value.ht, &child, sizeof(zval *), NULL);

		/* Add name, content and type as properties */
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		add_property_long(child, "type", last->type);
		if(last->content)
			add_property_stringl(child, "content", (char *) last->content, strlen(last->content), 1);
/*		add_property_resource(child, "node", ret); */

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

		last = last->next;
		count++;
	}
	return count;
}
/* }}} */

/* {{{ proto class xmltree(string xmldoc)
   Create a tree of php objects from an xml document */
PHP_FUNCTION(xmltree)
{
	zval *arg;
	zval *proot, *children, *attributes;
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

#endif /* HAVE_DOMXML */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
