/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
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
	PHP_FE(getdom,	NULL)
	PHP_FE(domxml_root,	NULL)
	PHP_FE(domxml_attributes,	NULL)
	PHP_FE(domxml_getattr,	NULL)
	PHP_FE(domxml_setattr,	NULL)
	PHP_FE(domxml_children,	NULL)
	PHP_FALIAS(dom,		getdom,	NULL)
	{NULL, NULL, NULL}
};


static zend_function_entry php_domxmldoc_class_functions[] = {
	PHP_FALIAS(root,	domxml_root,	NULL)
	PHP_FALIAS(intdtd,	domxml_intdtd,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnode_class_functions[] = {
	PHP_FALIAS(lastchild,	domxml_lastchild,	NULL)
	PHP_FALIAS(children,	domxml_children,	NULL)
	PHP_FALIAS(parent,	domxml_parent,		NULL)
	PHP_FALIAS(getattr,	domxml_getattr,		NULL)
	PHP_FALIAS(setattr,	domxml_setattr,		NULL)
	PHP_FALIAS(attributes,	domxml_attributes,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlattr_class_functions[] = {
	PHP_FALIAS(name,	domxml_attrname,	NULL)
	{NULL, NULL, NULL}
};

php3_module_entry php3_domxml_module_entry = {
	"DOM", php_domxml_functions, PHP_MINIT(domxml), NULL, NULL, NULL, PHP_MINFO(domxml), STANDARD_MODULE_PROPERTIES
};


PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry domxmldoc_class_entry;
	zend_class_entry domxmldtd_class_entry;
	zend_class_entry domxmlnode_class_entry;
	zend_class_entry domxmlattr_class_entry;

	le_domxmldocp = register_list_destructors(xmlFreeDoc, NULL);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	le_domxmlnodep = register_list_destructors(xmlFreeNode, NULL);
	le_domxmlattrp = register_list_destructors(xmlFreeProp, NULL);
	*/

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
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_attrname)
{
	pval *id, **tmp;
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
	while(attr) {
		ret = zend_list_insert(attr, le_domxmlattrp);

		/* construct an object with some methods */
		object_init_ex(return_value, domxmlattr_class_entry_ptr);
		add_property_long(return_value, "attribute", ret);
		attr = attr->next;
	}
}
/* }}} */

/* {{{ proto string domxml_lastchild([int node])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_lastchild)
{
	pval *id, **tmp;
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
	add_property_long(return_value, "node", ret);
	add_property_long(return_value, "type", last->type);
	add_property_stringl(return_value, "name", (char *) last->name, strlen(last->name), 1);
	if(last->content)
		add_property_stringl(return_value, "content", (char *) last->content, strlen(last->content), 1);
}
/* }}} */

/* {{{ proto string domxml_parent([int node])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_parent)
{
	pval *id, **tmp;
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
	add_property_long(return_value, "node", ret);
	add_property_long(return_value, "type", last->type);
	add_property_stringl(return_value, "name", (char *) last->name, strlen(last->name), 1);
	if(last->content)
		add_property_stringl(return_value, "content", (char *) last->content, strlen(last->content), 1);
}
/* }}} */

/* {{{ proto string domxml_children([int node])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_children)
{
	pval *id, **tmp;
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
		add_property_long(child, "node", ret);
		add_property_long(child, "type", last->type);
		zend_hash_next_index_insert(return_value->value.ht, &child, sizeof(zval *), NULL);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto string domxml_getattr([int node,] string attrname)
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_getattr)
{
	pval *id, *arg1, **tmp;
	int id_to_find;
	xmlNode *nodep, *last;
	char *value;
	int type;
	int ret;
	
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
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_setattr)
{
	pval *id, *arg1, *arg2, **tmp;
	int id_to_find;
	xmlNode *nodep, *last;
	xmlAttr *attr;
	int type;
	int ret;
	
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
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_attributes)
{
	pval *id, *arg1, **tmp;
	int id_to_find;
	xmlNode *nodep;
	xmlAttr *attr;
	char *value;
	int type;
	int ret;
	
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
		add_assoc_string(return_value, (char *) attr->name, attr->val->content, 1);
		attr = attr->next;
	}
}
/* }}} */

/* {{{ proto string domxml_root([int doc_handle])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_root)
{
	pval *id, **tmp;
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
	add_property_long(return_value, "node", ret);
	add_property_long(return_value, "type", node->type);
	add_property_stringl(return_value, "name", (char *) node->name, strlen(node->name), 1);
	if(node->content)
		add_property_stringl(return_value, "content", (char *) node->content, strlen(node->content), 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto string domxml_dtd([int dir_handle])
   Read directory entry from dir_handle */
PHP_FUNCTION(domxml_intdtd)
{
	pval *id, **tmp;
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
	add_property_long(return_value, "dtd", ret);
	if(dtd->ExternalID)
		add_property_string(return_value, "extid", (char *) dtd->ExternalID, 1);
	add_property_string(return_value, "sysid", (char *) dtd->SystemID, 1);
	add_property_string(return_value, "name", (char *) dtd->name, 1);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto class dom(string directory)
   Directory class with properties, handle and class and methods read, rewind and close */
PHP_FUNCTION(getdom)
{
	pval *arg;
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
	add_property_long(return_value, "doc", ret);
	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
	zend_list_addref(ret);
}
/* }}} */

#endif /* HAVE_DOMXML */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
