/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"

#include "ext/spl/php_spl.h"
#include "ext/spl/spl_iterators.h"
#include "sxe.h"

zend_class_entry *ce_SimpleXMLIterator = NULL;
zend_class_entry *ce_SimpleXMLElement;

#include "php_simplexml_exports.h"

/* {{{ proto void SimpleXMLIterator::rewind()
 Rewind to first element */
PHP_METHOD(ce_SimpleXMLIterator, rewind)
{
	php_sxe_iterator iter;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	iter.sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	ce_SimpleXMLElement->iterator_funcs.funcs->rewind((zend_object_iterator*)&iter TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::valid()
 Check whether iteration is valid */
PHP_METHOD(ce_SimpleXMLIterator, valid)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(sxe->iter.data);
}
/* }}} */

/* {{{ proto SimpleXMLIterator SimpleXMLIterator::current()
 Get current element */
PHP_METHOD(ce_SimpleXMLIterator, current)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!sxe->iter.data) {
		return; /* return NULL */
	}

	RETURN_ZVAL(sxe->iter.data, 1, 0);
}
/* }}} */

/* {{{ proto string SimpleXMLIterator::key()
 Get name of current child element */
PHP_METHOD(ce_SimpleXMLIterator, key)
{
	xmlNodePtr curnode;
	php_sxe_object *intern;
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!sxe->iter.data) {
		RETURN_FALSE;
	}

	intern = (php_sxe_object *)zend_object_store_get_object(sxe->iter.data TSRMLS_CC);
	if (intern != NULL && intern->node != NULL) {
		curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->node)->node;
		RETURN_STRINGL((char*)curnode->name, xmlStrlen(curnode->name), 1);
	}
    
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void SimpleXMLIterator::next()
 Move to next element */
PHP_METHOD(ce_SimpleXMLIterator, next)
{
	php_sxe_iterator iter;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	iter.sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	ce_SimpleXMLElement->iterator_funcs.funcs->move_forward((zend_object_iterator*)&iter TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::hasChildren()
 Check whether element has children (elements) */ 
PHP_METHOD(ce_SimpleXMLIterator, hasChildren)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	php_sxe_object *child;
	xmlNodePtr      node;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!sxe->iter.data || sxe->iter.type == SXE_ITER_ATTRLIST) {
		RETURN_FALSE;
	}
	child = php_sxe_fetch_object(sxe->iter.data TSRMLS_CC);

	GET_NODE(child, node);
	if (node) {
		node = node->children;
	}
	while (node && node->type != XML_ELEMENT_NODE) {
		node = node->next;
	}
	RETURN_BOOL(node ? 1 : 0);
}
/* }}} */

/* {{{ proto SimpleXMLIterator SimpleXMLIterator::getChildren()
 Get child element iterator */ 
PHP_METHOD(ce_SimpleXMLIterator, getChildren)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!sxe->iter.data || sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* return NULL */
	}
	RETURN_ZVAL(sxe->iter.data, 1, 0);
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_simplexmliterator__void, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry funcs_SimpleXMLIterator[] = {
	PHP_ME(ce_SimpleXMLIterator, rewind,                 arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, valid,                  arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, current,                arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, key,                    arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, next,                   arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, hasChildren,            arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	PHP_ME(ce_SimpleXMLIterator, getChildren,            arginfo_simplexmliterator__void, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

PHP_MINIT_FUNCTION(sxe) /* {{{ */
{
	zend_class_entry **pce;
	zend_class_entry sxi;

	if (zend_hash_find(CG(class_table), "simplexmlelement", sizeof("SimpleXMLElement"), (void **) &pce) == FAILURE) {
		ce_SimpleXMLElement  = NULL;
		ce_SimpleXMLIterator = NULL;
		return SUCCESS; /* SimpleXML must be initialized before */
	}

	ce_SimpleXMLElement = *pce;

	INIT_CLASS_ENTRY_EX(sxi, "SimpleXMLIterator", strlen("SimpleXMLIterator"), funcs_SimpleXMLIterator);
	ce_SimpleXMLIterator = zend_register_internal_class_ex(&sxi, ce_SimpleXMLElement, NULL TSRMLS_CC);
	ce_SimpleXMLIterator->create_object = ce_SimpleXMLElement->create_object;

	zend_class_implements(ce_SimpleXMLIterator TSRMLS_CC, 1, spl_ce_RecursiveIterator);
	zend_class_implements(ce_SimpleXMLIterator TSRMLS_CC, 1, spl_ce_Countable);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
