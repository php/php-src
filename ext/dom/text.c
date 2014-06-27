/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"
#include "dom_ce.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_text_split_text, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_text_is_whitespace_in_element_content, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_text_replace_whole_text, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_text_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMText extends DOMCharacterData 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1312295772
* Since: 
*/

const zend_function_entry php_dom_text_class_functions[] = {
	PHP_FALIAS(splitText, dom_text_split_text, arginfo_dom_text_split_text)
	PHP_FALIAS(isWhitespaceInElementContent, dom_text_is_whitespace_in_element_content, arginfo_dom_text_is_whitespace_in_element_content)
	PHP_FALIAS(isElementContentWhitespace, dom_text_is_whitespace_in_element_content, arginfo_dom_text_is_whitespace_in_element_content)
	PHP_FALIAS(replaceWholeText, dom_text_replace_whole_text, arginfo_dom_text_replace_whole_text)
	PHP_ME(domtext, __construct, arginfo_dom_text_construct, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ proto void DOMText::__construct([string value]); */
PHP_METHOD(domtext, __construct)
{

	zval *id;
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *value = NULL;
	int value_len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, dom_domexception_class_entry, &error_handling TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|s", &id, dom_text_class_entry, &value, &value_len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
	nodep = xmlNewText((xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = dom_object_get_node(intern);
		if (oldnode != NULL) {
			php_libxml_node_free_resource(oldnode  TSRMLS_CC);
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern TSRMLS_CC);
	}
}
/* }}} end DOMText::__construct */

/* {{{ wholeText	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-wholeText
Since: DOM Level 3
*/
int dom_text_whole_text_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr node;
	xmlChar *wholetext = NULL;

	node = dom_object_get_node(obj);

	if (node == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	/* Find starting text node */
	while (node->prev && ((node->prev->type == XML_TEXT_NODE) || (node->prev->type == XML_CDATA_SECTION_NODE))) {
		node = node->prev;
	}

	/* concatenate all adjacent text and cdata nodes */
	while (node && ((node->type == XML_TEXT_NODE) || (node->type == XML_CDATA_SECTION_NODE))) {
		wholetext = xmlStrcat(wholetext, node->content);
		node = node->next;
	}

	ALLOC_ZVAL(*retval);
	if (wholetext != NULL) {
		ZVAL_STRING(*retval, wholetext, 1);
		xmlFree(wholetext);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ proto DOMText dom_text_split_text(int offset);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-38853C1D
Since: 
*/
PHP_FUNCTION(dom_text_split_text)
{
	zval       *id;
	xmlChar    *cur;
	xmlChar    *first;
	xmlChar    *second;
	xmlNodePtr  node;
	xmlNodePtr  nnode;
	long        offset;
	int         ret;
	int         length;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &id, dom_text_class_entry, &offset) == FAILURE) {
		return;
	}
	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	if (node->type != XML_TEXT_NODE && node->type != XML_CDATA_SECTION_NODE) {
		RETURN_FALSE;
	}

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}
	length = xmlUTF8Strlen(cur);

	if (offset > length || offset < 0) {
		xmlFree(cur);
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, offset);
	second = xmlUTF8Strsub(cur, offset, length - offset);
	
	xmlFree(cur);

	xmlNodeSetContent(node, first);
	nnode = xmlNewDocText(node->doc, second);
	
	xmlFree(first);
	xmlFree(second);

	if (nnode == NULL) {
		RETURN_FALSE;
	}

	if (node->parent != NULL) {
		nnode->type = XML_ELEMENT_NODE;
		xmlAddNextSibling(node, nnode);
		nnode->type = XML_TEXT_NODE;
	}
	
	return_value = php_dom_create_object(nnode, &ret, return_value, intern TSRMLS_CC);
}
/* }}} end dom_text_split_text */

/* {{{ proto boolean dom_text_is_whitespace_in_element_content();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-isWhitespaceInElementContent
Since: DOM Level 3
*/
PHP_FUNCTION(dom_text_is_whitespace_in_element_content)
{
	zval       *id;
	xmlNodePtr  node;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &id, dom_text_class_entry) == FAILURE) {
		return;
	}
	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	if (xmlIsBlankNode(node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_text_is_whitespace_in_element_content */

/* {{{ proto DOMText dom_text_replace_whole_text(string content);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-replaceWholeText
Since: DOM Level 3
*/
PHP_FUNCTION(dom_text_replace_whole_text)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_text_replace_whole_text */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
