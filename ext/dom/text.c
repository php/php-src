/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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

/*
* class domtext extends domcharacterdata 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1312295772
* Since: 
*/

zend_function_entry php_dom_text_class_functions[] = {
	PHP_FALIAS(splitText, dom_text_split_text, NULL)
	PHP_FALIAS(isWhitespaceInElementContent, dom_text_is_whitespace_in_element_content, NULL)
	PHP_FALIAS(replaceWholeText, dom_text_replace_whole_text, NULL)
	PHP_FALIAS(domtext, dom_text_text, NULL)
	{NULL, NULL, NULL}
};

/* {{{ proto domtext_text([string value]); */
PHP_FUNCTION(dom_text_text)
{

	zval *id;
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *value = NULL;
	int value_len;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &value, &value_len) == FAILURE) {
		return;
	}

	nodep = xmlNewText((xmlChar *) value);

	if (!nodep)
		RETURN_FALSE;

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = (xmlNodePtr)intern->ptr;
		if (oldnode != NULL) {
			php_libxml_node_free_resource(oldnode  TSRMLS_CC);
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern TSRMLS_CC);
	}
}
/* }}} end dom_text_text */

/* {{{ proto wholeText	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-wholeText
Since: DOM Level 3
*/
int dom_text_whole_text_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr node;
	xmlChar *wholetext;

	node = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	wholetext = xmlNodeListGetString(node->doc, node, 1);
	ZVAL_STRING(*retval, wholetext, 1);

	xmlFree(wholetext);

	return SUCCESS;
}

/* }}} */


/* {{{ proto domtext dom_text_split_text(unsigned long offset);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-38853C1D
Since: 
*/
PHP_FUNCTION(dom_text_split_text)
{
	xmlChar    *cur;
	xmlChar    *first;
	xmlChar    *second;
	xmlNodePtr  node;
	xmlNodePtr  nnode;
	long        offset;
	int         ret;
	int         length;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}

	if (node->type != XML_TEXT_NODE) {
		RETURN_FALSE;
	}

	cur = xmlNodeListGetString(node->doc, node, 1);
	if (cur == NULL) {
		RETURN_FALSE;
	}
	length = xmlStrlen(cur);

	if (offset > length || offset < 0) {
		xmlFree(cur);
		RETURN_FALSE;
	}

	first = xmlStrndup(cur, offset);
	second = xmlStrdup(cur + offset);
	
	xmlFree(cur);

	xmlNodeSetContentLen(node, first, offset);
	nnode = xmlNewDocText(node->doc, second);
	
	xmlFree(first);
	xmlFree(second);

	if (node->parent != NULL) {
		nnode->type = XML_ELEMENT_NODE;
		xmlAddNextSibling(node, nnode);
		nnode->type = XML_TEXT_NODE;
	}
	
	return_value = php_dom_create_object(nnode, &ret, NULL, return_value, intern TSRMLS_CC);
}
/* }}} end dom_text_split_text */


/* {{{ proto boolean dom_text_is_whitespace_in_element_content();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-isWhitespaceInElementContent
Since: DOM Level 3
*/
PHP_FUNCTION(dom_text_is_whitespace_in_element_content)
{
	xmlNodePtr  node;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	DOM_NO_ARGS();

	if (xmlIsBlankNode(node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_text_is_whitespace_in_element_content */


/* {{{ proto domtext dom_text_replace_whole_text(string content);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-replaceWholeText
Since: DOM Level 3
*/
PHP_FUNCTION(dom_text_replace_whole_text)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_text_replace_whole_text */
#endif
