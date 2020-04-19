/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"

#include "php_simplexml.h"
#include "ext/spl/php_spl.h"
#include "ext/spl/spl_iterators.h"
#include "sxe.h"
#include "sxe_arginfo.h"

PHP_SXE_API zend_class_entry *ce_SimpleXMLIterator = NULL;
PHP_SXE_API zend_class_entry *ce_SimpleXMLElement;

#include "php_simplexml_exports.h"

/* {{{ proto void SimpleXMLIterator::rewind()
 Rewind to first element */
PHP_METHOD(SimpleXMLIterator, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	php_sxe_rewind_iterator(Z_SXEOBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::valid()
 Check whether iteration is valid */
PHP_METHOD(SimpleXMLIterator, valid)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(!Z_ISUNDEF(sxe->iter.data));
}
/* }}} */

/* {{{ proto SimpleXMLIterator SimpleXMLIterator::current()
 Get current element */
PHP_METHOD(SimpleXMLIterator, current)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(ZEND_THIS);
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_ISUNDEF(sxe->iter.data)) {
		return; /* return NULL */
	}

	data = &sxe->iter.data;
	ZVAL_COPY_DEREF(return_value, data);
}
/* }}} */

/* {{{ proto string SimpleXMLIterator::key()
 Get name of current child element */
PHP_METHOD(SimpleXMLIterator, key)
{
	xmlNodePtr curnode;
	php_sxe_object *intern;
	php_sxe_object *sxe = Z_SXEOBJ_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_ISUNDEF(sxe->iter.data)) {
		RETURN_FALSE;
	}

	intern = Z_SXEOBJ_P(&sxe->iter.data);
	if (intern != NULL && intern->node != NULL) {
		curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->node)->node;
		RETURN_STRINGL((char*)curnode->name, xmlStrlen(curnode->name));
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void SimpleXMLIterator::next()
 Move to next element */
PHP_METHOD(SimpleXMLIterator, next)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	php_sxe_move_forward_iterator(Z_SXEOBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::hasChildren()
 Check whether element has children (elements) */
PHP_METHOD(SimpleXMLIterator, hasChildren)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(ZEND_THIS);
	php_sxe_object *child;
	xmlNodePtr      node;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_ISUNDEF(sxe->iter.data) || sxe->iter.type == SXE_ITER_ATTRLIST) {
		RETURN_FALSE;
	}
	child = Z_SXEOBJ_P(&sxe->iter.data);

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
PHP_METHOD(SimpleXMLIterator, getChildren)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(ZEND_THIS);
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_ISUNDEF(sxe->iter.data) || sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* return NULL */
	}

	data = &sxe->iter.data;
	ZVAL_COPY_DEREF(return_value, data);
}

PHP_MINIT_FUNCTION(sxe) /* {{{ */
{
	zend_class_entry *pce;
	zend_class_entry sxi;

	if ((pce = zend_hash_str_find_ptr(CG(class_table), "simplexmlelement", sizeof("SimpleXMLElement") - 1)) == NULL) {
		ce_SimpleXMLElement  = NULL;
		ce_SimpleXMLIterator = NULL;
		return SUCCESS; /* SimpleXML must be initialized before */
	}

	ce_SimpleXMLElement = pce;

	INIT_CLASS_ENTRY_EX(sxi, "SimpleXMLIterator", sizeof("SimpleXMLIterator") - 1, class_SimpleXMLIterator_methods);
	ce_SimpleXMLIterator = zend_register_internal_class_ex(&sxi, ce_SimpleXMLElement);
	ce_SimpleXMLIterator->create_object = ce_SimpleXMLElement->create_object;

	zend_class_implements(ce_SimpleXMLIterator, 1, spl_ce_RecursiveIterator);
	zend_class_implements(ce_SimpleXMLIterator, 1, zend_ce_countable);

	return SUCCESS;
}
/* }}} */
