/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
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

static int le_domxmldocp;
static int le_domxmldtdp;
static int le_domxmlnodep;
static int le_domxmlattrp;
static zend_class_entry *domxmldoc_class_entry_ptr;
static zend_class_entry *domxmldtd_class_entry_ptr;
static zend_class_entry *domxmlnode_class_entry_ptr;
static zend_class_entry *domxmlattr_class_entry_ptr;

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
	PHP_FALIAS(add_root,	domxml_add_root,	NULL)
	PHP_FALIAS(dtd,	domxml_intdtd,	NULL)
	PHP_FALIAS(dumpmem,	domxml_dumpmem,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

/* FIXME: If the following list extends 5 entries, then calling
   any of the functions results in a segm fault in execute().
   It appears the hash table is somewhat corrupted.
*/
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

static zend_function_entry php_domxmlattr_class_functions[] = {
	PHP_FALIAS(name,	domxml_attrname,	NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_domxml_module_entry = {
	"DOM", php_domxml_functions, PHP_MINIT(domxml), NULL, NULL, NULL, PHP_MINFO(domxml), STANDARD_MODULE_PROPERTIES
};

void _free_node(xmlNode *tmp) {
//fprintf(stderr, "Freeing node: %s\n", tmp->name);
}

PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry domxmldoc_class_entry;
	zend_class_entry domxmldtd_class_entry;
	zend_class_entry domxmlnode_class_entry;
	zend_class_entry domxmlattr_class_entry;

	le_domxmldocp = register_list_destructors(xmlFreeDoc, NULL);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	*/
	le_domxmlnodep = register_list_destructors(_free_node, NULL);
	le_domxmlattrp = register_list_destructors(NULL, NULL);

	

	INIT_CLASS_ENTRY(domxmldoc_class_entry, "Dom document", php_domxmldoc_class_functions);
	INIT_CLASS_ENTRY(domxmldtd_class_entry, "Dtd", php_domxmldtd_class_functions);
	INIT_CLASS_ENTRY(domxmlnode_class_entry, "Dom node", php_domxmlnode_class_functions);
	INIT_CLASS_ENTRY(domxmlattr_class_entry, "Dom Attribute", php_domxmlattr_class_functions);

	domxmldoc_class_entry_ptr = register_internal_class(&domxmldoc_class_entry);
	domxmldtd_class_entry_ptr = register_internal_class(&domxmldtd_class_entry);
	domxmlnode_class_entry_ptr = register_internal_class(&domxmlnode_class_entry);
	domxmlattr_class_entry_ptr = register_internal_class(&domxmlattr_class_entry);

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
	return SUCCESS;
}

PHP_MINFO_FUNCTION(domxml)
{
		PUTS("DOM/XML support active (compiled with libxml ");
		PUTS(XML_DEFAULT_VERSION);
		PUTS(".)");
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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
		add_property_stringl(pattr, "value", (char *) attr->val->content, strlen(attr->val->content), 1);
		zend_hash_next_index_insert(return_value->value.ht, &pattr, sizeof(zval *), NULL);
		attr = attr->next;
	}
}
/* }}} */

/* {{{ proto class node(string name)
   Creates node */
PHP_FUNCTION(domxml_node)
{
	zval *arg;
	xmlNode *node;
	int ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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

	last = nodep->childs;
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
	
	if ((ARG_COUNT(ht) == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
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
	} else if ((ARG_COUNT(ht) == 2) && getParameters(ht, 2, &id, &arg1) == SUCCESS) {
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
	
	if ((ARG_COUNT(ht) == 2) && getParameters(ht, 2, &arg1, &arg2) == SUCCESS) {
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
	} else if ((ARG_COUNT(ht) == 3) && getParameters(ht, 3, &id, &arg1, &arg2) == SUCCESS) {
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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

	attr = nodep->properties;
	if (!attr) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while(attr) {
		if(attr->val->content)
			add_assoc_string(return_value, (char *) attr->name, attr->val->content, 1);
		attr = attr->next;
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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

	node = docp->root;
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
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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
//	char *mem;
	xmlChar *mem;
	int size;
	int type;
	
	if (ARG_COUNT(ht) == 0) {
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
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
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
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
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
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
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
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_new_child([int node_handle], string name, string content)
   Adds child node to parent node */
PHP_FUNCTION(domxml_new_child)
{
	zval *id, *name, *content, **tmp;
	int id_to_find;
	xmlNode *child, *nodep;
	int type;
	int ret;
	
	if (ARG_COUNT(ht) == 2) {
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
	} else if ((ARG_COUNT(ht) != 3) || getParameters(ht, 3, &id, &name, &content) == FAILURE) {
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

/* {{{ proto string domxml_add_root([int doc_handle], string name)
   Adds root node to document */
PHP_FUNCTION(domxml_add_root)
{
	zval *id, *name, **tmp;
	int id_to_find;
	xmlDoc *docp;
	xmlNode *node;
	int type;
	int ret;
	
	if (ARG_COUNT(ht) == 1) {
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
	} else if ((ARG_COUNT(ht) != 2) || getParameters(ht, 2, &id, &name) == FAILURE) {
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
	docp->root = node;
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
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
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
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_children([int node])
   Returns list of children nodes */
static int node_attributes(zval **attributes, xmlNode *nodep)
{
	xmlAttr *attr;
	int ret;

	/* Get the children of the current node */	
	attr = nodep->properties;
	if (!attr) {
		return -1;
	}

	/* create an php array for the children */
	MAKE_STD_ZVAL(*attributes);
	if (array_init(*attributes) == FAILURE) {
		return -1;
	}

	while(attr) {
		zval *pattr;
		MAKE_STD_ZVAL(pattr);
		ret = zend_list_insert(attr, le_domxmlattrp);

		/* construct an object with some methods */
		object_init_ex(pattr, domxmlattr_class_entry_ptr);
		add_property_resource(pattr, "attribute", ret);
		add_property_stringl(pattr, "name", (char *) attr->name, strlen(attr->name), 1);
		if(attr->val->content)
			add_property_stringl(pattr, "value", (char *) attr->val->content, strlen(attr->val->content), 1);
		zend_hash_next_index_insert((*attributes)->value.ht, &pattr, sizeof(zval *), NULL);
		attr = attr->next;
	}
	return 0;
}

/* {{{ proto string domxml_children([int node])
   Returns list of children nodes */
static int node_children(zval **children, xmlNode *nodep)
{
	zval *mchildren, *attributes;
	xmlNode *last;
	int ret;

	/* Get the children of the current node */	
	last = nodep->childs;
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
		ret = zend_list_insert(last, le_domxmlnodep);

		/* construct a node object for each child */
		object_init_ex(child, domxmlnode_class_entry_ptr);
		add_property_stringl(child, "name", (char *) last->name, strlen(last->name), 1);
		if(last->content)
			add_property_stringl(child, "content", (char *) last->content, strlen(last->content), 1);
		add_property_resource(child, "node", ret);
		add_property_long(child, "type", last->type);
		/* Add the node object to the array of children */
		zend_hash_next_index_insert((*children)->value.ht, &child, sizeof(zval *), NULL);
		/* Get recursively the children of the current node */
		if(!node_children(&mchildren, last))
			zend_hash_update(child->value.obj.properties, "children", strlen("children")+1, (void *) &mchildren, sizeof(zval *), NULL);
		if(!node_attributes(&attributes, last))
			zend_hash_update(child->value.obj.properties, "attributes", strlen("attributes")+1, (void *) &attributes, sizeof(zval *), NULL);

		last = last->next;
	}
	return 0;
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
	int ret, ret1;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Create a new xml document */
	docp = xmlParseMemory(arg->value.str.val, arg->value.str.len);
	if (!docp) {
		RETURN_FALSE;
	}
	ret = zend_list_insert(docp, le_domxmldocp);

	/* construct the document is a php object for return */
	object_init_ex(return_value, domxmldoc_class_entry_ptr);
	add_property_resource(return_value, "doc", ret);
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	zend_list_addref(ret);

	/* get the root and add as a property to the document */
	root = docp->root;
	if (!root) {
		RETURN_FALSE;
	}
	ret1 = zend_list_insert(root, le_domxmlnodep);
//	add_property_resource(return_value, "root", ret1);

	/* construct an object with some methods */
	MAKE_STD_ZVAL(proot);
	object_init_ex(proot, domxmlnode_class_entry_ptr);
	add_property_resource(proot, "node", ret1);
	add_property_long(proot, "type", root->type);
	add_property_stringl(proot, "name", (char *) root->name, strlen(root->name), 1);
	if(root->content)
		add_property_stringl(proot, "content", (char *) root->content, strlen(root->content), 1);
	/* Get the array of children of the root and add as property children */
	if(0 == node_children(&children, root))
		zend_hash_update(proot->value.obj.properties, "children", strlen("children")+1, (void *) &children, sizeof(zval *), NULL);
	if(0 == node_attributes(&attributes, root))
		zend_hash_update(proot->value.obj.properties, "attributes", strlen("attributes")+1, (void *) &attributes, sizeof(zval *), NULL);
	zend_list_addref(ret1);
	/* add the new root object to the document */
	zend_hash_update(return_value->value.obj.properties, "root", strlen("root")+1, (void *) &proot, sizeof(zval *), NULL);
}
/* }}} */

#endif /* HAVE_DOMXML */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
