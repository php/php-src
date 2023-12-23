/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "dom_ce.h"

/*
* class DOMText extends DOMCharacterData
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1312295772
* Since:
*/

/* {{{ */
PHP_METHOD(DOMText, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *value = NULL;
	size_t value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	nodep = xmlNewText((xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMText::__construct */

/* {{{ wholeText	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-wholeText
Since: DOM Level 3
*/
zend_result dom_text_whole_text_read(dom_object *obj, zval *retval)
{
	xmlNodePtr node;
	xmlChar *wholetext = NULL;

	node = dom_object_get_node(obj);

	if (node == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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

	if (wholetext != NULL) {
		ZVAL_STRING(retval, (char *) wholetext);
		xmlFree(wholetext);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-38853C1D
Modern spec URL: https://dom.spec.whatwg.org/#dom-text-splittext
Since:
*/
PHP_METHOD(DOMText, splitText)
{
	zval       *id;
	xmlChar    *cur;
	xmlChar    *first;
	xmlChar    *second;
	xmlNodePtr  node;
	xmlNodePtr  nnode;
	zend_long        offset;
	int         length;
	dom_object	*intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &offset) == FAILURE) {
		RETURN_THROWS();
	}
	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	if (offset < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (node->type != XML_TEXT_NODE && node->type != XML_CDATA_SECTION_NODE) {
		/* TODO Add warning? */
		RETURN_FALSE;
	}

	cur = node->content;
	if (cur == NULL) {
		/* TODO Add warning? */
		RETURN_FALSE;
	}
	length = xmlUTF8Strlen(cur);

	if (ZEND_LONG_INT_OVFL(offset) || (int)offset > length) {
		if (php_dom_follow_spec_intern(intern)) {
			php_dom_throw_error(INDEX_SIZE_ERR, /* strict */ true);
		}
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, (int)offset);
	second = xmlUTF8Strsub(cur, (int)offset, (int)(length - offset));

	xmlNodeSetContent(node, first);
	nnode = xmlNewDocText(node->doc, second);

	xmlFree(first);
	xmlFree(second);

	if (nnode == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	if (node->parent != NULL) {
		nnode->type = XML_ELEMENT_NODE;
		xmlAddNextSibling(node, nnode);
		nnode->type = XML_TEXT_NODE;
	}

	php_dom_create_object(nnode, return_value, intern);
}
/* }}} end dom_text_split_text */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-isWhitespaceInElementContent
Since: DOM Level 3
*/
PHP_METHOD(DOMText, isWhitespaceInElementContent)
{
	zval       *id;
	xmlNodePtr  node;
	dom_object	*intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	if (xmlIsBlankNode(node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_text_is_whitespace_in_element_content */

#endif
