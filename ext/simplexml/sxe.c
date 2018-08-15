/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

PHP_SXE_API zend_class_entry *ce_SimpleXMLIterator = NULL;
PHP_SXE_API zend_class_entry *ce_SimpleXMLElement;

#include "php_simplexml_exports.h"

/* {{{ proto void SimpleXMLIterator::rewind()
 Rewind to first element */
PHP_METHOD(ce_SimpleXMLIterator, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_sxe_rewind_iterator(Z_SXEOBJ_P(getThis()));
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::valid()
 Check whether iteration is valid */
PHP_METHOD(ce_SimpleXMLIterator, valid)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(!Z_ISUNDEF(sxe->iter.data));
}
/* }}} */

/* {{{ proto SimpleXMLIterator SimpleXMLIterator::current()
 Get current element */
PHP_METHOD(ce_SimpleXMLIterator, current)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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
PHP_METHOD(ce_SimpleXMLIterator, key)
{
	xmlNodePtr curnode;
	php_sxe_object *intern;
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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
PHP_METHOD(ce_SimpleXMLIterator, next)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_sxe_move_forward_iterator(Z_SXEOBJ_P(getThis()));
}
/* }}} */

/* {{{ proto bool SimpleXMLIterator::hasChildren()
 Check whether element has children (elements) */
PHP_METHOD(ce_SimpleXMLIterator, hasChildren)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());
	php_sxe_object *child;
	xmlNodePtr      node;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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
PHP_METHOD(ce_SimpleXMLIterator, getChildren)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (Z_ISUNDEF(sxe->iter.data) || sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* return NULL */
	}

	data = &sxe->iter.data;
	ZVAL_COPY_DEREF(return_value, data);
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
	PHP_FE_END
};
/* }}} */

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

	INIT_CLASS_ENTRY_EX(sxi, "SimpleXMLIterator", sizeof("SimpleXMLIterator") - 1, funcs_SimpleXMLIterator);
	ce_SimpleXMLIterator = zend_register_internal_class_ex(&sxi, ce_SimpleXMLElement);
	ce_SimpleXMLIterator->create_object = ce_SimpleXMLElement->create_object;

	zend_class_implements(ce_SimpleXMLIterator, 1, spl_ce_RecursiveIterator);
	zend_class_implements(ce_SimpleXMLIterator, 1, zend_ce_countable);

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
